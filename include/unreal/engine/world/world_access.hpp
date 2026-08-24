#pragma once
// Unreal-eXternalrEsolve - Level / Actor 访问
// 从 world.hpp 拆分：PersistentLevel、ULevel::Actors 以及相关便利访问。

#include "world.hpp"

namespace unreal
{

struct ActorArray
{
    uptr data  = 0;
    i32  count = 0;
};

inline bool GetLevelActors(
    const IMemoryAccessor& mem,
    const UEOffsets& offsets,
    uptr level,
    ActorArray& out)
{
    out = {};
    if (!IsCanonicalUserPtr(level) || offsets.ULevel_Actors < 0)
    {
        return false;
    }

    if (!ReadPtr(
            mem,
            level + offsets.ULevel_Actors,
            out.data) ||
        !ReadValue(
            mem,
            level + offsets.ULevel_Actors + 8,
            out.count))
    {
        out = {};
        return false;
    }

    if (!IsCanonicalUserPtr(out.data) ||
        out.count <= 0 ||
        out.count >= 200000)
    {
        out = {};
        return false;
    }
    return true;
}

inline bool GetPersistentLevel(uptr world, uptr& outLevel)
{
    outLevel = 0;
    if (!IsCanonicalUserPtr(world))
    {
        return false;
    }

    auto& off = Ctx().off;
    if (off.UWorld_PersistentLevel != -1)
    {
        GReadPtr(world + off.UWorld_PersistentLevel, outLevel);
    }

    if (IsCanonicalUserPtr(outLevel))
    {
        return true;
    }

    for (i32 levelOff = 0x28; levelOff <= 0x100; levelOff += 8)
    {
        uptr candidate = 0;
        if (!GReadPtr(world + levelOff, candidate))
        {
            continue;
        }
        if (!IsCanonicalUserPtr(candidate))
        {
            continue;
        }

        uptr levelClass = GetObjectClass(candidate);
        if (!levelClass)
        {
            continue;
        }

        if (GetObjectName(levelClass) == "Level")
        {
            outLevel = candidate;
            Ctx().off.UWorld_PersistentLevel = levelOff;
            return true;
        }
    }

    return false;
}

inline bool GetLevelActors(uptr level, ActorArray& out)
{
    out = {};
    if (!IsCanonicalUserPtr(level))
    {
        return false;
    }

    auto& off = Ctx().off;

    if (off.ULevel_Actors != -1)
    {
        return GetLevelActors(Mem(), off, level, out);
    }

    for (i32 actorsOff = 0x80; actorsOff <= 0x150; actorsOff += 8)
    {
        uptr data = 0;
        i32 count = 0;
        if (!GReadPtr(level + actorsOff, data))
        {
            continue;
        }
        if (!IsCanonicalUserPtr(data))
        {
            continue;
        }

        GReadI32(level + actorsOff + 8, count);
        if (count <= 0 || count >= 200000)
        {
            continue;
        }

        uptr firstActor = 0;
        if (GReadPtr(data, firstActor) && IsCanonicalUserPtr(firstActor))
        {
            out.data = data;
            out.count = count;
            Ctx().off.ULevel_Actors = actorsOff;
            return true;
        }
    }

    return false;
}

inline bool GetPersistentLevelActors(ActorArray& out)
{
    out = {};

    uptr world = GetUWorld();
    if (!world)
    {
        return false;
    }

    uptr level = 0;
    if (!GetPersistentLevel(world, level))
    {
        return false;
    }

    return GetLevelActors(level, out);
}

} // namespace unreal
