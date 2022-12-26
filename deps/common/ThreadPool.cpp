#include "ThreadPool.h"

CThreadPool::CThreadPool()
	: m_nMaxThread(0)
	, m_nMaxTask(MAX_TASK_COUNT)
	, m_bRunning(false)
{
}

CThreadPool::~CThreadPool()
{
	if (m_bRunning)
	{
		Stop();
	}
}

bool CThreadPool::init(int nThreadCount)
{
	if (m_vecpThread.size() != 0) return false;
	if (nThreadCount < 1) return false;

	m_nMaxThread = nThreadCount;
	m_vecpThread.reserve(m_nMaxThread);
	for (auto i = 0; i < m_nMaxThread; ++i)
	{
		m_vecpThread.emplace_back(new std::thread(&CThreadPool::Process, this, i));
	}
	m_bRunning = true;
	return true;
}

void CThreadPool::Process(unsigned nId)
{
	try
	{
		while (m_bRunning)
		{
			Task task(PopTask());
			if (task)
			{
				//printf("this is thread %d\n", nId);
				task();
			}
		}
	}
	catch (...)
	{
		// TODO
		throw;
	}
}

bool CThreadPool::IsTaskFull()
{
	return m_nMaxTask > 0 && m_taskList.size() >= m_nMaxTask;
}

void CThreadPool::AddTask(Task task)
{
	if (m_vecpThread.empty())
	{
		task();
	}
	else
	{
		UniqueLock lock(m_lock);
		while (IsTaskFull())
		{
			m_condNotFull.wait(lock); // 等待队列变得不满
		}

		m_taskList.push_back(std::move(task));
		m_condNotEmpty.notify_one(); // 通知现在有任务了
	}
}

Task CThreadPool::PopTask()
{
	UniqueLock lock(m_lock);
	while (m_taskList.empty() && m_bRunning)
	{
		m_condNotEmpty.wait(lock);
	}

	Task task;
	if (!m_taskList.empty())
	{
		task = m_taskList.front();
		m_taskList.pop_front();
		if (m_nMaxTask > 0)
		{
			m_condNotFull.notify_one();
		}
	}
	return task;
}

void CThreadPool::Stop()
{
	{
		AutoLock lk(m_lock);
		m_bRunning = false;
		m_condNotEmpty.notify_all();
	}

	for (auto& th : m_vecpThread)
	{
		th->join();
	}
}