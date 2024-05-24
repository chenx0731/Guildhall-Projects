#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/Job.hpp"
#include <thread>

JobSystem::JobSystem(JobSystemConfig config) : m_config(config)
{
	int hardware_concurrency = std::thread::hardware_concurrency();
	m_numOfThreads = m_config.m_desiredNumOfThreads;
	if (m_config.m_desiredNumOfThreads > hardware_concurrency)
		m_numOfThreads = hardware_concurrency;
}

JobSystem::~JobSystem()
{
}

void JobSystem::Startup()
{
	for (int threadIndex = 0; threadIndex < m_numOfThreads; threadIndex++) {
		
		JobWorker* worker = new JobWorker(this, threadIndex);
		m_threadWorkers.push_back(worker);
	}
}

void JobSystem::Shutdown()
{
	m_isQuitting = true;
	for (int threadIndex = 0; threadIndex < m_numOfThreads; threadIndex++) {
		JobWorker*& worker = m_threadWorkers[threadIndex];
		if (worker) {
			delete worker;
		}
	}
	m_threadWorkers.clear();
	m_waitingQueueLock.lock();
	m_waitingJobs.clear();
	m_waitingQueueLock.unlock();

	m_workingQueueLock.lock();
	m_workingJobs.clear();
	m_workingQueueLock.unlock();

	m_completedQueueLock.lock();
	m_completedJobs.clear();
	m_completedQueueLock.unlock();
	return;
}

void JobSystem::BeginFrame()
{
}

void JobSystem::EndFrame()
{
}


void JobSystem::AddJobToWaitingQueue(Job* job)
{
	m_waitingQueueLock.lock();
	m_waitingJobs.push_back(job);
	m_waitingQueueLock.unlock();
}

Job* JobSystem::ClaimJobFromWaitingQueue()
{
	Job* job = nullptr;
	
	// claim from waitlist
	m_waitingQueueLock.lock();
	if (!m_waitingJobs.empty()) {
		job = m_waitingJobs.front();
		m_waitingJobs.pop_front();
	}
	m_waitingQueueLock.unlock();

	// add to working list
	m_workingQueueLock.lock();
	if (job != nullptr)
		m_workingJobs.push_back(job);
	m_workingQueueLock.unlock();

	return job;
}

Job* JobSystem::RetrieveJobFromCompletedQueue()
{
	Job* job = nullptr;
	
	m_completedQueueLock.lock();
	if (!m_completedJobs.empty()) {
		job = m_completedJobs.front();
		m_completedJobs.pop_front();
	}
	m_completedQueueLock.unlock();
	
	return job;
}

void JobSystem::MoveJobFromWorkingToComplete(Job* job)
{

	// claim from working list
	m_workingQueueLock.lock();
	m_workingJobs.erase(std::remove(m_workingJobs.begin(), m_workingJobs.end(), job), m_workingJobs.end());
	m_workingQueueLock.unlock();

	// add to complete list
	m_completedQueueLock.lock();
	m_completedJobs.push_back(job);
	m_completedQueueLock.unlock();

}


JobWorker::JobWorker(JobSystem* jobSystem, int threadID)
{
	m_jobSystem = jobSystem;
	m_threadID = threadID;

	if (m_jobSystem)
		m_thread = new std::thread(&JobWorker::ThreadMain, this);
}

JobWorker::~JobWorker()
{
	if (m_thread) {
		m_thread->join();
	}
	delete m_thread;
	m_thread = nullptr;
	m_jobSystem = nullptr;
}

void JobWorker::ThreadMain()
{
	while (!m_jobSystem->m_isQuitting) {
		Job* jobToDo = nullptr;
		jobToDo = m_jobSystem->ClaimJobFromWaitingQueue();
		if (jobToDo) {
			// excute the job and move it to complete list when finished
			jobToDo->Execute(); // 100ms or more!
			m_jobSystem->MoveJobFromWorkingToComplete(jobToDo);
		}
		else 
			std::this_thread::sleep_for(std::chrono::microseconds(5));
	}
}
