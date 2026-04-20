// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

// This file intentionally includes only UE headers — no Jolt headers.
// Keeping Tasks/Task.h isolated here avoids include conflicts with Jolt macros.

#include <Jolt/Core/JoltUETaskDispatch.h>

#include "Tasks/Task.h"

void JoltUETaskDispatch(TFunction<void()> InTask)
{
	UE::Tasks::Launch(TEXT("JoltJob"), MoveTemp(InTask));
}
