// WebThreadSafe.cpp : Defines the entry point for the console application.
//

#include "header.h"
#include "ConnectionManager.h"
#include "Accepter.h"
#include "Processor.h"
#include "ThreadSafeStore.h"
#include "ThreadSafeQueue.h"

int bind_port = 444;

static int actionQueue(connection *con)
{
	std::string key = con->parser->get("key");
	std::string value = con->parser->get("value");
	//int max_id = utils::ft_atoi(con->parser->get("max_id").c_str());

	if (key.length() <= 0)
	{
		std::string body = "Undefined key by get queue";
		std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;

		send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
		return 0;
	}

	if (value.length() > 0)
	{
		ThreadSafeQueue::Instance()->push(key.data(), value.data(), value.length());

		std::string body = "Ok";
		std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;

		send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
		return 1;
	}
	else {
		
		queue* q = ThreadSafeQueue::Instance()->getQueue(key.c_str());

		int count = q->list.size();

		printf("get queue %s size %d\r\n", key.c_str(), count);
		if (count == 0)
		{
			std::string body = "[]";
			std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;
			send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
			return 1;
		}

		std::vector< std::string > json_items;
		
		for (int i = 0; i < count; i++)
		{
			store_data *data = q->list[i];
			json_items.push_back("{ \"id\": " + _itoa(i) + ", \"value\": \"" + data->value + "\"}");
			//printf("row: %s\r\n", row.c_str());
			data->flush();
			free(data);
		}
		
		std::string body = "[" + utils::implode(json_items, ",") + "]";

		q->list.clear();

		std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;
		

		send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);


		return 1;
	}

	return 1;
}

static int actionStore(connection *con)
{
	std::string key = con->parser->get("key");
	std::string value = con->parser->get("value");
	std::string clear = con->parser->get("clear");

	if (key.length() <= 0)
	{
		std::string body = "Undefined key by get request";
		std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;

		send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
		return 0;
	}

	if (clear.length() > 0)
	{
		ThreadSafeStore::Instance()->set(key.data(), "", 0);
		std::string body = "Clear Ok";
		std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;

		send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);

		return 1;
	}


	if (value.length() > 0)
	{
		ThreadSafeStore::Instance()->set(key.data(), value.data(), value.length());

		std::string body = "Ok";
		std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;

		send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
		return 1;
	}
	else {

		store_data *data = ThreadSafeStore::Instance()->get(key.data());
		if (data)
		{
			std::string body(data->value, data->len);
			std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;
			send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
		}
		else {
			std::string body = "empty";
			std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;
			send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);
		}


		return 1;
	}

	return 1;
}

static int router(connection *con)
{
	std::string action = con->parser->get("action");

	if (action.find("store") == 0)
	{
		actionStore(con);
		return 1;
	}

	if (action.find("queue") == 0)
	{
		actionQueue(con);
		return 1;
	}

	std::string body = "Not find router ?action=" + action;
	std::string responce = "HTTP/1.1 200 OK\r\nContent-length: " + utils::xitoa(body.length(), 10) + "\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" + body;
	send(con->socket, responce.data(), responce.length(), MSG_NOSIGNAL);

	return 0;
}

static int http_serv(connection *con)
{
	char buff[4096];
	int len = recv(con->socket, buff, 4096, 0);

	if (len <= 0)
	{
		printf("Error reciver 0 bytes\r\n");
		return 0;
	}


	std::string request_string(buff, len);
	con->parser = new Processor();
	con->parser->parseRequest(request_string);

	router(con);

	delete con->parser;

	return 0;
}

static void client_callback(connection *con)
{
	http_serv(con);
	con->done = 1;
	closesocket(con->socket);
}

void usage()
{
	printf("action [GET] ?action=queue or ?action=store\r\n");
	printf("store methods:[GET]\r\n");
	printf("get: &key=string\r\n");
	printf("set: &key=string&value=string\r\n");
	printf("clear: &clear=key\r\n");

	printf("queue methods: [GET]\r\n");
	printf("push: &key=string&value=string\r\n");
	printf("get: &key=string\r\n");
}

void start()
{
	usage();
	Accepter::Instance()->listener(bind_port, client_callback);

}

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup function failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}

	start();

	return 0;
}
#else

void posix_death_signal(int signum)
{
	printf("Server Error handler\r\nPlease Restart\r\n");
	signal(signum, SIG_DFL);
	exit(42);
}

int main(int argc, char* argv[])
{
	printf("Linux platform\r\n");

	signal(SIGSEGV, posix_death_signal);
	start();
}

#endif
