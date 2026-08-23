#pragma once

#include "../../collision/collision_types.hpp"

namespace xrd
{

struct ChaosWorldPose
{
    double posX = 0;
    double posY = 0;
    double posZ = 0;
    double rotX = 0;
    double rotY = 0;
    double rotZ = 0;
    double rotW = 1;

    Transform ToTransform() const
    {
        Transform transform;
        transform.p =
        {
            static_cast<float>(posX),
            static_cast<float>(posY),
            static_cast<float>(posZ)
        };
        transform.q =
        {
            static_cast<float>(rotX),
            static_cast<float>(rotY),
            static_cast<float>(rotZ),
            static_cast<float>(rotW)
        };
        return transform;
    }
};

} // namespace xrd
