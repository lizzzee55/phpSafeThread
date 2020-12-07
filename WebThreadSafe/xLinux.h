#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <chrono>
#include <stdio.h>
#include <vector>

class utils
{
public:
	static int xsleep(int ms)
	{
#ifdef WIN32
		Sleep(ms);
#else
		usleep(ms * 1000);
#endif
		return 0;
	}

	static uint64_t timeSinceEpochMillisec() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}


	static char* itoa(unsigned long long  value, char str[], int radix)
	{
		char        buf[66];
		char*       dest = buf + sizeof(buf);
		bool     sign = false;

		if (value == 0) {
			memcpy(str, "0", 2);
			return str;
		}

		if (radix < 0) {
			radix = -radix;
			if ((long long)value < 0) {
				value = -value;
				sign = true;
			}
		}

		*--dest = '\0';

		switch (radix)
		{
		case 16:
			while (value) {
				*--dest = '0' + (value & 0xF);
				if (*dest > '9') *dest += 'A' - '9' - 1;
				value >>= 4;
			}
			break;
		case 10:
			while (value) {
				*--dest = '0' + (value % 10);
				value /= 10;
			}
			break;

		case 8:
			while (value) {
				*--dest = '0' + (value & 7);
				value >>= 3;
			}
			break;

		case 2:
			while (value) {
				*--dest = '0' + (value & 1);
				value >>= 1;
			}
			break;

		default:            // The slow version, but universal
			while (value) {
				*--dest = '0' + (value % radix);
				if (*dest > '9') *dest += 'A' - '9' - 1;
				value /= radix;
			}
			break;
		}

		if (sign) *--dest = '-';

		memcpy(str, dest, buf + sizeof(buf) - dest);
		return str;
	}

	static std::string xitoa(int long long num, int radix = 10)
	{
		char buf[20];
		utils::itoa(num, buf, radix);
		std::string cnt = buf;
		return cnt;
	}

	static int ft_atoi(const char *str)
	{
		int	res;
		int	negative;

		negative = 1;
		res = 0;
		while (*str && (*str == ' ' || *str == '\n' || *str == '\t' ||
			*str == '\v' || *str == '\f' || *str == '\r'))
			++str;
		if (*str == '-')
			negative = -1;
		if (*str == '-' || *str == '+')
			++str;
		while (*str && *str >= '0' && *str <= '9')
		{
			res = res * 10 + (*str - 48);
			++str;
		}
		return (res * negative);
	}

	struct s_proto {
		int protocol;
		std::string hostname;
		int port;
		std::string uri;
	};

	static s_proto parseUrl_latest(std::string url) {


		s_proto proto;
		proto.protocol = 0;
		proto.port = 80;
		proto.hostname = "localhost";

		//auto t = explode("privet ++hhas ++askjd++", "++");

		//exit(1);

		std::vector<std::string> v = explode(url, "/");

		//myprintf("explode ok %d\r\n", v.size());
		if (v.size() < 2)
		{
			//myprintf("Error count args %d\r\n", v.size());
			return proto;
		}

		//myprintf("Error count args %d\r\n", v.size());

		//myprintf("for explode %s %s \r\n", v[0].c_str(), v[2].c_str());

		if (v[2].length() == 0)
		{
			return proto;
		}
		std::vector<std::string> s = explode(v[2], ":");

		//myprintf("%s %s\r\n", s[0].c_str(), s[1].c_str());

		if (s.size() == 0)
		{
			proto.hostname = v[2];
		}
		else
		{
			proto.hostname = s[0];
			proto.port = atoi(s[1].c_str());
		}
		//myprintf("Error count args\r\n");
		if (v[0].find("https") == 0)
		{
			proto.protocol = 1;
		}

		v = std::vector<std::string>(v.begin() + 3, v.end());

		std::string uri = "/" + implode(v, "/");




		//proto.port = v[2];
		//"http://sf.aferon.com:8080/asdas/asd/asd/asd/"
		return proto;
	}

	static std::string implode(std::vector<std::string> arr, const char *delim)
	{
		std::string result;
		for (int i = 0; i < arr.size(); i++)
		{
			result += arr[i] + delim;
		}

		result = result.substr(0, result.length() - strlen(delim));

		return result;
	}

	static int asciitolower(char in) {
		if (in <= 'Z' && in >= 'A')
			return in - ('Z' - 'z');
		return in;
	}

	static void ft_tolower(std::string &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), asciitolower);
	}


	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	static std::vector<std::string> explode(std::string str, const char *delim)
	{

		str += delim;

		std::vector<std::string> result;

		while (true)
		{


			int pos = str.find(delim);
			if (pos == std::string::npos)
			{
				break;
			}

			if (str.length() == 0)
			{
				break;
			}

			//myprintf("pos %d\r\n", pos);

			std::string part = str.substr(0, pos);

			//myprintf("part %s\r\n", part.c_str());
			result.push_back(part);

			pos = pos + strlen(delim);
			str = str.substr(pos, str.length() - pos);
			//myprintf("res %s\r\n", str.c_str());

			//break;
		}


		return result;
	}
};