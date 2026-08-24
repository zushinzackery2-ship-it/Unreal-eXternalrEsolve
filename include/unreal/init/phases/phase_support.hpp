#pragma once
// Unreal-eXternalrEsolve - 初始化阶段公共工具

#include "../init_cancel.hpp"
#include "../lifecycle/types.hpp"
#include <Windows.h>
#include <iostream>

namespace unreal
{
namespace detail
{

inline InitStepResult AutoInitCancellationPoint()
{
    return IsAutoInitCancellationRequested()
        ? InitStepResult::Cancelled
        : InitStepResult::Succeeded;
}

inline void LogSlowInitPhase(
    const char* phaseName,
    ULONGLONG startTick,
    ULONGLONG thresholdMs = 100)
{
    ULONGLONG elapsedMs = GetTickCount64() - startTick;
    if (elapsedMs >= thresholdMs)
    {
        std::cerr << "[unreal][Perf] " << phaseName
                  << " 耗时 " << elapsedMs << " ms\n";
    }
}

} // namespace detail
} // namespace unreal
