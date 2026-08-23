#pragma once

#include "../../core/types.hpp"

namespace xrd
{

struct PhysXOffsets
{
    u32 physics_scenes = 0x08;
    u32 physics_scenesCount = 0x10;

    u32 scene_actors = 0x2618;
    u32 scene_actorsCount = 0x2620;

    u32 actor_type = 0x08;
    u32 actor_userData = 0x10;
    u32 actor_shapesData = 0x28;
    u32 actor_shapesCount = 0x30;
    u32 staticType = 7;
    u32 dynamicType = 6;

    u32 staticPose_flag = 0x68;
    u32 staticPose_flagBit = 0x40;
    u32 staticPose_extPtr = 0x70;
    u32 staticPose_extOffset = 0xB0;
    u32 staticPose_inline = 0x90;

    u32 dynPose_flagAddr = 0x17C;
    u32 dynPose_flagBit = 0x200;
    u32 dynPose_extPtr = 0x70;
    u32 dynPose_extOffset = 0xE0;
    u32 dynPose_b2wInline = 0xB0;
    u32 dynPose_body2actor = 0x140;

    u32 shape_flagByte = 0x38;
    u32 shape_extPtr = 0x40;
    u32 shape_localPose = 0x70;
    u32 shape_geomType = 0x98;
    u32 shape_geomData = 0x9C;

    u32 shapeFlagsInline = 0x90;
    u32 shapeFlags_extOff = 0x34;
    u32 shapeFlags_flagBit = 0x40;
};

} // namespace xrd
