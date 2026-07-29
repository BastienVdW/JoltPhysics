// "Ounce" (codename) platform support for Jolt Physics.
//@ BASTIEN ADD
#pragma once

// Minimum required: breakpoint intrinsic (ARM64/Clang toolchain)
#define JPH_BREAKPOINT __builtin_trap()

// JPH_PLATFORM_OUNCE_GET_TICKS / _MUTEX* / _RWLOCK* / _SEMAPHORE* are
// intentionally NOT defined here: leaving them undefined makes
// TickCounter.h / Mutex.h / Semaphore.h fall back to their portable
// generic-ARM / std::mutex / std::condition_variable implementations,
// which are correct (if not maximally fast) on Ounce. Define these
// later using NN SDK primitives (nn::os::Mutex / ReaderWriterLock /
// Semaphore, and the NN SDK tick-counter API) as a follow-up perf pass.
//@ BASTIEN END
