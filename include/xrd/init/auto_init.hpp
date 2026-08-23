#pragma once
// Xrd-eXternalrEsolve - AutoInit 公共入口

#include "init_cancel.hpp"
#include "init_helpers.hpp"
#include "lifecycle/controller.hpp"

namespace xrd
{

inline bool AutoInit(const wchar_t* processName = nullptr)
{
    detail::AutoInitConfiguration configuration;
    configuration.mode = detail::AutoInitMode::WinApi;
    configuration.processName = processName;
    configuration.modeTag = "AutoInit";
    return detail::RunAutoInit(configuration);
}

inline bool AutoInitSharedMem(const wchar_t* processName = nullptr)
{
    detail::AutoInitConfiguration configuration;
    configuration.mode = detail::AutoInitMode::SharedMemory;
    configuration.processName = processName;
    configuration.modeTag = "AutoInit (SharedMem)";
    return detail::RunAutoInit(configuration);
}

inline bool AutoInitDriver(const wchar_t* processName = nullptr)
{
    return AutoInitSharedMem(processName);
}

} // namespace xrd
