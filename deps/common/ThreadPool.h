#pragma once
/************************************************************************/
/*
 * 通用线程池
 * 线程池初始化N个线程，通过AddTask的方式加入任务，线程池会分配线程处理任务
 * 任务函数类型是无参void返回的函数
*/
/************************************************************************/
#include <thread>
#include <vector>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>

using Mutex = std::mutex;
using AutoLock = std::lock_guard<Mutex>;
using UniqueLock = std::unique_lock<Mutex>;
using Task = std::function<void()>;
const int MAX_TASK_COUNT = 8000;

class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();

	CThreadPool(const CThreadPool& other) = delete;
	CThreadPool& operator=(const CThreadPool& other) = delete;

public:
	bool init(int nThreadCount);

	void Stop();

	void AddTask(Task task);

private:
	void Process(unsigned nId);

	bool IsTaskFull();

	Task PopTask();

public:
	int m_nMaxThread;
	int m_nMaxTask;
	std::vector<std::unique_ptr<std::thread>> m_vecpThread;

	Mutex m_lock;

	bool m_bRunning;

	std::deque<Task> m_taskList;

	std::condition_variable m_condNotEmpty;
	std::condition_variable m_condNotFull;
};

