#pragma once

#include "data.hpp"

namespace unreal
{
namespace detail
{

inline void ConvertPhysXActors(
    const std::vector<PxActorData>& source,
    std::vector<CollisionActor>& destination,
    bool isDynamic)
{
    destination.reserve(source.size());
    for (const auto& physxActor : source)
    {
        CollisionActor actor;
        actor.address = physxActor.address;
        actor.isDynamic = isDynamic;
        actor.shapes.reserve(physxActor.shapes.size());

        for (const auto& physxShape : physxActor.shapes)
        {
            CollisionShape shape;
            shape.worldPose = TransformMul(
                physxActor.globalPose,
                physxShape.localPose);

            switch (physxShape.geomType)
            {
                case eBOX:
                    shape.geomType = eGEOM_BOX;
                    shape.box = physxShape.geom.box;
                    break;
                case eSPHERE:
                    shape.geomType = eGEOM_SPHERE;
                    shape.sphere = physxShape.geom.sphere;
                    break;
                case eCAPSULE:
                    shape.geomType = eGEOM_CAPSULE;
                    shape.capsule = physxShape.geom.capsule;
                    break;
                case eCONVEXMESH:
                    shape.geomType = eGEOM_CONVEX;
                    shape.convexVerts = physxShape.convexVerts;
                    shape.convexEdges = physxShape.convexEdges;
                    shape.convexTriangles = physxShape.convexTriangles;
                    break;
                default:
                    continue;
            }

            actor.shapes.push_back(std::move(shape));
        }

        if (!actor.shapes.empty())
        {
            destination.push_back(std::move(actor));
        }
    }
}

} // namespace detail

inline CollisionScene ToCollisionScene(const PxSceneData& physxData)
{
    CollisionScene scene;
    detail::ConvertPhysXActors(
        physxData.staticActors,
        scene.staticActors,
        false);
    detail::ConvertPhysXActors(
        physxData.dynamicActors,
        scene.dynamicActors,
        true);
    return scene;
}

} // namespace unreal
