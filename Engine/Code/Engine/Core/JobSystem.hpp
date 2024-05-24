#pragma once
#include <mutex>
#include <deque>
#include <vector>

class Job;
class JobWorker;

struct JobSystemConfig
{
	int m_desiredNumOfThreads = 0;
};

class JobSystem
{
public:
	JobSystem(JobSystemConfig config);
	~JobSystem();
	void	Startup();
	void	Shutdown();
	void	BeginFrame();
	void	EndFrame();


	void	AddJobToWaitingQueue(Job* job);
	Job*	ClaimJobFromWaitingQueue();
	Job*	RetrieveJobFromCompletedQueue();
	void	MoveJobFromWorkingToComplete(Job* job);


public:
	JobSystemConfig			m_config;
	int						m_numOfThreads;

	bool					m_isQuitting;
	
	std::vector<JobWorker*> m_threadWorkers;

	std::mutex				m_waitingQueueLock;
	std::deque<Job*>		m_waitingJobs;

	std::mutex				m_workingQueueLock;
	std::deque<Job*>		m_workingJobs;

	std::mutex				m_completedQueueLock;
	std::deque<Job*>		m_completedJobs;

	
};


class JobWorker
{
public:
	JobWorker(JobSystem* jobSystem, int threadID);
	~JobWorker();
	void ThreadMain();

public:
	JobSystem*		m_jobSystem = nullptr;
	int				m_threadID = -1;
	std::thread*	m_thread = nullptr;
};
