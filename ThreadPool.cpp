#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(string(*t_function)(string), ostream * o_taget, int thread_count) :
	task_function(t_function), out_taget(o_taget)
{
	cond_ready = false;
	destroy_threads = false;
	for (int i = 0; i < thread_count; i++)
	{
		t_vector.emplace_back(thread(&ThreadPool::taskWorker, this));
	}
}

ThreadPool::~ThreadPool()
{
	destroy_threads = true;
	active_cond.notify_all();
	for (auto & t : t_vector)
	{
		t.join();
	}
}

ThreadPool& ThreadPool::Instance(string(*t_function)(string), ostream * o_taget, int t_count)
{
	static ThreadPool singleInstance(t_function, o_taget, t_count);
	return singleInstance;
}

void ThreadPool::pushTasks(priority_queue<string> * input_q)
{
	while (!input_q->empty())
	{
		task_deque.push_front(task_wrapper(input_q->top()));
		input_q->pop();
	}
	cond_ready = true;
	active_cond.notify_all();
}

ThreadPool::task_wrapper * ThreadPool::taskGet()
{
	std::lock_guard<mutex> lock(m_get);
	for (auto & task_wrapper : task_deque)
	{
		if (!task_wrapper.in_prog)
		{
			task_wrapper.in_prog = true;
			return &task_wrapper;
		}
	}
	return nullptr;
}

bool ThreadPool::isReady()
{
	return task_deque.empty();
}

void ThreadPool::taskWorker()
{
	while (!destroy_threads)
	{
		unique_lock<mutex> lock(m_active);
		active_cond.wait(lock, [this]() {return cond_ready || destroy_threads; });
		if (destroy_threads) break;

		task_wrapper * task = taskGet();
		if (task)
		{
			string out = task_function(task->input);
			task->output = out;
			taskDone(task);
		}
		else
		{
			cond_ready = false;
		}
	}
}

void ThreadPool::taskDone(task_wrapper * task)
{
	std::lock_guard<mutex> lock(m_done);
	task->done = true;
	while (!task_deque.empty())
	{
		if (task_deque.front().done)
		{
			*out_taget << task_deque.front().output << flush;
			task_deque.pop_front();
		}
		else
		{
			break;
		}
	}
}