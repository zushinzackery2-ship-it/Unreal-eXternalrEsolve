#pragma once

#include "reader.hpp"

namespace xrd
{

inline void ChaosReader::ReadConvexElems(
    uptr aggregateGeometryAddress,
    const Transform& worldPose,
    CollisionActor& outActor) const
{
    uptr data = 0;
    i32 count = 0;
    if (!ReadElementArray(
            aggregateGeometryAddress,
            m_off.AggGeom_ConvexElems,
            256,
            data,
            count))
    {
        return;
    }

    for (i32 index = 0; index < count; ++index)
    {
        const uptr element =
            data + index * m_off.ConvexElem_Size;

        uptr verticesData = 0;
        i32 vertexCount = 0;
        if (!ReadPtr(
                m_mem,
                element + m_off.ConvexElem_VertexData,
                verticesData) ||
            !ReadValue(
                m_mem,
                element + m_off.ConvexElem_VertexData + 8,
                vertexCount) ||
            !IsCanonicalUserPtr(verticesData) ||
            vertexCount <= 0 ||
            vertexCount > 4096)
        {
            continue;
        }

        const i32 vectorSize =
            m_ueOff.bUseDoublePrecision ? 24 : 12;
        std::vector<u8> vertexBuffer(
            static_cast<std::size_t>(vertexCount) *
            static_cast<std::size_t>(vectorSize));
        if (!m_mem.Read(
                verticesData,
                vertexBuffer.data(),
                vertexBuffer.size()))
        {
            continue;
        }

        CollisionShape shape;
        shape.geomType = eGEOM_CONVEX;
        shape.convexVerts.reserve(
            static_cast<std::size_t>(vertexCount));
        for (i32 vertexIndex = 0;
             vertexIndex < vertexCount;
             ++vertexIndex)
        {
            Vec3 vertex;
            if (m_ueOff.bUseDoublePrecision)
            {
                double x = 0;
                double y = 0;
                double z = 0;
                std::memcpy(
                    &x,
                    &vertexBuffer[vertexIndex * 24],
                    sizeof(x));
                std::memcpy(
                    &y,
                    &vertexBuffer[vertexIndex * 24 + 8],
                    sizeof(y));
                std::memcpy(
                    &z,
                    &vertexBuffer[vertexIndex * 24 + 16],
                    sizeof(z));
                vertex =
                {
                    static_cast<float>(x),
                    static_cast<float>(y),
                    static_cast<float>(z)
                };
            }
            else
            {
                std::memcpy(
                    &vertex.x,
                    &vertexBuffer[vertexIndex * 12],
                    sizeof(vertex.x));
                std::memcpy(
                    &vertex.y,
                    &vertexBuffer[vertexIndex * 12 + 4],
                    sizeof(vertex.y));
                std::memcpy(
                    &vertex.z,
                    &vertexBuffer[vertexIndex * 12 + 8],
                    sizeof(vertex.z));
            }
            shape.convexVerts.push_back(vertex);
        }

        uptr indexData = 0;
        i32 indexCount = 0;
        ReadPtr(
            m_mem,
            element + m_off.ConvexElem_IndexData,
            indexData);
        ReadValue(
            m_mem,
            element + m_off.ConvexElem_IndexData + 8,
            indexCount);
        if (IsCanonicalUserPtr(indexData) &&
            indexCount >= 3 &&
            indexCount <= 65536)
        {
            std::vector<i32> indices(
                static_cast<std::size_t>(indexCount));
            if (m_mem.Read(
                    indexData,
                    indices.data(),
                    indices.size() * sizeof(i32)))
            {
                std::set<std::pair<u8, u8>> edgeSet;
                const auto addEdge =
                    [&](u8 first, u8 second)
                    {
                        if (first > second)
                        {
                            std::swap(first, second);
                        }
                        edgeSet.insert({ first, second });
                    };

                for (i32 triangle = 0;
                     triangle + 2 < indexCount;
                     triangle += 3)
                {
                    const i32 first = indices[triangle];
                    const i32 second = indices[triangle + 1];
                    const i32 third = indices[triangle + 2];
                    if (first < 0 || first >= 256 ||
                        second < 0 || second >= 256 ||
                        third < 0 || third >= 256)
                    {
                        continue;
                    }

                    addEdge(
                        static_cast<u8>(first),
                        static_cast<u8>(second));
                    addEdge(
                        static_cast<u8>(second),
                        static_cast<u8>(third));
                    addEdge(
                        static_cast<u8>(third),
                        static_cast<u8>(first));
                }
                shape.convexEdges.assign(
                    edgeSet.begin(),
                    edgeSet.end());
            }
        }

        shape.worldPose = worldPose;
        outActor.shapes.push_back(std::move(shape));
    }
}

} // namespace xrd
