#pragma once

#include "reader.hpp"

namespace xrd
{

inline void ChaosReader::ProcessActor(
    uptr actorPtr,
    CollisionScene& outScene) const
{
    const uptr actorClass = GetObjectClass(actorPtr);
    if (actorClass == 0)
    {
        return;
    }

    const i32 rootComponentOffset =
        GetPropertyOffsetByName(actorClass, "RootComponent");
    if (rootComponentOffset < 0)
    {
        return;
    }

    uptr rootComponent = 0;
    if (!ReadPtr(
            m_mem,
            actorPtr + rootComponentOffset,
            rootComponent) ||
        !IsCanonicalUserPtr(rootComponent) ||
        m_off.PrimComp_BodyInstance < 0)
    {
        return;
    }

    CollisionActor actor;
    actor.address = actorPtr;
    actor.isDynamic = false;
    const uptr bodyInstanceAddress =
        rootComponent + m_off.PrimComp_BodyInstance;
    if (ReadBodyInstanceCollision(bodyInstanceAddress, actor) &&
        !actor.shapes.empty())
    {
        outScene.staticActors.push_back(std::move(actor));
    }
}

inline bool ChaosReader::ReadBodyInstanceCollision(
    uptr bodyInstanceAddress,
    CollisionActor& outActor) const
{
    if (m_off.PrimComp_BodyInstance < 0 ||
        m_off.BodyInstance_BodySetup < 0 ||
        m_off.BodySetup_AggGeom < 0)
    {
        return false;
    }

    i32 bodySetupWeakIndex = 0;
    if (!ReadValue(
            m_mem,
            bodyInstanceAddress + m_off.BodyInstance_BodySetup,
            bodySetupWeakIndex))
    {
        return false;
    }

    const uptr bodySetup = ResolveWeakObjectPtr(
        bodySetupWeakIndex);
    if (!IsCanonicalUserPtr(bodySetup))
    {
        return false;
    }

    Transform worldPose;
    if (!ReadWorldPoseFromBodyInstance(
            bodyInstanceAddress,
            worldPose))
    {
        return false;
    }

    const uptr aggregateGeometryAddress =
        bodySetup + m_off.BodySetup_AggGeom;
    ReadBoxElems(
        aggregateGeometryAddress,
        worldPose,
        outActor);
    ReadSphereElems(
        aggregateGeometryAddress,
        worldPose,
        outActor);
    ReadSphylElems(
        aggregateGeometryAddress,
        worldPose,
        outActor);
    ReadConvexElems(
        aggregateGeometryAddress,
        worldPose,
        outActor);
    return !outActor.shapes.empty();
}

inline uptr ChaosReader::ResolveWeakObjectPtr(
    i32 objectIndex) const
{
    if (objectIndex <= 0 || m_ueOff.GObjects == 0)
    {
        return 0;
    }

    const i32 chunkIndex = objectIndex >> 16;
    const i32 itemIndex = objectIndex & 0xFFFF;

    uptr chunks = 0;
    if (!ReadPtr(m_mem, m_ueOff.GObjects, chunks) ||
        !IsCanonicalUserPtr(chunks))
    {
        return 0;
    }

    uptr chunk = 0;
    if (!ReadPtr(
            m_mem,
            chunks + chunkIndex * sizeof(uptr),
            chunk) ||
        !IsCanonicalUserPtr(chunk))
    {
        return 0;
    }

    const uptr itemAddress =
        chunk +
        itemIndex * m_ueOff.FUObjectItemSize +
        m_ueOff.FUObjectItemInitialOffset;
    uptr object = 0;
    if (!ReadPtr(m_mem, itemAddress, object))
    {
        return 0;
    }
    return object;
}

} // namespace xrd
