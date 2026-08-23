#pragma once

#include "reader.hpp"

namespace xrd
{

inline bool PhysXReader::ReadConvexMeshData(
    uptr geometryDataAddress,
    PxShapeData& outData) const
{
    uptr meshPtr = 0;
    if (!ReadPtr(
            m_mem,
            geometryDataAddress + kConvexGeomMeshPtr,
            meshPtr) ||
        !IsCanonicalUserPtr(meshPtr))
    {
        return false;
    }

    u8 vertexCount = 0;
    if (!ReadValue(
            m_mem,
            meshPtr + kConvexNbVertices,
            vertexCount) ||
        vertexCount == 0)
    {
        return false;
    }

    uptr verticesPtr = 0;
    if (!ReadPtr(
            m_mem,
            meshPtr + kConvexVertices,
            verticesPtr) ||
        !IsCanonicalUserPtr(verticesPtr))
    {
        return false;
    }

    outData.convexVerts.resize(vertexCount);
    if (!m_mem.Read(
            verticesPtr,
            outData.convexVerts.data(),
            vertexCount * sizeof(PxVec3)))
    {
        outData.convexVerts.clear();
        return false;
    }

    u8 polygonCount = 0;
    ReadValue(
        m_mem,
        meshPtr + kConvexNbPolygons,
        polygonCount);

    uptr polygonsPtr = 0;
    ReadPtr(
        m_mem,
        meshPtr + kConvexPolygons,
        polygonsPtr);
    if (polygonCount > 0 && IsCanonicalUserPtr(polygonsPtr))
    {
        ReadPhysXConvexTopology(
            m_mem,
            polygonsPtr,
            polygonCount,
            vertexCount,
            outData.convexEdges,
            outData.convexTriangles);
    }

    return true;
}

} // namespace xrd
