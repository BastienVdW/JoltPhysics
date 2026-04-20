// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"

// Dispatches a task onto UE's worker thread pool.
// Exists as a separate translation unit so Tasks/Task.h never mixes with Jolt headers.
JOLTPHYSICS_API void JoltUETaskDispatch(TFunction<void()> InTask);
