#pragma once

#include "../../collision/collision_types.hpp"

namespace unreal
{

using PxVec3 = Vec3;
using PxQuat = Quat;
using PxTransform = Transform;

inline PxQuat QuatMultiply(const PxQuat& a, const PxQuat& b)
{
    return QuatMul(a, b);
}

inline PxQuat QuatConjugate(const PxQuat& q)
{
    return QuatConj(q);
}

inline PxVec3 QuatRotate(const PxQuat& q, const PxVec3& v)
{
    return QuatRot(q, v);
}

inline PxTransform TransformMultiply(const PxTransform& a, const PxTransform& b)
{
    return TransformMul(a, b);
}

inline PxVec3 TransformPoint(const PxTransform& t, const PxVec3& v)
{
    return TransformPt(t, v);
}

} // namespace unreal
