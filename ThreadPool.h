#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <deque>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

class ThreadPool
{
private:
	struct task_wrapper
	{
		bool in_prog;
		bool done;
		string input;
		string output;

		task_wrapper(string p) :
			input(p), output(), in_prog(false), done(false)
		{}
	};

	bool cond_ready;
	bool destroy_threads;
	mutex m_get;
	mutex m_done;
	mutex m_active;
	condition_variable active_cond;
	ostream * out_taget;
	string(*task_function)(string);
	deque<task_wrapper> task_deque;
	vector<thread> t_vector;

	void taskWorker();
	void taskDone(task_wrapper * task);
	task_wrapper * taskGet();

public:
	ThreadPool(string(*t_function)(string), ostream * o_taget = &cout, int t_count = 10);
	~ThreadPool();
	static ThreadPool& Instance(string(*t_function)(string) = nullptr, ostream * o_taget = &cout, int t_count = 10);
	void pushTasks(priority_queue<string> * input_q);
	bool isReady();
};