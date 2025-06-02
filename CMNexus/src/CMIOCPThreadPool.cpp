#include "../include/CMIOCPThreadPool.hpp"

CMIOCPThreadPool::CMIOCPThreadPool(const short max_threads)
{
	is_stop = false;
	for (int i = 0; i < max_threads; i++) {
		this->threads.emplace_back([this] {
			while (1) {
				function<void()> temp_func; 
				{
					unique_lock<mutex> lock(this->GetM_Tex());
					this->GetM_cv().wait(lock, [this] {
						return !(this->GetTasks().empty()) || this->GetIsStop();
						});
					if (this->GetIsStop() && this->GetTasks().empty()) return;

					temp_func = this->GetTasks().front();
					this->GetTasks().pop();
				}	
				temp_func();
			}
			});
	}
}

CMIOCPThreadPool::~CMIOCPThreadPool()
{
	{
		unique_lock<mutex> lock(m_tex);
		is_stop = true;
	}
	m_cv.notify_all();
	for (auto& i : this->GetThreads()) {
		i.join();
	}
}

bool& CMIOCPThreadPool::GetIsStop()
{
	return this->is_stop;
}

mutex& CMIOCPThreadPool::GetM_Tex()
{
	return this->m_tex;
}

vector<thread>& CMIOCPThreadPool::GetThreads()
{
	return this->threads;
}

queue<function<void()>>& CMIOCPThreadPool::GetTasks()
{
	return this->tasks;
}

condition_variable& CMIOCPThreadPool::GetM_cv()
{
	return this->m_cv;
}
