#pragma once

#include "actor_walk.hpp"
#include "proxy_scan.hpp"
#include "reflection.hpp"
#include "world_scan.hpp"
#include "../../engine/world/world_levels.hpp"
#include <iostream>
#include <vector>

namespace xrd
{

inline void InitChaosOffsets(Context& ctx)
{
    auto& mem = *ctx.mem;
    auto& engineOffsets = ctx.off;

    std::cerr << "[xrd][Chaos] === 开始偏移发现 ===\n";

    uptr world = 0;
    if (!ReadPtr(mem, engineOffsets.GWorld, world) ||
        !IsCanonicalUserPtr(world))
    {
        std::cerr << "[xrd][Chaos] 无法读取 GWorld\n";
        return;
    }

    detail::DiscoverChaosSdkOffsets(ctx);

    std::vector<uptr> actorPtrs;
    bool actorsLoaded = false;
    const auto ensureActorPtrs = [&]() -> const std::vector<uptr>&
    {
        if (!actorsLoaded)
        {
            actorPtrs = GetAllActors();
            actorsLoaded = true;
        }
        return actorPtrs;
    };

    detail::DiscoverChaosSdkOffsetsByActorWalk(
        ctx,
        ensureActorPtrs());
    detail::ScanChaosPhysicsScene(ctx, world);
    detail::ScanChaosPhysicsProxyByActors(
        ctx,
        ensureActorPtrs());

    std::cerr << "[xrd][Chaos] === 偏移发现完成 ===\n";
}

} // namespace xrd
