#pragma once

#include "fallbacks.hpp"
#include "reflection_helpers.hpp"

namespace xrd
{
namespace detail
{

inline void DiscoverChaosSdkOffsets(Context& ctx)
{
    auto& offsets = ctx.chaosOff;

    if (offsets.PrimComp_BodyInstance < 0)
    {
        ReflectClassOffset(
            "PrimitiveComponent",
            "BodyInstance",
            offsets.PrimComp_BodyInstance,
            "PrimitiveComponent::BodyInstance");
    }
    if (offsets.BodySetup_AggGeom < 0)
    {
        ReflectClassOffset(
            "BodySetup",
            "AggGeom",
            offsets.BodySetup_AggGeom,
            "BodySetup::AggGeom");
    }
    if (offsets.BodyInstance_StructSize < 0)
    {
        ReflectStructSize(
            { "BodyInstance" },
            offsets.BodyInstance_StructSize,
            "FBodyInstance size");
    }

    i32 reflectedBodySetupOffset = -1;
    ReflectStructOffset(
        { "BodyInstance" },
        "BodySetup",
        reflectedBodySetupOffset,
        "FBodyInstance::BodySetup");
    if (reflectedBodySetupOffset >= 0)
    {
        offsets.BodyInstance_BodySetup = reflectedBodySetupOffset;
    }

    ReflectStructOffset(
        { "KAggregateGeom" },
        "SphereElems",
        offsets.AggGeom_SphereElems,
        "FKAggregateGeom::SphereElems");
    ReflectStructOffset(
        { "KAggregateGeom" },
        "BoxElems",
        offsets.AggGeom_BoxElems,
        "FKAggregateGeom::BoxElems");
    ReflectStructOffset(
        { "KAggregateGeom" },
        "SphylElems",
        offsets.AggGeom_SphylElems,
        "FKAggregateGeom::SphylElems");
    ReflectStructOffset(
        { "KAggregateGeom" },
        "ConvexElems",
        offsets.AggGeom_ConvexElems,
        "FKAggregateGeom::ConvexElems");

    ReflectStructSize(
        { "KShapeElem" },
        offsets.ShapeElem_BaseSize,
        "FKShapeElem size");

    ReflectStructOffset(
        { "KSphereElem" },
        "Center",
        offsets.SphereElem_Center,
        "FKSphereElem::Center");
    ReflectStructOffset(
        { "KSphereElem" },
        "Radius",
        offsets.SphereElem_Radius,
        "FKSphereElem::Radius");
    ReflectStructSize(
        { "KSphereElem" },
        offsets.SphereElem_Size,
        "FKSphereElem size");

    ReflectStructOffset(
        { "KBoxElem" },
        "Center",
        offsets.BoxElem_Center,
        "FKBoxElem::Center");
    ReflectStructOffset(
        { "KBoxElem" },
        "Rotation",
        offsets.BoxElem_Rotation,
        "FKBoxElem::Rotation");
    ReflectStructOffset(
        { "KBoxElem" },
        "X",
        offsets.BoxElem_X,
        "FKBoxElem::X");
    ReflectStructOffset(
        { "KBoxElem" },
        "Y",
        offsets.BoxElem_Y,
        "FKBoxElem::Y");
    ReflectStructOffset(
        { "KBoxElem" },
        "Z",
        offsets.BoxElem_Z,
        "FKBoxElem::Z");
    ReflectStructSize(
        { "KBoxElem" },
        offsets.BoxElem_Size,
        "FKBoxElem size");

    ReflectStructOffset(
        { "KSphylElem" },
        "Center",
        offsets.SphylElem_Center,
        "FKSphylElem::Center");
    ReflectStructOffset(
        { "KSphylElem" },
        "Rotation",
        offsets.SphylElem_Rotation,
        "FKSphylElem::Rotation");
    ReflectStructOffset(
        { "KSphylElem" },
        "Radius",
        offsets.SphylElem_Radius,
        "FKSphylElem::Radius");
    ReflectStructOffset(
        { "KSphylElem" },
        "Length",
        offsets.SphylElem_Length,
        "FKSphylElem::Length");
    ReflectStructSize(
        { "KSphylElem" },
        offsets.SphylElem_Size,
        "FKSphylElem size");

    ReflectStructOffset(
        { "KConvexElem" },
        "VertexData",
        offsets.ConvexElem_VertexData,
        "FKConvexElem::VertexData");
    ReflectStructOffset(
        { "KConvexElem" },
        "IndexData",
        offsets.ConvexElem_IndexData,
        "FKConvexElem::IndexData");
    ReflectStructSize(
        { "KConvexElem" },
        offsets.ConvexElem_Size,
        "FKConvexElem size");

    ApplyChaosSdkFallbacks(ctx);
}

} // namespace detail
} // namespace xrd
