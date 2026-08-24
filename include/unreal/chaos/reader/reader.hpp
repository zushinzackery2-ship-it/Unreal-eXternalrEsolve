#pragma once

#include "../chaos_types.hpp"
#include "../../core/context.hpp"
#include "../../engine/world/world_levels.hpp"
#include <cmath>
#include <cstring>
#include <set>
#include <utility>
#include <vector>

namespace unreal
{

class ChaosReader
{
public:
    ChaosReader(
        const IMemoryAccessor& mem,
        const UEOffsets& ueOffsets,
        const ChaosOffsets& chaosOffsets,
        uptr moduleBase = 0)
        : m_mem(mem),
          m_ueOff(ueOffsets),
          m_off(chaosOffsets),
          m_moduleBase(moduleBase)
    {
    }

    ChaosOffsets& Offsets()
    {
        return m_off;
    }

    const ChaosOffsets& Offsets() const
    {
        return m_off;
    }

    bool ReadStaticCollision(
        uptr worldPtr,
        CollisionScene& outScene) const;

private:
    const IMemoryAccessor& m_mem;
    const UEOffsets& m_ueOff;
    ChaosOffsets m_off;
    uptr m_moduleBase = 0;

    void ProcessActor(
        uptr actorPtr,
        CollisionScene& outScene) const;
    bool ReadBodyInstanceCollision(
        uptr bodyInstanceAddr,
        CollisionActor& outActor) const;
    uptr ResolveWeakObjectPtr(i32 objectIndex) const;
    bool ReadWorldPoseFromBodyInstance(
        uptr bodyInstanceAddr,
        Transform& outPose) const;

    bool ReadElementArray(
        uptr aggregateGeometryAddress,
        i32 arrayOffset,
        i32 maximumCount,
        uptr& outData,
        i32& outCount) const;
    void ReadBoxElems(
        uptr aggregateGeometryAddress,
        const Transform& worldPose,
        CollisionActor& outActor) const;
    void ReadSphereElems(
        uptr aggregateGeometryAddress,
        const Transform& worldPose,
        CollisionActor& outActor) const;
    void ReadSphylElems(
        uptr aggregateGeometryAddress,
        const Transform& worldPose,
        CollisionActor& outActor) const;
    void ReadConvexElems(
        uptr aggregateGeometryAddress,
        const Transform& worldPose,
        CollisionActor& outActor) const;

    static Quat EulerToQuat(
        double pitchDegrees,
        double yawDegrees,
        double rollDegrees);
};

} // namespace unreal
