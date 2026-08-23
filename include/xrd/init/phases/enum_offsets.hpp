#pragma once
// Xrd-eXternalrEsolve - UEnum::Names 与 UField::Next 偏移阶段

#include "phase_support.hpp"
#include "../../core/context.hpp"
#include "../../resolve/property/scan_property_offsets.hpp"
#include "../../resolve/uobject/scan_offsets.hpp"
#include <iostream>
#include <string>

namespace xrd
{
namespace detail
{

inline bool TryDiscoverEnumNamesOffset(Context& ctx, uptr object)
{
    uptr objectClass = 0;
    if (!ReadPtr(*ctx.mem, object + ctx.off.UObject_Class, objectClass) ||
        !IsCanonicalUserPtr(objectClass))
    {
        return false;
    }

    FName classNameValue{};
    if (!ReadValue(
        *ctx.mem,
        objectClass + ctx.off.UObject_Name,
        classNameValue))
    {
        return false;
    }

    std::string className = resolve::ResolveNameDirect(
        *ctx.mem,
        ctx.off,
        classNameValue.ComparisonIndex,
        classNameValue.Number);
    if (className != "Enum" && className != "UserDefinedEnum")
    {
        return false;
    }

    for (i32 testOffset = 0x30; testOffset <= 0xA0; testOffset += 8)
    {
        uptr data = 0;
        i32 count = 0;
        i32 maximum = 0;
        ReadPtr(*ctx.mem, object + testOffset, data);
        ReadI32(*ctx.mem, object + testOffset + 8, count);
        ReadI32(*ctx.mem, object + testOffset + 12, maximum);

        if (!IsCanonicalUserPtr(data) ||
            count <= 0 ||
            count > 256 ||
            maximum < count ||
            maximum > 1024)
        {
            continue;
        }

        FName firstName{};
        if (!ReadValue(*ctx.mem, data, firstName))
        {
            continue;
        }

        std::string resolvedName = resolve::ResolveNameDirect(
            *ctx.mem,
            ctx.off,
            firstName.ComparisonIndex,
            firstName.Number);
        if (!resolvedName.empty() && resolvedName.size() < 256)
        {
            ctx.off.UEnum_Names = testOffset;
            std::cerr << "[xrd] UEnum::Names +0x"
                      << std::hex << testOffset << std::dec << "\n";
            return true;
        }
    }

    return false;
}

inline InitStepResult RunEnumAndFieldDiscoveryPhase()
{
    Context& ctx = Ctx();
    ULONGLONG phaseTick = GetTickCount64();

    if (ctx.off.UEnum_Names == -1)
    {
        i32 objectCount = resolve::GetObjectCount(*ctx.mem, ctx.off);
        for (i32 index = 0; index < objectCount; ++index)
        {
            if ((index % 256) == 0 &&
                AutoInitCancellationPoint() == InitStepResult::Cancelled)
            {
                return InitStepResult::Cancelled;
            }

            uptr object = resolve::ReadObjectAt(*ctx.mem, ctx.off, index);
            if (!IsCanonicalUserPtr(object))
            {
                continue;
            }

            if (TryDiscoverEnumNamesOffset(ctx, object))
            {
                break;
            }
        }
    }

    LogSlowInitPhase("UEnum::Names 偏移搜索", phaseTick);

    if (ctx.off.UObject_Outer != -1)
    {
        ctx.off.UField_Next = ctx.off.UObject_Outer + 8;
    }

    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace xrd
