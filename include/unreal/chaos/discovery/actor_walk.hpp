#pragma once

#include "../../core/context.hpp"
#include "../../engine/world/world_levels.hpp"
#include <algorithm>
#include <format>
#include <iostream>
#include <vector>

namespace unreal
{
namespace detail
{

inline void DiscoverChaosSdkOffsetsByActorWalk(
    Context& ctx,
    const std::vector<uptr>& actorPtrs)
{
    auto& mem = *ctx.mem;
    auto& engineOffsets = ctx.off;
    auto& chaosOffsets = ctx.chaosOff;

    bool needBodyInstance =
        chaosOffsets.PrimComp_BodyInstance < 0;
    bool needAggregateGeometry =
        chaosOffsets.BodySetup_AggGeom < 0;
    bool needBodyInstanceSize =
        chaosOffsets.BodyInstance_StructSize < 0;
    if (!needBodyInstance &&
        !needAggregateGeometry &&
        !needBodyInstanceSize)
    {
        return;
    }

    const i32 actorCount = std::min(
        static_cast<i32>(actorPtrs.size()),
        300);
    for (i32 index = 0; index < actorCount; ++index)
    {
        const uptr actorPtr =
            actorPtrs[static_cast<std::size_t>(index)];
        if (!IsCanonicalUserPtr(actorPtr))
        {
            continue;
        }

        const uptr actorClass = GetObjectClass(actorPtr);
        if (actorClass == 0)
        {
            continue;
        }

        const i32 rootComponentOffset =
            GetPropertyOffsetByName(actorClass, "RootComponent");
        if (rootComponentOffset < 0)
        {
            continue;
        }

        uptr rootComponent = 0;
        if (!ReadPtr(
                mem,
                actorPtr + rootComponentOffset,
                rootComponent) ||
            !IsCanonicalUserPtr(rootComponent))
        {
            continue;
        }

        const uptr componentClass = GetObjectClass(rootComponent);
        if (componentClass == 0)
        {
            continue;
        }

        if (needBodyInstance)
        {
            const i32 bodyInstanceOffset =
                GetPropertyOffsetByName(
                    componentClass,
                    "BodyInstance");
            if (bodyInstanceOffset >= 0)
            {
                chaosOffsets.PrimComp_BodyInstance =
                    bodyInstanceOffset;
                needBodyInstance = false;
                std::cerr << std::format(
                    "[unreal][Chaos] PrimComp::BodyInstance = "
                    "+0x{:X} (actor-walk)\n",
                    bodyInstanceOffset);
            }
        }

        if (needAggregateGeometry &&
            chaosOffsets.PrimComp_BodyInstance >= 0 &&
            chaosOffsets.BodyInstance_BodySetup >= 0)
        {
            i32 bodySetupWeakIndex = 0;
            const uptr bodyInstanceAddress =
                rootComponent +
                chaosOffsets.PrimComp_BodyInstance;
            if (ReadValue(
                    mem,
                    bodyInstanceAddress +
                        chaosOffsets.BodyInstance_BodySetup,
                    bodySetupWeakIndex) &&
                bodySetupWeakIndex > 0)
            {
                const uptr bodySetup =
                    GetObjectByIndex(bodySetupWeakIndex);
                const uptr bodySetupClass =
                    IsCanonicalUserPtr(bodySetup)
                        ? GetObjectClass(bodySetup)
                        : 0;
                if (bodySetupClass != 0)
                {
                    const i32 aggregateGeometryOffset =
                        GetPropertyOffsetByName(
                            bodySetupClass,
                            "AggGeom");
                    if (aggregateGeometryOffset >= 0)
                    {
                        chaosOffsets.BodySetup_AggGeom =
                            aggregateGeometryOffset;
                        needAggregateGeometry = false;
                        std::cerr << std::format(
                            "[unreal][Chaos] BodySetup::AggGeom = "
                            "+0x{:X} (actor-walk)\n",
                            aggregateGeometryOffset);
                    }
                }
            }
        }

        if (needBodyInstanceSize &&
            chaosOffsets.PrimComp_BodyInstance >= 0 &&
            engineOffsets.StructProperty_Struct >= 0 &&
            engineOffsets.UStruct_Size >= 0)
        {
            uptr bodyInstanceProperty = 0;
            uptr currentClass = componentClass;
            while (currentClass != 0 && bodyInstanceProperty == 0)
            {
                bodyInstanceProperty = FindPropertyInStruct(
                    currentClass,
                    "BodyInstance");
                if (bodyInstanceProperty == 0)
                {
                    currentClass = GetSuperStruct(currentClass);
                }
            }

            uptr bodyInstanceStruct = 0;
            if (bodyInstanceProperty != 0)
            {
                ReadPtr(
                    mem,
                    bodyInstanceProperty +
                        engineOffsets.StructProperty_Struct,
                    bodyInstanceStruct);
            }
            if (IsCanonicalUserPtr(bodyInstanceStruct))
            {
                i32 structSize = 0;
                ReadValue(
                    mem,
                    bodyInstanceStruct + engineOffsets.UStruct_Size,
                    structSize);
                if (structSize > 0 && structSize < 0x1000)
                {
                    chaosOffsets.BodyInstance_StructSize =
                        structSize;
                    needBodyInstanceSize = false;
                    std::cerr << std::format(
                        "[unreal][Chaos] FBodyInstance size = "
                        "0x{:X} (actor-walk)\n",
                        structSize);
                }
            }
        }

        if (!needBodyInstance &&
            !needAggregateGeometry &&
            !needBodyInstanceSize)
        {
            break;
        }
    }

    if (needBodyInstance)
    {
        std::cerr
            << "[unreal][Chaos] PrimComp::BodyInstance 未找到\n";
    }
    if (needAggregateGeometry)
    {
        std::cerr
            << "[unreal][Chaos] BodySetup::AggGeom 未找到\n";
    }
    if (needBodyInstanceSize)
    {
        std::cerr
            << "[unreal][Chaos] FBodyInstance 结构大小未知\n";
    }
}

} // namespace detail
} // namespace unreal
