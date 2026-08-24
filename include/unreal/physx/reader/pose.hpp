#pragma once

#include "reader.hpp"

namespace unreal
{

inline bool PhysXReader::ReadStaticGlobalPose(
    uptr actorAddr,
    PxTransform& outPose) const
{
    u8 storageFlags = 0;
    ReadValue(
        m_mem,
        actorAddr + m_off.staticPose_flag,
        storageFlags);

    if ((storageFlags & m_off.staticPose_flagBit) == 0)
    {
        return ReadValue(
            m_mem,
            actorAddr + m_off.staticPose_inline,
            outPose);
    }

    uptr externalStorage = 0;
    if (!ReadPtr(
            m_mem,
            actorAddr + m_off.staticPose_extPtr,
            externalStorage) ||
        !IsCanonicalUserPtr(externalStorage))
    {
        return false;
    }

    return ReadValue(
        m_mem,
        externalStorage + m_off.staticPose_extOffset,
        outPose);
}

inline bool PhysXReader::ReadDynamicGlobalPose(
    uptr actorAddr,
    PxTransform& outPose) const
{
    PxTransform bodyToWorld{};
    u32 storageFlags = 0;
    ReadValue(
        m_mem,
        actorAddr + m_off.dynPose_flagAddr,
        storageFlags);

    if ((storageFlags & m_off.dynPose_flagBit) != 0)
    {
        uptr externalStorage = 0;
        if (!ReadPtr(
                m_mem,
                actorAddr + m_off.dynPose_extPtr,
                externalStorage) ||
            !IsCanonicalUserPtr(externalStorage) ||
            !ReadValue(
                m_mem,
                externalStorage + m_off.dynPose_extOffset,
                bodyToWorld))
        {
            return false;
        }
    }
    else if (!ReadValue(
                 m_mem,
                 actorAddr + m_off.dynPose_b2wInline,
                 bodyToWorld))
    {
        return false;
    }

    PxTransform bodyToActor{};
    ReadValue(
        m_mem,
        actorAddr + m_off.dynPose_body2actor,
        bodyToActor);

    const bool isIdentity =
        std::abs(bodyToActor.q.x) < 1e-5f &&
        std::abs(bodyToActor.q.y) < 1e-5f &&
        std::abs(bodyToActor.q.z) < 1e-5f &&
        std::abs(bodyToActor.q.w - 1.0f) < 1e-5f &&
        std::abs(bodyToActor.p.x) < 1e-5f &&
        std::abs(bodyToActor.p.y) < 1e-5f &&
        std::abs(bodyToActor.p.z) < 1e-5f;
    if (isIdentity)
    {
        outPose = bodyToWorld;
        return true;
    }

    const PxQuat inverseRotation = QuatConjugate(bodyToActor.q);
    const PxVec3 inversePosition = QuatRotate(
        inverseRotation,
        {
            -bodyToActor.p.x,
            -bodyToActor.p.y,
            -bodyToActor.p.z
        });
    const PxTransform inverseBodyToActor =
    {
        inverseRotation,
        inversePosition
    };
    outPose = TransformMultiply(bodyToWorld, inverseBodyToActor);
    return true;
}

} // namespace unreal
