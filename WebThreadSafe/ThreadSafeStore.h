#include "Singleton.h"
#include "header.h"
#include <vector>
#include <string>

class store_data
{
public:
	char *value;
	char *key;
	size_t len;

	store_data(const char *key)
	{
		size_t len = strlen(key);
		this->key = (char*)malloc(sizeof(char) * (len + 1));
		memcpy(this->key, key, len);
		this->key[len] = 0;

		this->len = 0;
	}

	void setValue(const char *val, size_t len)
	{
		if (this->len > 0)
			free(this->value);

		this->value = (char*)malloc(sizeof(char) * (len + 1));
		this->len = len;
		memcpy(this->value, val, len);
		this->value[len] = 0;
	}

	void flush()
	{
		if (this->len > 0)
			free(this->value);

		free(this->key);
	}
};


class ThreadSafeStore
{
	DECLARE_SINGLETON(ThreadSafeStore)
public:
	ThreadSafeStore();
	void set(const char *key, const char *value, int len);
	store_data* get(const char *key);

	pthread_mutex_t mutex;
	std::vector< store_data* > store;
	
	
};

#pragma once

inline ThreadSafeStore::ThreadSafeStore()
{
	pthread_mutex_init(&mutex, NULL);
}


inline store_data* ThreadSafeStore::get(const char *key)
{
	store_data *result = NULL;

	printf("store size %d\r\n", store.size());

	pthread_mutex_lock(&mutex);
	for (int i = 0; i < store.size(); i++)
	{
		if (strcmp(store[i]->key, key) == 0)
		{
			result = store[i];
		}
	}
	pthread_mutex_unlock(&mutex);

	return result;
}

inline void ThreadSafeStore::set(const char *key, const char *value, int len)
{
	store_data *data = get(key);
	if (data)
	{
		data->setValue(value, len);
		return;
	}

	pthread_mutex_lock(&mutex);
	
	printf("val %s key %s\r\n", value, key);
	data = new store_data(key);
	data->setValue(value, len);
	store.push_back(data);

	pthread_mutex_unlock(&mutex);
}


