#pragma once

#include "reader.hpp"

namespace xrd
{

inline bool PhysXReader::ReadStaticCollision(PxSceneData& outData) const
{
    outData.staticActors.clear();
    outData.dynamicActors.clear();

    for (uptr scenePtr : ReadSceneList())
    {
        for (uptr actorPtr : ReadActorList(scenePtr))
        {
            PxActorData actor;
            if (!ReadActorData(actorPtr, actor) ||
                actor.type != eRIGID_STATIC)
            {
                continue;
            }

            bool hasRenderableShape = false;
            for (const auto& shape : actor.shapes)
            {
                if (shape.geomType == eBOX ||
                    shape.geomType == eSPHERE ||
                    shape.geomType == eCAPSULE ||
                    shape.geomType == eCONVEXMESH)
                {
                    hasRenderableShape = true;
                    break;
                }
            }

            if (hasRenderableShape)
            {
                outData.staticActors.push_back(std::move(actor));
            }
        }
    }

    return true;
}

inline bool PhysXReader::ReadActorData(
    uptr actorAddr,
    PxActorData& outData) const
{
    outData.address = actorAddr;

    u16 rawType = 0;
    if (!ReadValue(m_mem, actorAddr + m_off.actor_type, rawType))
    {
        return false;
    }

    if (rawType == m_off.staticType)
    {
        outData.type = eRIGID_STATIC;
        if (!ReadStaticGlobalPose(actorAddr, outData.globalPose))
        {
            return false;
        }
    }
    else if (rawType == m_off.dynamicType)
    {
        outData.type = eRIGID_DYNAMIC;
        if (!ReadDynamicGlobalPose(actorAddr, outData.globalPose))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    ReadShapes(actorAddr, outData);
    return true;
}

inline std::vector<uptr> PhysXReader::ReadSceneList() const
{
    std::vector<uptr> result;
    uptr scenesData = 0;
    u32 sceneCount = 0;
    if (!ReadPtr(
            m_mem,
            m_globalPtr + m_off.physics_scenes,
            scenesData) ||
        !ReadValue(
            m_mem,
            m_globalPtr + m_off.physics_scenesCount,
            sceneCount))
    {
        return result;
    }

    sceneCount &= 0x7FFFFFFF;
    if (!IsCanonicalUserPtr(scenesData) ||
        sceneCount == 0 ||
        sceneCount > 64)
    {
        return result;
    }

    result.resize(sceneCount);
    if (!m_mem.Read(
            scenesData,
            result.data(),
            sceneCount * sizeof(uptr)))
    {
        result.clear();
    }
    return result;
}

inline std::vector<uptr> PhysXReader::ReadActorList(
    uptr scenePtr) const
{
    std::vector<uptr> result;
    if (!IsCanonicalUserPtr(scenePtr))
    {
        return result;
    }

    uptr actorsData = 0;
    u32 actorCount = 0;
    if (!ReadPtr(
            m_mem,
            scenePtr + m_off.scene_actors,
            actorsData) ||
        !ReadValue(
            m_mem,
            scenePtr + m_off.scene_actorsCount,
            actorCount))
    {
        return result;
    }

    if (!IsCanonicalUserPtr(actorsData) ||
        actorCount == 0 ||
        actorCount > 100000)
    {
        return result;
    }

    result.resize(actorCount);
    if (!m_mem.Read(
            actorsData,
            result.data(),
            actorCount * sizeof(uptr)))
    {
        result.clear();
    }
    return result;
}

} // namespace xrd
