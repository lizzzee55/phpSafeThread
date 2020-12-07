#include "ThreadSafeStore.h"
#include "Singleton.h"

class queue
{
public:
	std::string key;
	std::vector< store_data* > list;
};

class ThreadSafeQueue
{
	DECLARE_SINGLETON(ThreadSafeQueue)
public:
	ThreadSafeQueue();
	void push(const char *key, const char *value, int len);
	queue* createQueue(const char *key);
	queue* getQueue(const char *key);

	//void flush_by_index(int index);

	pthread_mutex_t mutex;
	std::vector< queue* > queue_store;
};

#pragma once

inline ThreadSafeQueue::ThreadSafeQueue()
{
	pthread_mutex_init(&mutex, NULL);
}


inline queue* ThreadSafeQueue::createQueue(const char *key)
{
	queue *q = new queue();
	q->key = key;
	pthread_mutex_lock(&mutex);
	queue_store.push_back(q);
	pthread_mutex_unlock(&mutex);
	return q;
}

inline void ThreadSafeQueue::push(const char *key, const char *value, int len)
{

	queue* q = getQueue(key);

	store_data *data = new store_data(key);
	data->setValue(value, len);
	q->list.push_back(data);
}

inline queue* ThreadSafeQueue::getQueue(const char *key)
{
	queue* result = NULL;
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < queue_store.size(); i++)
	{
		queue *q = queue_store[i];
		if (q->key.find(key) == 0)
		{
			result = q;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);

	if (!result)
	{
		result = createQueue(key);
	}

	return result;
}