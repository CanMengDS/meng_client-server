#pragma once
#include<mutex>
#include<queue>
#include<thread>
#include<vector>
#include<functional>
#include<condition_variable>

#include <iostream>
using namespace std;

class CMAbstractThreadPool {
public:
	CMAbstractThreadPool() = default;
	~CMAbstractThreadPool() = default;
protected:
	bool is_stop;
	mutex m_tex;
	vector<thread> threads;
	queue<function<void()>> tasks;
	condition_variable m_cv;
};