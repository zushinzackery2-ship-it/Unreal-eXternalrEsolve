#pragma once

#include "aliases.hpp"
#include <utility>
#include <vector>

namespace xrd
{

enum PxGeometryType : u32
{
    eSPHERE = 0,
    ePLANE = 1,
    eCAPSULE = 2,
    eBOX = 3,
    eCONVEXMESH = 4,
    eTRIANGLEMESH = 5,
    eHEIGHTFIELD = 6,
    eGEOM_INVALID = 0xFFFFFFFF
};

constexpr u32 eSIMULATION_SHAPE = (1 << 0);
constexpr u32 eSCENE_QUERY_SHAPE = (1 << 1);
constexpr u32 eTRIGGER_SHAPE = (1 << 2);

union GeomData
{
    BoxGeom box;
    SphereGeom sphere;
    CapsuleGeom capsule;
};

struct PxShapeData
{
    PxGeometryType geomType = eGEOM_INVALID;
    PxTransform localPose;
    GeomData geom{};
    u32 pxFlags = 0;
    std::vector<PxVec3> convexVerts;
    std::vector<std::pair<u8, u8>> convexEdges;
    std::vector<u8> convexTriangles;
};

enum PxActorType : u32
{
    eRIGID_STATIC = 7,
    eRIGID_DYNAMIC = 6
};

struct PxActorData
{
    uptr address = 0;
    PxActorType type = eRIGID_STATIC;
    PxTransform globalPose;
    std::vector<PxShapeData> shapes;
};

struct PxSceneData
{
    std::vector<PxActorData> staticActors;
    std::vector<PxActorData> dynamicActors;
};

} // namespace xrd
