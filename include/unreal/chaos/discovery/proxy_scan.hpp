#pragma once

#include "../../core/context.hpp"
#include "../../engine/world/world_levels.hpp"
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <vector>

namespace unreal
{
namespace detail
{

inline void ScanChaosPhysicsProxyByActors(
    Context& ctx,
    const std::vector<uptr>& actorPtrs)
{
    auto& mem = *ctx.mem;
    auto& offsets = ctx.chaosOff;
    if (offsets.BodyInstance_PhysicsProxy >= 0 ||
        offsets.BodyInstance_StructSize <= 0 ||
        offsets.PrimComp_BodyInstance < 0)
    {
        return;
    }

    std::cerr
        << "[unreal][Chaos] 扫描 FBodyInstance 私有物理句柄 ...\n";

    const i32 actorCount = std::min(
        static_cast<i32>(actorPtrs.size()),
        200);
    bool found = false;
    for (i32 index = 0; index < actorCount && !found; ++index)
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

        const i32 bodyInstanceOffset = GetPropertyOffsetByName(
            componentClass,
            "BodyInstance");
        if (bodyInstanceOffset < 0)
        {
            continue;
        }

        const uptr bodyInstanceAddress =
            rootComponent + bodyInstanceOffset;
        const i32 scanStart = std::max(
            offsets.BodyInstance_StructSize - 0x40,
            0x20);
        const i32 scanEnd = offsets.BodyInstance_StructSize;

        for (i32 probeOffset = scanStart;
             probeOffset < scanEnd;
             probeOffset += 8)
        {
            uptr proxyCandidate = 0;
            if (!ReadPtr(
                    mem,
                    bodyInstanceAddress + probeOffset,
                    proxyCandidate) ||
                !IsCanonicalUserPtr(proxyCandidate))
            {
                continue;
            }

            uptr particle = 0;
            if (!ReadPtr(
                    mem,
                    proxyCandidate + offsets.Proxy_Particle,
                    particle) ||
                !IsCanonicalUserPtr(particle))
            {
                continue;
            }

            double positionX = 0;
            double positionY = 0;
            double positionZ = 0;
            ReadValue(
                mem,
                particle + offsets.Particle_PosX,
                positionX);
            ReadValue(
                mem,
                particle + offsets.Particle_PosY,
                positionY);
            ReadValue(
                mem,
                particle + offsets.Particle_PosZ,
                positionZ);

            const bool inRange =
                std::abs(positionX) < 1e8 &&
                std::abs(positionY) < 1e8 &&
                std::abs(positionZ) < 1e8;
            const bool nonZero =
                std::abs(positionX) > 1e-6 ||
                std::abs(positionY) > 1e-6 ||
                std::abs(positionZ) > 1e-6;
            if (!inRange || !nonZero)
            {
                continue;
            }

            offsets.BodyInstance_PhysicsProxy = probeOffset;
            std::cerr << std::format(
                "[unreal][Chaos] FBodyInstance::PhysicsProxy = "
                "+0x{:X} (Pos: {:.1f}, {:.1f}, {:.1f})\n",
                probeOffset,
                positionX,
                positionY,
                positionZ);
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cerr
            << "[unreal][Chaos] FBodyInstance 私有句柄未找到\n";
    }
}

} // namespace detail
} // namespace unreal
