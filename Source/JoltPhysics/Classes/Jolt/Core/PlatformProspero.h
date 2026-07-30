// "Prospero" (codename) platform support for Jolt Physics.
//@ BASTIEN ADD
#pragma once

// Prospero toolchain is Clang-based (ARM64-style intrinsics not applicable, x86_64 target)
#define JPH_BREAKPOINT __builtin_trap()

// JPH_PLATFORM_PROSPERO_GET_TICKS / _MUTEX* / _RWLOCK* / _SEMAPHORE* are
// intentionally NOT defined here: leaving them undefined makes
// TickCounter.h / Mutex.h / Semaphore.h fall back to their portable
// generic-x86 / std::mutex / std::condition_variable implementations,
// which are correct (if not maximally fast) on Prospero. Define these later
// using Prospero SDK primitives (sceKernel* mutex/rwlock/semaphore APIs and
// the Prospero tick-counter API) as a follow-up perf pass.
//@ BASTIEN END
