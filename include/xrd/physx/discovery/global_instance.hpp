#pragma once

#include "exports.hpp"
#include "module.hpp"
#include <format>
#include <iostream>

namespace xrd
{

inline bool ResolvePhysXGlobalInstance(
    const IMemoryAccessor& mem,
    u32 pid,
    uptr& outDllBase,
    uptr& outGlobalPtr)
{
    outDllBase = 0;
    outGlobalPtr = 0;

    constexpr const wchar_t* kDllNames[] =
    {
        L"PhysX3_x64.dll",
        L"PhysX3.dll",
        L"PhysX_64.dll",
    };

    for (const wchar_t* name : kDllNames)
    {
        if (!TryResolveModuleBaseWithAccessor(mem, pid, name, outDllBase))
        {
            continue;
        }

        std::cerr << "[xrd][PhysX] DLL found: ";
        for (const wchar_t* character = name; *character; ++character)
        {
            std::cerr << static_cast<char>(*character);
        }
        std::cerr << std::format(" base=0x{:X}\n", outDllBase);
        break;
    }

    if (outDllBase == 0)
    {
        std::cerr
            << "[xrd][PhysX] FAIL: PhysX DLL not found in process modules\n";
        return false;
    }

    u32 globalRVA = 0;
    u32 functionRVA = 0;
    if (FindExportRVA(mem, outDllBase, "PxGetPhysics", functionRVA))
    {
        std::cerr << std::format(
            "[xrd][PhysX] PxGetPhysics RVA=0x{:X}\n",
            functionRVA);
        if (!ResolveRipRelativeTarget(
                mem,
                outDllBase,
                functionRVA,
                globalRVA))
        {
            std::cerr
                << "[xrd][PhysX] WARN: mov reg,[rip+disp32] not found\n";
        }
    }
    else
    {
        std::cerr
            << "[xrd][PhysX] WARN: export not found, using hardcoded RVA\n";
    }

    if (globalRVA == 0)
    {
        globalRVA = 0x1E8878;
        std::cerr << std::format(
            "[xrd][PhysX] fallback globalRVA=0x{:X}\n",
            globalRVA);
    }

    std::cerr << std::format(
        "[xrd][PhysX] global RVA=0x{:X} VA=0x{:X}\n",
        globalRVA,
        outDllBase + globalRVA);

    uptr instancePtr = 0;
    if (!ReadPtr(mem, outDllBase + globalRVA, instancePtr))
    {
        std::cerr
            << "[xrd][PhysX] FAIL: cannot read global pointer\n";
        return false;
    }
    if (!IsCanonicalUserPtr(instancePtr))
    {
        std::cerr << std::format(
            "[xrd][PhysX] FAIL: invalid ptr 0x{:X}\n",
            instancePtr);
        return false;
    }

    outGlobalPtr = instancePtr;
    return true;
}

} // namespace xrd
