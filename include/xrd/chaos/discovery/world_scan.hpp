#pragma once

#include "../../core/context.hpp"
#include <format>
#include <iostream>

namespace xrd
{
namespace detail
{

inline void ScanChaosPhysicsScene(Context& ctx, uptr world)
{
    auto& mem = *ctx.mem;
    auto& engineOffsets = ctx.off;
    auto& chaosOffsets = ctx.chaosOff;
    if (chaosOffsets.UWorld_PhysScene >= 0)
    {
        return;
    }

    std::cerr
        << "[xrd][Chaos] 扫描 UWorld -> FPhysScene_Chaos ...\n";
    for (i32 testOffset = 0x100;
         testOffset < 0x800;
         testOffset += 8)
    {
        uptr candidate = 0;
        if (!ReadPtr(mem, world + testOffset, candidate) ||
            !IsCanonicalUserPtr(candidate))
        {
            continue;
        }

        uptr ownerWorld = 0;
        if (!ReadPtr(
                mem,
                candidate + chaosOffsets.PhysScene_OwnerWorld,
                ownerWorld) ||
            ownerWorld != world)
        {
            continue;
        }

        uptr solver = 0;
        if (!ReadPtr(
                mem,
                candidate + chaosOffsets.PhysScene_Solver,
                solver) ||
            !IsCanonicalUserPtr(solver))
        {
            continue;
        }

        uptr solverScene = 0;
        if (!ReadPtr(
                mem,
                solver + chaosOffsets.Solver_PhysScene,
                solverScene) ||
            solverScene != candidate)
        {
            continue;
        }

        chaosOffsets.UWorld_PhysScene = testOffset;
        engineOffsets.ChaosPhysScene = candidate;
        std::cerr << std::format(
            "[xrd][Chaos] FPhysScene_Chaos 找到: "
            "UWorld+0x{:X} = 0x{:X}\n",
            testOffset,
            candidate);
        return;
    }

    std::cerr
        << "[xrd][Chaos] FPhysScene_Chaos 未找到"
        << "（碰撞场景链不可用）\n";
}

} // namespace detail
} // namespace xrd
