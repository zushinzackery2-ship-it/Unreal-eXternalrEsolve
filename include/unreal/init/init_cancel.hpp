#pragma once
// Unreal-eXternalrEsolve - AutoInit 取消回调
// 本文件只负责取消信号；上下文回滚由生命周期控制器统一处理。

#include <Windows.h>
#include <atomic>

namespace unreal
{

using AutoInitCancelCallback = bool (*)();

namespace detail
{

inline std::atomic<AutoInitCancelCallback>& AutoInitCancelCallbackStorage()
{
    static std::atomic<AutoInitCancelCallback> callback{ nullptr };
    return callback;
}

inline bool IsAutoInitCancellationRequested()
{
    AutoInitCancelCallback callback =
        AutoInitCancelCallbackStorage().load(std::memory_order_acquire);
    return callback != nullptr && callback();
}

inline bool WaitForAutoInitRetry(DWORD delayMs)
{
    constexpr DWORD kSleepSliceMs = 25;
    DWORD elapsedMs = 0;

    while (elapsedMs < delayMs)
    {
        if (IsAutoInitCancellationRequested())
        {
            return false;
        }

        DWORD remainingMs = delayMs - elapsedMs;
        DWORD sleepMs = remainingMs < kSleepSliceMs
            ? remainingMs
            : kSleepSliceMs;
        Sleep(sleepMs);
        elapsedMs += sleepMs;
    }

    return !IsAutoInitCancellationRequested();
}

} // namespace detail

inline void SetAutoInitCancelCallback(AutoInitCancelCallback callback)
{
    detail::AutoInitCancelCallbackStorage().store(
        callback,
        std::memory_order_release);
}

inline AutoInitCancelCallback GetAutoInitCancelCallback()
{
    return detail::AutoInitCancelCallbackStorage().load(
        std::memory_order_acquire);
}

} // namespace unreal
