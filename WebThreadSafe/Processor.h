#include <algorithm>
#include "Singleton.h"
#include <vector>
#include "xLinux.h"
#include <map>
#include <sstream>

class Processor {

public:
	std::string firstLine;

	std::string html;
	std::string header;
	std::string body;


	/* most key headers */
	std::string method;
	std::string uri;
	std::string protocol;

	/* types */
	std::string contentType;
	std::string contentEncode;
	std::string transferEncoding;
	std::string host;
	int port;
	int contentLength;
	int code;

	int is_gzip;
	int is_chunked;

	std::string referer;

	Processor();
	void reload(std::string html);

	int splitHtml();


	int parseRequest(std::string html);
	int parseResponse(std::string html);
	std::string getHeader(std::string key);
	int removeChunk();

	void parseData();
	std::string get(std::string name);

	std::map <std::string, std::string> dataList;
	std::map <std::string, std::string> headerList;

	std::string tmp;

protected:
	void serializeHeader();
	int parseHeader();
};

#pragma once
inline Processor::Processor()
{

	this->is_gzip = 0;
	this->is_chunked = 0;
	this->contentLength = -1;
};

inline void Processor::reload(std::string html)
{
	this->html = html;
	this->is_gzip = 0;
	this->is_chunked = 0;
	this->contentLength = -1;
};


inline std::string Processor::getHeader(std::string key) {

	return this->headerList[key];
};

inline int Processor::parseHeader() {

	std::string line;

	std::stringstream fd;
	fd << this->header;

	int numLine = 0;

	while (getline(fd, line))
	{
		if (line.length() == 0)
			break;

		if (numLine == 0)
		{
			this->firstLine = line;
		}
		else
		{


			int posDel = line.find(":");
			if (posDel != 0 && posDel != std::string::npos)
			{

				std::string key = line.substr(0, posDel);
				std::string val = line.substr(posDel + 1, line.length() - posDel - 1);

				utils::ft_tolower(key);
				utils::trim(key);
				utils::trim(val);

				this->headerList[key] = val;
			}
		}

		numLine++;
	}

	return 1;

}

inline int Processor::splitHtml()
{
	int pos = this->html.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		this->header = this->html.substr(0, pos);
		this->body = this->html.substr(pos + 4, this->html.length());
		return 1;
	}
	return 0;
}

inline std::string Processor::get(std::string name)
{
	return dataList[name];
}

inline void Processor::parseData() {
	if (this->uri.empty()) return;

	dataList.clear();
	//std::string tmp;
	int pos = this->uri.find("?");
	if (pos != std::string::npos)
	{
		this->tmp = this->uri.substr(pos + 1, this->uri.length() - pos - 1);

		std::vector<std::string> ex = utils::explode(this->tmp, "&");
		if (this->tmp.length() > 0 && ex.size() > 0)
		{
			for (int i = 0; i < ex.size(); i++)
			{
				std::vector<std::string> ex_p = utils::explode(ex[i], "=");
				if (ex_p.size() == 2)
				{
					dataList[ex_p[0]] = ex_p[1];
				}
				else if (ex_p.size() == 1)
				{
					dataList[ex_p[0]] = "";
				}
				else {
					dataList[ex_p[0]] = ex_p[1];
				}


				//this->tmp = ex_p[0] + "=" + dataList[ex_p[0]];
			}
		}

	}
}

inline int Processor::parseRequest(std::string html) {
	this->html = html;
	if (this->splitHtml())
	{
		this->parseHeader();


		std::vector<std::string> arr = utils::explode(this->firstLine, " ");

		this->method = arr[0];
		this->uri = arr[1];
		this->protocol = arr[2];


		this->serializeHeader();
		this->parseData();

		//printf("%s %s\r\n", this->method.c_str(), host.c_str());
		return 1;
	}

	return 0;
};

inline int Processor::parseResponse(std::string html) {
	this->html = html;
	if (this->splitHtml())
	{
		this->parseHeader();

		std::vector<std::string> arr = utils::explode(this->firstLine, " ");
		this->protocol = arr[0];
		this->code = atoi(arr[1].c_str());
		this->protocol = arr[2];

		this->serializeHeader();


		printf("HTML type:%s encode:%s transfer:%s length: %d code:%d\r\n", contentType.c_str(), contentEncode.c_str(), transferEncoding.c_str(), contentLength, code);
		return 1;
	}

	return 0;
};

inline void Processor::serializeHeader()
{
	this->contentType = this->getHeader("content-type");
	this->contentEncode = this->getHeader("content-encoding");
	this->transferEncoding = this->getHeader("transfer-encoding");
	this->contentLength = atoi(this->getHeader("content-length").c_str());
	this->host = this->getHeader("host");
	this->port = 80;

	if (this->host.length() > 0)
	{
		int posDel = host.find(":");
		if (posDel != std::string::npos)
		{
			std::string portS = host.substr(posDel + 1, this->host.length() - posDel + 1);
			this->port = atoi(portS.c_str());
			this->host = this->host.substr(0, posDel);
		}
	}


	if (this->contentEncode.find("gzip") != std::string::npos)
	{
		this->is_gzip = 1;
	}

	if (this->transferEncoding.find("chunked") != std::string::npos)
	{
		this->is_chunked = 1;
	}

};

inline int Processor::removeChunk() {
	if (!this->is_chunked) return 0;

	int chunkPart = 0;
	int chunkLen = 0;
	int startChunkBlock = 0;
	int endChunkBlock = 0;

	std::string block;
	int posBlock = 0;
	std::string bodyNoChunk;

	std::string chunk = "";
	//printf("Read chunk | allLength %d\r\n", this->responce->body);


	while (true) {
		int n = (endChunkBlock) ? endChunkBlock : 0;

		//printf("start chunk block %d %d htmllen\r\n", n, pSvcInfo->html.length());

		if (n >= this->body.length()) {
			printf("break 1\r\n");
			break;
		}

		chunk = "";

		while (true) {
			if (this->body[n] == 13 && this->body[n + 1] == 10) {
				chunkLen = strtol(chunk.c_str(), NULL, 16);
				startChunkBlock = n + 2;
				endChunkBlock = startChunkBlock + chunkLen;
				break;
			}
			chunk += (char)this->body[n];
			n++;

			if (n >= this->body.length()) {
				printf("break 2\r\n");
				break;
			}
		}

		//printf("Chunk(%d) %s %d (%d - %d)\r\n", chunk.length(), chunk.c_str(), chunkLen, startChunkBlock, endChunkBlock);

		if (chunkLen == 0) {
			break;
		}

		for (int i = startChunkBlock; i < endChunkBlock; i++) {
			bodyNoChunk += (char)this->body[i];
		}

		endChunkBlock += 2;
	}

	this->body = bodyNoChunk;
	return 1;

}
