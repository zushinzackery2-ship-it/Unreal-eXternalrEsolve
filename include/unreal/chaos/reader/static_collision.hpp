#pragma once

#include "reader.hpp"
#include <iostream>

namespace unreal
{

inline bool ChaosReader::ReadStaticCollision(
    uptr worldPtr,
    CollisionScene& outScene) const
{
    outScene.staticActors.clear();
    outScene.dynamicActors.clear();
    if (!IsCanonicalUserPtr(worldPtr))
    {
        return false;
    }

    std::vector<uptr> actorPtrs;
    if (!GetAllActors(
            m_mem,
            m_ueOff,
            worldPtr,
            actorPtrs))
    {
        std::cerr
            << "[unreal][Chaos] 无法枚举已加载 Levels\n";
        return false;
    }

    for (uptr actorPtr : actorPtrs)
    {
        ProcessActor(actorPtr, outScene);
    }

    std::size_t totalShapes = 0;
    for (const auto& actor : outScene.staticActors)
    {
        totalShapes += actor.shapes.size();
    }

    std::cerr << "[Chaos] ReadStaticCollision: actors="
              << outScene.staticActors.size()
              << " shapes=" << totalShapes;
    if (!outScene.staticActors.empty())
    {
        const auto& firstPosition =
            outScene.staticActors.front().shapes.front().worldPose.p;
        std::cerr << " first=("
                  << firstPosition.x << ","
                  << firstPosition.y << ","
                  << firstPosition.z << ")";
    }
    std::cerr << "\n";
    return true;
}

} // namespace unreal
