#pragma once

#include "../../core/process.hpp"
#include "../../memory/memory.hpp"
#include "../../memory/memory_shmem.hpp"

namespace xrd
{
namespace detail
{

inline bool TryResolvePhysXModuleBase(
    const IMemoryAccessor& mem,
    u32 pid,
    const wchar_t* moduleName,
    uptr& outBase)
{
    outBase = 0;

    const auto* shmemAccessor =
        dynamic_cast<const SharedMemoryAccessor*>(&mem);
    if (shmemAccessor != nullptr)
    {
        outBase = static_cast<uptr>(
            shmemAccessor->GetModuleBase(moduleName));
        return outBase != 0;
    }

    outBase = FindModuleBase(pid, moduleName);
    return outBase != 0;
}

} // namespace detail

inline bool TryResolveModuleBaseWithAccessor(
    const IMemoryAccessor& mem,
    u32 pid,
    const wchar_t* moduleName,
    uptr& outBase)
{
    return detail::TryResolvePhysXModuleBase(
        mem,
        pid,
        moduleName,
        outBase);
}

} // namespace xrd
