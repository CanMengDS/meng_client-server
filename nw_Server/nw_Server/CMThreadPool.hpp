#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<queue>
#include<functional>
#include<condition_variable>
using namespace std;

class CMThreadPool {
public:
	//CMThreadPool();
	CMThreadPool(unsigned short threads);
	template<class T>
	void PostTask(T&& task);
	~CMThreadPool();
private:
	bool is_stop;
	mutex cm_tex;
	condition_variable thread_cv;
	queue<function<void()>> task;
	vector<thread> thread_list;
};

template<class T>
inline void CMThreadPool::PostTask(T&& task)
{
	{
		unique_lock<mutex> lock(cm_tex);
		this->task.emplace(task);
	}
	thread_cv.notify_all();
}
