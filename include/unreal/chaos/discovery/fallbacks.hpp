#pragma once

#include "../../core/context.hpp"

namespace unreal
{
namespace detail
{

inline void ApplyChaosSdkFallbacks(Context& ctx)
{
    auto& offsets = ctx.chaosOff;
    const bool useDoublePrecision = ctx.off.bUseDoublePrecision;

    if (offsets.BodyInstance_BodySetup < 0)
    {
        offsets.BodyInstance_BodySetup = 0x08;
    }
    if (offsets.AggGeom_SphereElems < 0)
    {
        offsets.AggGeom_SphereElems = 0x00;
    }
    if (offsets.AggGeom_BoxElems < 0)
    {
        offsets.AggGeom_BoxElems = 0x10;
    }
    if (offsets.AggGeom_SphylElems < 0)
    {
        offsets.AggGeom_SphylElems = 0x20;
    }
    if (offsets.AggGeom_ConvexElems < 0)
    {
        offsets.AggGeom_ConvexElems = 0x30;
    }
    if (offsets.ShapeElem_BaseSize < 0)
    {
        offsets.ShapeElem_BaseSize = 0x30;
    }

    if (offsets.SphereElem_Center < 0)
    {
        offsets.SphereElem_Center = 0x30;
    }
    if (offsets.SphereElem_Radius < 0)
    {
        offsets.SphereElem_Radius =
            useDoublePrecision ? 0x48 : 0x3C;
    }
    if (offsets.SphereElem_Size < 0)
    {
        offsets.SphereElem_Size =
            useDoublePrecision ? 0x50 : 0x40;
    }

    if (offsets.BoxElem_Center < 0)
    {
        offsets.BoxElem_Center = 0x30;
    }
    if (offsets.BoxElem_Rotation < 0)
    {
        offsets.BoxElem_Rotation =
            useDoublePrecision ? 0x48 : 0x3C;
    }
    if (offsets.BoxElem_X < 0)
    {
        offsets.BoxElem_X = useDoublePrecision ? 0x60 : 0x48;
    }
    if (offsets.BoxElem_Y < 0)
    {
        offsets.BoxElem_Y = useDoublePrecision ? 0x64 : 0x4C;
    }
    if (offsets.BoxElem_Z < 0)
    {
        offsets.BoxElem_Z = useDoublePrecision ? 0x68 : 0x50;
    }
    if (offsets.BoxElem_Size < 0)
    {
        offsets.BoxElem_Size = useDoublePrecision ? 0x70 : 0x58;
    }

    if (offsets.SphylElem_Center < 0)
    {
        offsets.SphylElem_Center = 0x30;
    }
    if (offsets.SphylElem_Rotation < 0)
    {
        offsets.SphylElem_Rotation =
            useDoublePrecision ? 0x48 : 0x3C;
    }
    if (offsets.SphylElem_Radius < 0)
    {
        offsets.SphylElem_Radius =
            useDoublePrecision ? 0x60 : 0x48;
    }
    if (offsets.SphylElem_Length < 0)
    {
        offsets.SphylElem_Length =
            useDoublePrecision ? 0x64 : 0x4C;
    }
    if (offsets.SphylElem_Size < 0)
    {
        offsets.SphylElem_Size =
            useDoublePrecision ? 0x68 : 0x50;
    }

    if (offsets.ConvexElem_VertexData < 0)
    {
        offsets.ConvexElem_VertexData = 0x30;
    }
    if (offsets.ConvexElem_IndexData < 0)
    {
        offsets.ConvexElem_IndexData = 0x40;
    }
    if (offsets.ConvexElem_Size < 0)
    {
        offsets.ConvexElem_Size =
            useDoublePrecision ? 0x100 : 0xB0;
    }
}

} // namespace detail
} // namespace unreal
