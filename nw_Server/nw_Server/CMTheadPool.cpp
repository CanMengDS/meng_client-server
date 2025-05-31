#include "CMThreadPool.hpp"

CMThreadPool::CMThreadPool(unsigned short threads) :is_stop(false)
{
	for (int i = 0; i < threads; i++) {
		thread_list.emplace_back([this] {
			while (1) {
				function<void()> temp_task;
				{
					unique_lock<mutex> lock(cm_tex);
					thread_cv.wait(lock, [this] {
						return !task.empty();
						});

					if (is_stop && task.empty()) return;
					temp_task = task.front();
					task.pop();
				}
				temp_task();
			}
			});
	}
}

CMThreadPool::~CMThreadPool()
{
	{
		unique_lock<mutex> lock(cm_tex);
		is_stop = true;
	}
	thread_cv.notify_all();
	for (auto& i : thread_list) {
		i.join();
	}
}
