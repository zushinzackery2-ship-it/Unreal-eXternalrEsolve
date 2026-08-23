#pragma once
// Xrd-eXternalrEsolve - FVector 精度发现阶段

#include "phase_support.hpp"
#include "../../engine/objects/objects_search.hpp"
#include "../../engine/world/world_levels.hpp"
#include <algorithm>
#include <iostream>

namespace xrd
{
namespace detail
{

inline uptr FindRelativeLocationProperty(uptr componentClass)
{
    uptr currentClass = componentClass;
    while (currentClass != 0)
    {
        uptr property = FindPropertyInStruct(
            currentClass,
            "RelativeLocation");
        if (property != 0)
        {
            return property;
        }

        currentClass = GetSuperStruct(currentClass);
    }

    return 0;
}

inline InitStepResult RunVectorPrecisionDiscoveryPhase()
{
    Context& ctx = Ctx();
    ULONGLONG phaseTick = GetTickCount64();
    bool usesDoublePrecision = false;
    bool detected = false;

    std::vector<uptr> actors = GetAllActors();
    i32 actorCount = std::min(
        static_cast<i32>(actors.size()),
        static_cast<i32>(20));

    for (i32 index = 0; index < actorCount; ++index)
    {
        if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
        {
            return InitStepResult::Cancelled;
        }

        uptr actor = actors[static_cast<std::size_t>(index)];
        if (!IsCanonicalUserPtr(actor))
        {
            continue;
        }

        uptr actorClass = GetObjectClass(actor);
        if (actorClass == 0)
        {
            continue;
        }

        i32 rootComponentOffset = GetPropertyOffsetByName(
            actorClass,
            "RootComponent");
        if (rootComponentOffset < 0)
        {
            continue;
        }

        uptr rootComponent = 0;
        if (!ReadPtr(
            *ctx.mem,
            actor + rootComponentOffset,
            rootComponent) ||
            !IsCanonicalUserPtr(rootComponent))
        {
            continue;
        }

        uptr componentClass = GetObjectClass(rootComponent);
        if (componentClass == 0)
        {
            continue;
        }

        uptr property = FindRelativeLocationProperty(componentClass);
        if (property == 0)
        {
            continue;
        }

        i32 elementSize = 0;
        ReadValue(
            *ctx.mem,
            property + ctx.off.Property_ElementSize,
            elementSize);

        if (elementSize == 24)
        {
            usesDoublePrecision = true;
            detected = true;
            std::cerr << "[xrd] FVector 精度: double (UE5) "
                      << "[RelativeLocation.ElementSize=24]\n";
            break;
        }

        if (elementSize == 12)
        {
            detected = true;
            std::cerr << "[xrd] FVector 精度: float (UE4) "
                      << "[RelativeLocation.ElementSize=12]\n";
            break;
        }
    }

    if (!detected)
    {
        std::cerr << "[xrd] 无法检测 FVector 精度，默认 float\n";
    }

    ctx.off.bUseDoublePrecision = usesDoublePrecision;
    LogSlowInitPhase("FVector 精度检测", phaseTick);
    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace xrd
