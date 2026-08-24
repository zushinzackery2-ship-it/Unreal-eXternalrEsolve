#pragma once
// Unreal-eXternalrEsolve - PE 段缓存阶段

#include "phase_support.hpp"
#include "../../core/context.hpp"
#include "../../core/process_sections.hpp"
#include <iostream>

namespace unreal
{
namespace detail
{

inline bool EnsureAutoInitSectionCache(Context& ctx)
{
    if (!ctx.sections.empty())
    {
        const SectionCache* textSection = FindSection(ctx.sections, ".text");
        const SectionCache* dataSection = FindSection(ctx.sections, ".data");
        if (textSection != nullptr && dataSection != nullptr)
        {
            return true;
        }

        ctx.sections.clear();
    }

    return CacheSections(
        *ctx.mem,
        ctx.mainModule.base,
        ctx.mainModule.size,
        ctx.sections);
}

inline InitStepResult RunSectionCachePhase()
{
    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    Context& ctx = Ctx();
    if (!EnsureAutoInitSectionCache(ctx))
    {
        std::cerr << "[unreal] 缓存 PE 段失败\n";
        return InitStepResult::RetryRequired;
    }

    std::cerr << "[unreal] 缓存了 " << ctx.sections.size() << " 个段: ";
    for (const SectionCache& section : ctx.sections)
    {
        std::cerr << section.name << " ";
    }
    std::cerr << "\n";

    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace unreal
