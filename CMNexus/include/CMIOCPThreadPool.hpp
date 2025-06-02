#pragma once
#include"../include/CMAbstractThreadPool.h"

class CMIOCPThreadPool : protected CMAbstractThreadPool {
public:
	CMIOCPThreadPool(const short max_threads);
	~CMIOCPThreadPool();
	template<class T>
	void PostTask(T&& task);
	bool& GetIsStop();
	mutex& GetM_Tex();
	vector<thread>& GetThreads();
	queue<function<void()>>& GetTasks();
	condition_variable& GetM_cv();
};

template<class T>
inline void CMIOCPThreadPool::PostTask(T&& task)
{
	{
		unique_lock<mutex> lock(this->GetM_Tex());
		this->GetTasks().emplace(task);
	}
	this->GetM_cv().notify_one();
}
