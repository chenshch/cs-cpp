#pragma once
//#include "def.h"
#include <mutex>

template <class T>
class CSingleton
{
public:
	static inline T* getInstance()
	{
		std::unique_lock <std::mutex> lock;
		if (nullptr == s_instance)
		{
			s_instance = new (std::nothrow) T();
		}
		return s_instance;
	};

protected:
	CSingleton() {}
	~CSingleton() {}
	CSingleton(const CSingleton&) {}
	CSingleton& operator= (const CSingleton&) {}
	static T* s_instance;
};
template <class T>
T* CSingleton<T>::s_instance = nullptr;