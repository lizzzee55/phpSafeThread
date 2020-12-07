#pragma once
#pragma warning(disable : 4996)
#ifdef WIN32

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <ws2tcpip.h>
#include "windows.h"
#include <tchar.h>
#include "io.h"

#define SHUT_RDWR 2
#define MSG_NOSIGNAL 0

#else
#define WSAGetLastError() 1

//#define _XOPEN_SOURCE_EXTENDED 1 //#define _OE_SOCKETS //
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <cstring>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET 0

typedef unsigned long DWORD;
typedef void* (*LPTHREAD_START_ROUTINE)(void*);
#define FALSE 0
#define TRUE 1
typedef void* LPVOID;

//typedef pthread_t HANDLE;

#define HANDLE pthread_t

#define closesocket(s) close(s)
#define Sleep(s) usleep(s * 1000)
#define SOCKET int

#endif


#include <fcntl.h>
#include <string>

#ifdef WIN32

#include <mutex>

#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_init(x, y) InitializeCriticalSection(x)
#define pthread_mutex_lock(x) EnterCriticalSection(x)
#define pthread_mutex_unlock(x) LeaveCriticalSection(x)
#define pthread_mutex_destroy(x) DeleteCriticalSection(x)


const char *lizzz_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
	struct sockaddr_storage ss;
	unsigned long s = size;

	ZeroMemory(&ss, sizeof(ss));
	ss.ss_family = af;

	switch (af) {
	case AF_INET:
		((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
		break;
	case AF_INET6:
		((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
		break;
	default:
		return NULL;
	}
	/* cannot direclty use &size because of strict aliasing rules */
	return (WSAAddressToStringA((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0) ?
		dst : NULL;
}

int lizzz_inet_pton(int af, const char *src, void *dst)
{
	struct sockaddr_storage ss;
	int size = sizeof(ss);
	char src_copy[INET6_ADDRSTRLEN + 1];

	ZeroMemory(&ss, sizeof(ss));
	/* stupid non-const API */
	strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
	src_copy[INET6_ADDRSTRLEN] = 0;

	if (WSAStringToAddressA(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
		switch (af) {
		case AF_INET:
			*(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
			return 1;
		case AF_INET6:
			*(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
			return 1;
		}
	}
	return 0;
}

#endif



#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#undef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 64*1024

size_t stacksz = MAX(8192, PTHREAD_STACK_MIN);

HANDLE createThread(LPTHREAD_START_ROUTINE fnRoutine, LPVOID lpParameter)
{

#ifdef WIN32
	DWORD m_dwThreadId;
	return ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fnRoutine, lpParameter, 0, &m_dwThreadId);
#else
	HANDLE pt;

	pthread_attr_t* a = 0, attr;
	if (pthread_attr_init(&attr) == 0) {
		a = &attr;
		pthread_attr_setstacksize(a, stacksz);
	}
	if (pthread_create(&pt, a, fnRoutine, lpParameter) != 0)
	{
		printf("pthread_create failed. OOM?\n");
		exit(0);
	}

	//printf("Create Ptr %d\r\n", pt);

	if (a) pthread_attr_destroy(&attr);

	return pt;

	//int ret = pthread_create(&m_hThread, NULL, fnRoutine, lpParameter);
	//return m_hThread;
#endif
}


void joinThread(HANDLE pt)
{
	if (pt)
	{
#ifdef WIN32
		::WaitForSingleObject(pt, INFINITE);
#else
		pthread_join(pt, 0);
#endif
	}

}

static std::string _itoa(int val)
{
	//printf("val %d\r\n", val);
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%d", val);
	std::string result(buf, strlen(buf));
	return result;
}