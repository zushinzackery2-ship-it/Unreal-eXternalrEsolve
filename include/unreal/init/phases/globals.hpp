#pragma once
// Unreal-eXternalrEsolve - GObjects / GNames 扫描阶段

#include "phase_support.hpp"
#include "../../core/context.hpp"
#include "../../resolve/globals/scan_gnames.hpp"
#include "../../resolve/globals/scan_gobjects.hpp"
#include <iostream>

namespace unreal
{
namespace detail
{

inline InitStepResult RunGlobalPointerScanPhase()
{
    Context& ctx = Ctx();

    bool chunked = false;
    if (!resolve::ScanGObjects(
        ctx.sections,
        *ctx.mem,
        ctx.off.GObjects,
        chunked))
    {
        std::cerr << "[unreal] GObjects 未找到\n";
        return InitStepResult::RetryRequired;
    }
    ctx.off.bIsChunkedObjArray = chunked;

    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    bool isNamePool = false;
    if (!resolve::ScanGNames(
        ctx.sections,
        *ctx.mem,
        ctx.off.GNames,
        isNamePool))
    {
        std::cerr << "[unreal] GNames 未找到\n";
        return InitStepResult::RetryRequired;
    }
    ctx.off.bUseNamePool = isNamePool;

    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace unreal
