// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemUETask.h>
#include <Jolt/Core/JoltUETaskDispatch.h>
#include <Jolt/Core/Profiler.h>

JPH_NAMESPACE_BEGIN

JobSystemUETask::JobSystemUETask(uint inMaxJobs, uint inMaxBarriers)
{
	Init(inMaxJobs, inMaxBarriers);
}

void JobSystemUETask::Init(uint inMaxJobs, uint inMaxBarriers)
{
	// Initialize barrier list via base class
	JobSystemWithBarrier::Init(inMaxBarriers);

	// Initialize job free list
	mJobs.Init(inMaxJobs, inMaxJobs);
}

int JobSystemUETask::GetMaxConcurrency() const
{
	// Return number of CPU cores + 1 (the calling thread can also execute jobs in WaitForJobs)
	unsigned num_threads = std::thread::hardware_concurrency();
	return int(num_threads > 0 ? num_threads + 1 : 5);  // Default to 5 if hardware_concurrency unavailable
}

JobHandle JobSystemUETask::CreateJob(const char *inName, ColorArg inColor, const JobFunction &inJobFunction, uint32 inNumDependencies)
{
	JPH_PROFILE_FUNCTION();

	// Loop until we can get a job from the free list
	uint32 index;
	for (;;)
	{
		index = mJobs.ConstructObject(inName, inColor, this, inJobFunction, inNumDependencies);
		if (index != AvailableJobs::cInvalidObjectIndex)
			break;
		JPH_ASSERT(false, "No jobs available!");
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	Job *job = &mJobs.Get(index);

	// Construct handle to keep a reference, the job is queued below and may immediately complete
	JobHandle handle(job);

	// If there are no dependencies, queue the job now
	if (inNumDependencies == 0)
		QueueJob(job);

	// Return the handle
	return handle;
}

void JobSystemUETask::FreeJob(Job *inJob)
{
	mJobs.DestructObject(inJob);
}

void JobSystemUETask::QueueJob(Job *inJob)
{
	JPH_PROFILE_FUNCTION();

	// Add reference to job because we're dispatching it
	inJob->AddRef();

	// Increment pending task counter
	++mPendingTaskCount;

	JoltUETaskDispatch([this, inJob]()
	{
		inJob->Execute();
		inJob->Release();
		NotifyTaskComplete();
	});
}

void JobSystemUETask::QueueJobs(Job **inJobs, uint inNumJobs)
{
	JPH_PROFILE_FUNCTION();

	JPH_ASSERT(inNumJobs > 0);

	// Queue all jobs
	for (Job **job = inJobs, **job_end = inJobs + inNumJobs; job < job_end; ++job)
		QueueJob(*job);
}

void JobSystemUETask::NotifyTaskComplete()
{
	if (--mPendingTaskCount == 0)
	{
		std::lock_guard<std::mutex> lock(mTaskMutex);
		mTaskComplete.notify_all();
	}
}

void JobSystemUETask::WaitAllTasks()
{
	std::unique_lock<std::mutex> lock(mTaskMutex);
	mTaskComplete.wait(lock, [this] { return mPendingTaskCount.load() == 0; });
}

JPH_NAMESPACE_END
