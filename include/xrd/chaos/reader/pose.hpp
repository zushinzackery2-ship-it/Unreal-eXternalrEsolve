#pragma once

#include "reader.hpp"

namespace xrd
{

inline bool ChaosReader::ReadWorldPoseFromBodyInstance(
    uptr bodyInstanceAddress,
    Transform& outPose) const
{
    outPose = {};
    if (m_off.BodyInstance_PhysicsProxy < 0)
    {
        return false;
    }

    uptr proxy = 0;
    if (!ReadPtr(
            m_mem,
            bodyInstanceAddress + m_off.BodyInstance_PhysicsProxy,
            proxy) ||
        !IsCanonicalUserPtr(proxy))
    {
        return false;
    }

    uptr proxyVtable = 0;
    if (!ReadPtr(m_mem, proxy, proxyVtable) ||
        m_moduleBase == 0 ||
        proxyVtable < m_moduleBase ||
        proxyVtable > m_moduleBase + 0x8000000)
    {
        return false;
    }

    uptr particle = 0;
    if (!ReadPtr(
            m_mem,
            proxy + m_off.Proxy_Particle,
            particle) ||
        !IsCanonicalUserPtr(particle))
    {
        return false;
    }

    ChaosWorldPose pose;
    const bool poseRead =
        ReadValue(
            m_mem,
            particle + m_off.Particle_PosX,
            pose.posX) &&
        ReadValue(
            m_mem,
            particle + m_off.Particle_PosY,
            pose.posY) &&
        ReadValue(
            m_mem,
            particle + m_off.Particle_PosZ,
            pose.posZ) &&
        ReadValue(
            m_mem,
            particle + m_off.Particle_RotX,
            pose.rotX) &&
        ReadValue(
            m_mem,
            particle + m_off.Particle_RotY,
            pose.rotY) &&
        ReadValue(
            m_mem,
            particle + m_off.Particle_RotZ,
            pose.rotZ) &&
        ReadValue(
            m_mem,
            particle + m_off.Particle_RotW,
            pose.rotW);
    if (!poseRead)
    {
        return false;
    }

    if (pose.posX < -1e7 || pose.posX > 1e7 ||
        pose.posY < -1e7 || pose.posY > 1e7 ||
        pose.posZ < -1e7 || pose.posZ > 1e7)
    {
        return false;
    }

    const double rotationLength = std::sqrt(
        pose.rotX * pose.rotX +
        pose.rotY * pose.rotY +
        pose.rotZ * pose.rotZ +
        pose.rotW * pose.rotW);
    if (rotationLength < 0.95 || rotationLength > 1.05)
    {
        pose.rotX = 0;
        pose.rotY = 0;
        pose.rotZ = 0;
        pose.rotW = 1;
    }

    outPose = pose.ToTransform();
    return true;
}

} // namespace xrd
