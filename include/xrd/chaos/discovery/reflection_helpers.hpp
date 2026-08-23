#pragma once

#include "../../core/context.hpp"
#include "../../engine/objects/objects_search.hpp"
#include <initializer_list>

namespace xrd
{
namespace detail
{

inline uptr FindScriptStructByNames(
    std::initializer_list<const char*> names)
{
    uptr result = 0;
    ForEachObject(
        [&](uptr object, i32)
        {
            if (GetObjectClassName(object) != "ScriptStruct")
            {
                return true;
            }

            const std::string objectName = GetObjectName(object);
            for (const char* candidate : names)
            {
                if (candidate != nullptr && objectName == candidate)
                {
                    result = object;
                    return false;
                }
            }
            return true;
        });
    return result;
}

inline void ReflectClassOffset(
    const char* className,
    const char* propertyName,
    i32& outOffset,
    const char* logName)
{
    const uptr objectClass =
        FindClassByName(className != nullptr ? className : "");
    if (objectClass == 0)
    {
        return;
    }

    const i32 offset = GetPropertyOffsetByName(
        objectClass,
        propertyName != nullptr ? propertyName : "");
    if (offset < 0)
    {
        return;
    }

    outOffset = offset;
    std::cerr << "[xrd][Chaos] " << logName
              << " = +0x" << std::hex << offset << std::dec
              << " (reflect)\n";
}

inline void ReflectStructOffset(
    std::initializer_list<const char*> structNames,
    const char* propertyName,
    i32& outOffset,
    const char* logName)
{
    const uptr scriptStruct = FindScriptStructByNames(structNames);
    if (scriptStruct == 0)
    {
        return;
    }

    const i32 offset = GetPropertyOffsetByName(
        scriptStruct,
        propertyName != nullptr ? propertyName : "");
    if (offset < 0)
    {
        return;
    }

    outOffset = offset;
    std::cerr << "[xrd][Chaos] " << logName
              << " = +0x" << std::hex << offset << std::dec
              << " (reflect)\n";
}

inline void ReflectStructSize(
    std::initializer_list<const char*> structNames,
    i32& outSize,
    const char* logName)
{
    if (Off().UStruct_Size < 0)
    {
        return;
    }

    const uptr scriptStruct = FindScriptStructByNames(structNames);
    if (scriptStruct == 0)
    {
        return;
    }

    i32 size = 0;
    if (!GReadValue(scriptStruct + Off().UStruct_Size, size) ||
        size <= 0)
    {
        return;
    }

    outSize = size;
    std::cerr << "[xrd][Chaos] " << logName
              << " = 0x" << std::hex << size << std::dec
              << " (reflect)\n";
}

} // namespace detail
} // namespace xrd
