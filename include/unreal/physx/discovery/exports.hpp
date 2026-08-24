#pragma once

#include "../../memory/memory.hpp"
#include <cstring>
#include <format>
#include <iostream>
#include <vector>

namespace unreal
{

inline bool FindExportRVA(
    const IMemoryAccessor& mem,
    uptr dllBase,
    const char* funcName,
    u32& outRVA)
{
    outRVA = 0;

    IMAGE_DOS_HEADER dos{};
    if (!mem.Read(dllBase, &dos, sizeof(dos)) ||
        dos.e_magic != IMAGE_DOS_SIGNATURE)
    {
        return false;
    }

    IMAGE_NT_HEADERS64 nt{};
    if (!mem.Read(dllBase + dos.e_lfanew, &nt, sizeof(nt)) ||
        nt.Signature != IMAGE_NT_SIGNATURE)
    {
        return false;
    }

    const auto& exportDirectory =
        nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (exportDirectory.VirtualAddress == 0 || exportDirectory.Size == 0)
    {
        std::cerr << "[unreal][PE] no export dir\n";
        return false;
    }

    IMAGE_EXPORT_DIRECTORY exports{};
    if (!mem.Read(
            dllBase + exportDirectory.VirtualAddress,
            &exports,
            sizeof(exports)))
    {
        std::cerr << "[unreal][PE] read export dir failed\n";
        return false;
    }

    const u32 nameCount = exports.NumberOfNames;
    std::cerr << std::format(
        "[unreal][PE] nameCount={} AddrOfNames=0x{:X}\n",
        nameCount,
        exports.AddressOfNames);
    if (nameCount == 0)
    {
        return false;
    }

    std::vector<u32> nameRVAs(nameCount);
    if (!mem.Read(
            dllBase + exports.AddressOfNames,
            nameRVAs.data(),
            nameCount * sizeof(u32)))
    {
        std::cerr << "[unreal][PE] read name RVAs failed\n";
        return false;
    }

    char nameBuffer[128]{};
    u32 readFailureCount = 0;
    u32 readSuccessCount = 0;
    for (u32 index = 0; index < nameCount; ++index)
    {
        if (!mem.Read(
                dllBase + nameRVAs[index],
                nameBuffer,
                sizeof(nameBuffer) - 1))
        {
            ++readFailureCount;
            continue;
        }

        ++readSuccessCount;
        nameBuffer[sizeof(nameBuffer) - 1] = '\0';
        if (std::strcmp(nameBuffer, funcName) != 0)
        {
            continue;
        }

        u16 ordinal = 0;
        if (!ReadValue(
                mem,
                dllBase + exports.AddressOfNameOrdinals +
                    index * sizeof(u16),
                ordinal))
        {
            return false;
        }

        return ReadValue(
            mem,
            dllBase + exports.AddressOfFunctions +
                ordinal * sizeof(u32),
            outRVA);
    }

    std::cerr << std::format(
        "[unreal][PE] not found '{}': readOk={} readFail={}\n",
        funcName,
        readSuccessCount,
        readFailureCount);
    return false;
}

inline bool ResolveRipRelativeTarget(
    const IMemoryAccessor& mem,
    uptr dllBase,
    u32 funcRVA,
    u32& outGlobalRVA)
{
    outGlobalRVA = 0;

    constexpr u32 kScanSize = 64;
    u8 code[kScanSize]{};
    if (!mem.Read(dllBase + funcRVA, code, kScanSize))
    {
        return false;
    }

    for (u32 index = 0; index + 7 <= kScanSize; ++index)
    {
        if (code[index] != 0x48 || code[index + 1] != 0x8B)
        {
            continue;
        }

        const u8 modrm = code[index + 2];
        if ((modrm & 0xC7) != 0x05)
        {
            continue;
        }

        i32 displacement = 0;
        std::memcpy(
            &displacement,
            &code[index + 3],
            sizeof(displacement));
        outGlobalRVA =
            funcRVA +
            index +
            7 +
            static_cast<u32>(displacement);
        return true;
    }

    return false;
}

} // namespace unreal
