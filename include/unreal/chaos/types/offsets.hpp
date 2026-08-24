#pragma once

#include "../../core/types.hpp"

namespace unreal
{

struct ChaosOffsets
{
    i32 PrimComp_BodyInstance = -1;
    i32 BodyInstance_BodySetup = -1;
    i32 BodyInstance_PhysicsProxy = -1;
    i32 BodySetup_AggGeom = -1;

    i32 AggGeom_SphereElems = -1;
    i32 AggGeom_BoxElems = -1;
    i32 AggGeom_SphylElems = -1;
    i32 AggGeom_ConvexElems = -1;

    i32 ShapeElem_BaseSize = -1;

    i32 BoxElem_Center = -1;
    i32 BoxElem_Rotation = -1;
    i32 BoxElem_X = -1;
    i32 BoxElem_Y = -1;
    i32 BoxElem_Z = -1;
    i32 BoxElem_Size = -1;

    i32 SphereElem_Center = -1;
    i32 SphereElem_Radius = -1;
    i32 SphereElem_Size = -1;

    i32 SphylElem_Center = -1;
    i32 SphylElem_Rotation = -1;
    i32 SphylElem_Radius = -1;
    i32 SphylElem_Length = -1;
    i32 SphylElem_Size = -1;

    i32 ConvexElem_VertexData = -1;
    i32 ConvexElem_IndexData = -1;
    i32 ConvexElem_Size = -1;

    i32 Proxy_Owner = 0x08;
    i32 Proxy_Particle = 0x20;

    i32 Particle_PosX = 0x08;
    i32 Particle_PosY = 0x10;
    i32 Particle_PosZ = 0x18;
    i32 Particle_RotX = 0x30;
    i32 Particle_RotY = 0x38;
    i32 Particle_RotZ = 0x40;
    i32 Particle_RotW = 0x48;

    i32 UWorld_PhysScene = -1;
    i32 PhysScene_OwnerWorld = 0x10;
    i32 PhysScene_Solver = 0x18;
    i32 Solver_PhysScene = 0x10;

    i32 BodyInstance_StructSize = -1;
};

} // namespace unreal
