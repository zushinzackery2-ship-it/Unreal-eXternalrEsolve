#pragma once
// Xrd-eXternalrEsolve - AutoInit 内部生命周期类型

namespace xrd
{
namespace detail
{

enum class AutoInitMode
{
    WinApi,
    SharedMemory
};

enum class InitStepResult
{
    Succeeded,
    RetryRequired,
    Cancelled
};

struct AutoInitConfiguration
{
    AutoInitMode mode = AutoInitMode::WinApi;
    const wchar_t* processName = nullptr;
    const char* modeTag = "AutoInit";
};

} // namespace detail
} // namespace xrd
