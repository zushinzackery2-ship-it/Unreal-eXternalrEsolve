#pragma once
// Xrd-eXternalrEsolve - UObject / Property / Function / Class 偏移阶段

#include "phase_support.hpp"
#include "../../core/context.hpp"
#include "../../resolve/globals/scan_gnames.hpp"
#include "../../resolve/property/scan_property_base_offsets.hpp"
#include "../../resolve/property/scan_property_base_offsets2.hpp"
#include "../../resolve/property/scan_property_offsets.hpp"
#include "../../resolve/property/scan_property_offsets_extra.hpp"
#include "../../resolve/uobject/scan_offsets.hpp"
#include "../../resolve/uobject/scan_struct_offsets.hpp"
#include "../../resolve/uobject/scan_uclass_offsets.hpp"
#include "../../resolve/uobject/scan_ufunction_offsets.hpp"
#include <iostream>

namespace xrd
{
namespace detail
{

inline InitStepResult RunObjectLayoutDiscoveryPhase()
{
    Context& ctx = Ctx();

    if (!resolve::DiscoverUObjectOffsets(*ctx.mem, ctx.off))
    {
        std::cerr << "[xrd] UObject 偏移发现失败\n";
        return InitStepResult::RetryRequired;
    }

    if (ctx.off.bUseNamePool)
    {
        resolve::DetectFNamePoolBlockBits(*ctx.mem, ctx.off);
    }

    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    resolve::DiscoverStructOffsets(*ctx.mem, ctx.off);
    resolve::DiscoverPropertyBaseOffsets(*ctx.mem, ctx.off);
    resolve::DiscoverAllPropertyOffsets(*ctx.mem, ctx.off);

    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    ULONGLONG phaseTick = GetTickCount64();
    resolve::DiscoverFunctionFlagsOffset(*ctx.mem, ctx.off);
    resolve::DiscoverExecFunctionOffset(*ctx.mem, ctx.off);
    resolve::DiscoverCastFlagsOffset(*ctx.mem, ctx.off);
    resolve::DiscoverClassDefaultObjectOffset(*ctx.mem, ctx.off);
    LogSlowInitPhase("函数与类偏移发现", phaseTick);

    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace xrd
