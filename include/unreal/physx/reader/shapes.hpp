#pragma once

#include "reader.hpp"

namespace unreal
{

inline void PhysXReader::ReadShapes(
    uptr actorAddr,
    PxActorData& outData) const
{
    uptr shapesData = 0;
    u16 shapeCount = 0;
    if (!ReadPtr(
            m_mem,
            actorAddr + m_off.actor_shapesData,
            shapesData) ||
        !ReadValue(
            m_mem,
            actorAddr + m_off.actor_shapesCount,
            shapeCount) ||
        shapeCount == 0 ||
        shapeCount > 256)
    {
        return;
    }

    std::vector<uptr> shapeAddresses;
    if (shapeCount == 1)
    {
        if (IsCanonicalUserPtr(shapesData))
        {
            shapeAddresses.push_back(shapesData);
        }
    }
    else
    {
        if (!IsCanonicalUserPtr(shapesData))
        {
            return;
        }

        shapeAddresses.resize(shapeCount);
        if (!m_mem.Read(
                shapesData,
                shapeAddresses.data(),
                shapeCount * sizeof(uptr)))
        {
            return;
        }
    }

    outData.shapes.reserve(
        outData.shapes.size() + shapeAddresses.size());
    for (uptr shapeAddress : shapeAddresses)
    {
        if (!IsCanonicalUserPtr(shapeAddress))
        {
            continue;
        }

        PxShapeData shape;
        if (ReadShapeData(shapeAddress, shape) &&
            (shape.pxFlags & eSIMULATION_SHAPE) != 0)
        {
            outData.shapes.push_back(std::move(shape));
        }
    }
}

inline bool PhysXReader::ReadShapeData(
    uptr shapeAddr,
    PxShapeData& outData) const
{
    outData.pxFlags = ReadPxShapeFlags(shapeAddr);

    u8 storageFlags = 0;
    ReadValue(
        m_mem,
        shapeAddr + m_off.shape_flagByte,
        storageFlags);

    uptr externalStorage = 0;
    if ((storageFlags & 0x44) != 0)
    {
        ReadPtr(
            m_mem,
            shapeAddr + m_off.shape_extPtr,
            externalStorage);
    }

    if ((storageFlags & 0x04) != 0)
    {
        if (!IsCanonicalUserPtr(externalStorage))
        {
            return false;
        }
        ReadValue(m_mem, externalStorage, outData.localPose);
    }
    else
    {
        ReadValue(
            m_mem,
            shapeAddr + m_off.shape_localPose,
            outData.localPose);
    }

    uptr geometryAddress = 0;
    if ((storageFlags & 0x01) != 0)
    {
        if (!IsCanonicalUserPtr(externalStorage))
        {
            return false;
        }
        geometryAddress = externalStorage + 0x38;
    }
    else
    {
        geometryAddress = shapeAddr + m_off.shape_geomType;
    }

    return ReadGeometryData(geometryAddress, outData);
}

inline u32 PhysXReader::ReadPxShapeFlags(uptr shapeAddr) const
{
    u8 storageFlags = 0;
    ReadValue(
        m_mem,
        shapeAddr + m_off.shape_flagByte,
        storageFlags);

    u8 shapeFlags = 0;
    if ((storageFlags & m_off.shapeFlags_flagBit) == 0)
    {
        ReadValue(
            m_mem,
            shapeAddr + m_off.shapeFlagsInline,
            shapeFlags);
        return shapeFlags;
    }

    uptr externalStorage = 0;
    if (!ReadPtr(
            m_mem,
            shapeAddr + m_off.shape_extPtr,
            externalStorage) ||
        !IsCanonicalUserPtr(externalStorage))
    {
        return 0;
    }

    ReadValue(
        m_mem,
        externalStorage + m_off.shapeFlags_extOff,
        shapeFlags);
    return shapeFlags;
}

inline bool PhysXReader::ReadGeometryData(
    uptr geometryAddress,
    PxShapeData& outData) const
{
    u32 geometryType = eGEOM_INVALID;
    if (!ReadValue(m_mem, geometryAddress, geometryType))
    {
        return false;
    }
    outData.geomType = static_cast<PxGeometryType>(geometryType);

    const uptr dataAddress = geometryAddress + 4;
    switch (outData.geomType)
    {
        case eBOX:
            ReadValue(m_mem, dataAddress, outData.geom.box);
            break;
        case eSPHERE:
            ReadValue(m_mem, dataAddress, outData.geom.sphere);
            break;
        case eCAPSULE:
            ReadValue(m_mem, dataAddress, outData.geom.capsule);
            break;
        case eCONVEXMESH:
            return ReadConvexMeshData(dataAddress, outData);
        default:
            break;
    }
    return true;
}

} // namespace unreal
