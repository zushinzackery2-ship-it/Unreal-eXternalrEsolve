#pragma once

#include "reader.hpp"

namespace xrd
{

inline void ChaosReader::ReadBoxElems(
    uptr aggregateGeometryAddress,
    const Transform& worldPose,
    CollisionActor& outActor) const
{
    uptr data = 0;
    i32 count = 0;
    if (!ReadElementArray(
            aggregateGeometryAddress,
            m_off.AggGeom_BoxElems,
            1024,
            data,
            count))
    {
        return;
    }

    for (i32 index = 0; index < count; ++index)
    {
        const uptr element =
            data + index * m_off.BoxElem_Size;
        double centerX = 0;
        double centerY = 0;
        double centerZ = 0;
        ReadValue(
            m_mem,
            element + m_off.BoxElem_Center,
            centerX);
        ReadValue(
            m_mem,
            element + m_off.BoxElem_Center + 8,
            centerY);
        ReadValue(
            m_mem,
            element + m_off.BoxElem_Center + 16,
            centerZ);

        double pitch = 0;
        double yaw = 0;
        double roll = 0;
        ReadValue(
            m_mem,
            element + m_off.BoxElem_Rotation,
            pitch);
        ReadValue(
            m_mem,
            element + m_off.BoxElem_Rotation + 8,
            yaw);
        ReadValue(
            m_mem,
            element + m_off.BoxElem_Rotation + 16,
            roll);

        float sizeX = 0;
        float sizeY = 0;
        float sizeZ = 0;
        ReadValue(m_mem, element + m_off.BoxElem_X, sizeX);
        ReadValue(m_mem, element + m_off.BoxElem_Y, sizeY);
        ReadValue(m_mem, element + m_off.BoxElem_Z, sizeZ);

        CollisionShape shape;
        shape.geomType = eGEOM_BOX;
        shape.box =
        {
            sizeX * 0.5f,
            sizeY * 0.5f,
            sizeZ * 0.5f
        };

        Transform localPose;
        localPose.p =
        {
            static_cast<float>(centerX),
            static_cast<float>(centerY),
            static_cast<float>(centerZ)
        };
        localPose.q = EulerToQuat(pitch, yaw, roll);
        shape.worldPose = TransformMul(worldPose, localPose);
        outActor.shapes.push_back(std::move(shape));
    }
}

inline void ChaosReader::ReadSphereElems(
    uptr aggregateGeometryAddress,
    const Transform& worldPose,
    CollisionActor& outActor) const
{
    uptr data = 0;
    i32 count = 0;
    if (!ReadElementArray(
            aggregateGeometryAddress,
            m_off.AggGeom_SphereElems,
            1024,
            data,
            count))
    {
        return;
    }

    for (i32 index = 0; index < count; ++index)
    {
        const uptr element =
            data + index * m_off.SphereElem_Size;
        double centerX = 0;
        double centerY = 0;
        double centerZ = 0;
        ReadValue(
            m_mem,
            element + m_off.SphereElem_Center,
            centerX);
        ReadValue(
            m_mem,
            element + m_off.SphereElem_Center + 8,
            centerY);
        ReadValue(
            m_mem,
            element + m_off.SphereElem_Center + 16,
            centerZ);

        float radius = 0;
        ReadValue(
            m_mem,
            element + m_off.SphereElem_Radius,
            radius);

        CollisionShape shape;
        shape.geomType = eGEOM_SPHERE;
        shape.sphere = { radius };

        Transform localPose;
        localPose.p =
        {
            static_cast<float>(centerX),
            static_cast<float>(centerY),
            static_cast<float>(centerZ)
        };
        shape.worldPose = TransformMul(worldPose, localPose);
        outActor.shapes.push_back(std::move(shape));
    }
}

inline void ChaosReader::ReadSphylElems(
    uptr aggregateGeometryAddress,
    const Transform& worldPose,
    CollisionActor& outActor) const
{
    uptr data = 0;
    i32 count = 0;
    if (!ReadElementArray(
            aggregateGeometryAddress,
            m_off.AggGeom_SphylElems,
            1024,
            data,
            count))
    {
        return;
    }

    for (i32 index = 0; index < count; ++index)
    {
        const uptr element =
            data + index * m_off.SphylElem_Size;
        double centerX = 0;
        double centerY = 0;
        double centerZ = 0;
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Center,
            centerX);
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Center + 8,
            centerY);
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Center + 16,
            centerZ);

        double pitch = 0;
        double yaw = 0;
        double roll = 0;
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Rotation,
            pitch);
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Rotation + 8,
            yaw);
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Rotation + 16,
            roll);

        float radius = 0;
        float length = 0;
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Radius,
            radius);
        ReadValue(
            m_mem,
            element + m_off.SphylElem_Length,
            length);

        CollisionShape shape;
        shape.geomType = eGEOM_CAPSULE;
        shape.capsule = { radius, length * 0.5f };

        Transform localPose;
        localPose.p =
        {
            static_cast<float>(centerX),
            static_cast<float>(centerY),
            static_cast<float>(centerZ)
        };
        localPose.q = EulerToQuat(pitch, yaw, roll);
        shape.worldPose = TransformMul(worldPose, localPose);
        outActor.shapes.push_back(std::move(shape));
    }
}

} // namespace xrd
