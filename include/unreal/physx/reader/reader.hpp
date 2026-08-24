#pragma once

#include "../physx_convex.hpp"
#include "../physx_types.hpp"
#include "../../memory/memory.hpp"
#include <cmath>
#include <vector>

namespace unreal
{

class PhysXReader
{
public:
    PhysXReader(const IMemoryAccessor& mem, uptr physxGlobalPtr)
        : m_mem(mem), m_globalPtr(physxGlobalPtr)
    {
    }

    PhysXOffsets& Offsets()
    {
        return m_off;
    }

    const PhysXOffsets& Offsets() const
    {
        return m_off;
    }

    bool ReadStaticCollision(PxSceneData& outData) const;
    bool ReadActorData(uptr actorAddr, PxActorData& outData) const;

private:
    const IMemoryAccessor& m_mem;
    uptr m_globalPtr = 0;
    PhysXOffsets m_off;

    std::vector<uptr> ReadSceneList() const;
    std::vector<uptr> ReadActorList(uptr scenePtr) const;

    bool ReadStaticGlobalPose(
        uptr actorAddr,
        PxTransform& outPose) const;
    bool ReadDynamicGlobalPose(
        uptr actorAddr,
        PxTransform& outPose) const;

    void ReadShapes(uptr actorAddr, PxActorData& outData) const;
    bool ReadShapeData(uptr shapeAddr, PxShapeData& outData) const;
    u32 ReadPxShapeFlags(uptr shapeAddr) const;
    bool ReadGeometryData(uptr geomAddr, PxShapeData& outData) const;
    bool ReadConvexMeshData(
        uptr geomDataAddr,
        PxShapeData& outData) const;

    static constexpr u32 kConvexGeomMeshPtr = 0x1C;
    static constexpr u32 kConvexVertices = 0x20;
    static constexpr u32 kConvexPolygons = 0x30;
    static constexpr u32 kConvexNbVertices = 0x4A;
    static constexpr u32 kConvexNbPolygons = 0x4B;
};

} // namespace unreal
