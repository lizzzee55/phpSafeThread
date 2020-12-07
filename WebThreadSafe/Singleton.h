#pragma once
#include <string>

#define DECLARE_SINGLETON(className)					\
public:													\
	static className* Instance()						\
	{													\
		static className* myInstance = NULL;			\
		if(!myInstance)									\
		{												\
			myInstance = new className();				\
		}												\
		return myInstance;								\
	}													
