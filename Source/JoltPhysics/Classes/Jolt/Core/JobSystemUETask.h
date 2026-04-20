// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include <Jolt/Core/JobSystemWithBarrier.h>
#include <Jolt/Core/FixedSizeFreeList.h>

JPH_NAMESPACE_BEGIN

/// Implementation of a JobSystem using Unreal Engine's task graph
///
/// This implementation dispatches each Jolt job as a UE::Tasks::Launch task.
/// No dedicated worker threads are created — jobs run on the engine's worker thread pool
/// and are properly cleaned up when idle.
class JOLTPHYSICS_API JobSystemUETask final : public JobSystemWithBarrier
{
public:
	JPH_OVERRIDE_NEW_DELETE

	/// Creates a task-based job system.
	/// @see JobSystemUETask::Init
	explicit						JobSystemUETask(uint inMaxJobs, uint inMaxBarriers);
									JobSystemUETask() = default;
	virtual							~JobSystemUETask() override = default;

	/// Initialize the task-based job system
	/// @param inMaxJobs Max number of jobs that can be allocated at any time
	/// @param inMaxBarriers Max number of barriers that can be allocated at any time
	void							Init(uint inMaxJobs, uint inMaxBarriers);

	// See JobSystem
	virtual int						GetMaxConcurrency() const override;
	virtual JobHandle				CreateJob(const char *inName, ColorArg inColor, const JobFunction &inJobFunction, uint32 inNumDependencies = 0) override;

	/// Wait for all pending task-dispatched jobs to complete
	void							WaitAllTasks();

protected:
	// See JobSystem
	virtual void					QueueJob(Job *inJob) override;
	virtual void					QueueJobs(Job **inJobs, uint inNumJobs) override;
	virtual void					FreeJob(Job *inJob) override;

private:
	using AvailableJobs = FixedSizeFreeList<Job>;
	AvailableJobs					mJobs;

	mutable atomic<int32>			mPendingTaskCount { 0 };
};

JPH_NAMESPACE_END
