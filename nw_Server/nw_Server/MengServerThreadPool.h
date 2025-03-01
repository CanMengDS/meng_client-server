#include<iostream>
#include<queue>
#include<vector>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<functional>
using namespace std;

class MengServerThreadPool {
public:
	MengServerThreadPool(int threadsNum);
	~MengServerThreadPool();
	template<class T>
	void enterTask(T&& task);

private:
	vector<thread> threads;
	queue<function<void()>> tasks;
	condition_variable thread_cv;
	mutex tex;
	bool stop = false;
};

template<class T>
inline void MengServerThreadPool::enterTask(T&& ta)
{
	//function<void()> func(ta);
	{
		unique_lock<mutex> lock(tex);
		tasks.emplace(function<void()>(ta));
	}
	thread_cv.notify_one();
}
