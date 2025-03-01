#include "MengServerThreadPool.h"

MengServerThreadPool::MengServerThreadPool(int threadsNum) :stop(false) {
	for (int i = 0; i <= threadsNum; i++) {
		threads.emplace_back([this] {
			while (1) {
				unique_lock<mutex> lock(tex);
				thread_cv.wait(lock, [this] {
					return !tasks.empty();
					});
				if (stop && tasks.empty()) {
					return;
				}

				function<void()> task = tasks.front();
				task();
				tasks.pop();
			}
			});
	}
}

MengServerThreadPool::~MengServerThreadPool()
{
	{
		unique_lock<mutex> lock(tex);
		stop = true;
	}
	thread_cv.notify_all();
	for (auto& e : threads) {
		e.join();
	}
}
