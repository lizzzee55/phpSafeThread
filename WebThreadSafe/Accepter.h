#include "Singleton.h"
#include <vector>
class Accepter
{
	DECLARE_SINGLETON(Accepter)
public:
	Accepter();
	int listener(int port, void(*f)(connection*));
};

#pragma once
#include "ConnectionManager.h"

inline Accepter::Accepter()
{
}

inline int Accepter::listener(int port, void(*callback)(connection*))
{
	printf("Bind %d\r\n", port);
	int fd = -1;


	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		printf("Error create server socket\r\n", port);
		return 0;
	}

	//#ifdef LINUX
	//https://stackoverflow.com/questions/47179793/how-to-gracefully-handle-accept-giving-emfile-and-close-the-connection

	int enable = 1;
	int opt = TRUE;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int)) < 0)
		printf("setsockopt(SO_REUSEADDR) failed");
	else
	{
		//printf("Overbind enable\r\n");
	}

	//#endif

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof serv_addr);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);


	if (bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error bind on: %d port\r\n", port);
		close(fd);
		return 0;
	}

	if (listen(fd, 128) < 0)
	{
		printf("Error listen on: %d port\r\n", port);
		return 0;
	}

	while (1) {


		connection *con = new connection();
		con->socket_port = port;
		con->done = 0;

		socklen_t clen = sizeof con->addr;
		con->socket = accept(fd, (struct sockaddr *)&con->addr, &clen);

		if (con->socket <= 0)
		{
			//printf("Listener down %d\r\n", serv->fd);
			//break;
			continue;
		}


		sprintf(con->socket_ip, "%s", inet_ntoa(con->addr.sin_addr));
		printf("Accept socket %d %s:%d\r\n", con->socket, con->socket_ip, con->socket_port);
		ConnectionManager::Instance()->add(con);

		//callback(con);

		con->ptr = createThread((LPTHREAD_START_ROUTINE)callback, con);

		if (!con->ptr)
		{
			printf("CreateThread failed. OOM?\n");
		}


	}

	closesocket(fd);


	printf("Server port closed %d\r\n", port);
	//delete serv;

	return 0;
};