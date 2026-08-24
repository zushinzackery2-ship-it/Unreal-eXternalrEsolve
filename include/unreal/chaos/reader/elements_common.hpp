#pragma once

#include "reader.hpp"

namespace unreal
{

inline bool ChaosReader::ReadElementArray(
    uptr aggregateGeometryAddress,
    i32 arrayOffset,
    i32 maximumCount,
    uptr& outData,
    i32& outCount) const
{
    outData = 0;
    outCount = 0;
    if (arrayOffset < 0 ||
        !ReadPtr(
            m_mem,
            aggregateGeometryAddress + arrayOffset,
            outData) ||
        !ReadValue(
            m_mem,
            aggregateGeometryAddress + arrayOffset + 8,
            outCount))
    {
        return false;
    }

    return IsCanonicalUserPtr(outData) &&
           outCount > 0 &&
           outCount <= maximumCount;
}

inline Quat ChaosReader::EulerToQuat(
    double pitchDegrees,
    double yawDegrees,
    double rollDegrees)
{
    constexpr double kDegreesToRadians =
        3.14159265358979323846 / 180.0;
    const double pitch =
        pitchDegrees * kDegreesToRadians * 0.5;
    const double yaw =
        yawDegrees * kDegreesToRadians * 0.5;
    const double roll =
        rollDegrees * kDegreesToRadians * 0.5;

    const double cosinePitch = std::cos(pitch);
    const double sinePitch = std::sin(pitch);
    const double cosineYaw = std::cos(yaw);
    const double sineYaw = std::sin(yaw);
    const double cosineRoll = std::cos(roll);
    const double sineRoll = std::sin(roll);

    Quat quaternion;
    quaternion.x = static_cast<float>(
        cosineRoll * sinePitch * sineYaw -
        sineRoll * cosinePitch * cosineYaw);
    quaternion.y = static_cast<float>(
        -cosineRoll * sinePitch * cosineYaw -
        sineRoll * cosinePitch * sineYaw);
    quaternion.z = static_cast<float>(
        cosineRoll * cosinePitch * sineYaw -
        sineRoll * sinePitch * cosineYaw);
    quaternion.w = static_cast<float>(
        cosineRoll * cosinePitch * cosineYaw +
        sineRoll * sinePitch * sineYaw);
    return quaternion;
}

} // namespace unreal
