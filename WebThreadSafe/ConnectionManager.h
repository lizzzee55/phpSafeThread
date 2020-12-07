#include <vector>
#include "Singleton.h"
#include "Processor.h"

class connection
{
public:
	int index;
	SOCKET socket;
	volatile int done;
	struct sockaddr_in addr;
	char socket_ip[21];
	int socket_port;
	HANDLE ptr;
	Processor *parser;

	connection()
	{
		done = 0;
		memset(socket_ip, 0, sizeof socket_ip);
	}
};


class ConnectionManager
{
	DECLARE_SINGLETON(ConnectionManager)
public:

	ConnectionManager();
	void add(connection *con);
	connection* get(int socket);

	int connection_size = 0;


	std::vector< connection* > con_list;

	pthread_mutex_t mutex;

};

#pragma once


inline ConnectionManager::ConnectionManager()
{
	pthread_mutex_init(&mutex, NULL);
}

inline connection* ConnectionManager::get(int socket)
{
	connection *con = NULL;

	pthread_mutex_lock(&mutex);


	size_t i;
	for (i = 0; i<con_list.size();i++) {
		connection *tmp = con_list[i];

		if (tmp->socket == socket)
		{
			con = tmp;
			break;
		}

	}

	pthread_mutex_unlock(&mutex);

	return con;

}


inline void ConnectionManager::add(connection *con)
{

	pthread_mutex_lock(&mutex);
	size_t i;
	for (i = 0; i<con_list.size();i++) {
		connection *tmp = con_list[i];
		if (tmp->done)
		{
			joinThread(tmp->ptr);
			con_list.erase(con_list.begin() + i);
			delete tmp;
		}
	}
	con_list.push_back(con);

	printf("[%d] threads connection\r\n", con_list.size());
	pthread_mutex_unlock(&mutex);

}