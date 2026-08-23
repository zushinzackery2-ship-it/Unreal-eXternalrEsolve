#pragma once
// Xrd-eXternalrEsolve - 初始化扫描阶段编排

#include "enum_offsets.hpp"
#include "globals.hpp"
#include "object_offsets.hpp"
#include "runtime_scan.hpp"
#include "sections.hpp"
#include "vector_precision.hpp"
#include "../init_chaos.hpp"
#include "../init_world_chain.hpp"
#include "../lifecycle/state.hpp"

namespace xrd
{
namespace detail
{

inline InitStepResult RunAutoInitCoreScan(
    AutoInitStateMachine& stateMachine)
{
    stateMachine.TransitionTo(AutoInitStage::ScanningGlobals);

    InitStepResult result = RunSectionCachePhase();
    if (result != InitStepResult::Succeeded)
    {
        return result;
    }

    result = RunGlobalPointerScanPhase();
    if (result != InitStepResult::Succeeded)
    {
        return result;
    }

    stateMachine.TransitionTo(AutoInitStage::DiscoveringObjectOffsets);
    result = RunObjectLayoutDiscoveryPhase();
    if (result != InitStepResult::Succeeded)
    {
        return result;
    }

    result = RunEnumAndFieldDiscoveryPhase();
    if (result != InitStepResult::Succeeded)
    {
        return result;
    }

    stateMachine.TransitionTo(AutoInitStage::ScanningRuntime);
    result = RunRuntimeSymbolScanPhase();
    if (result != InitStepResult::Succeeded)
    {
        return result;
    }

    return RunPhysicsBackendDiscoveryPhase();
}

inline InitStepResult RunAutoInitWorldDiscovery(
    AutoInitStateMachine& stateMachine)
{
    stateMachine.TransitionTo(AutoInitStage::DiscoveringWorld);
    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    Context& ctx = Ctx();

    ULONGLONG phaseTick = GetTickCount64();
    DiscoverWorldChainOffsets(ctx);
    LogSlowInitPhase("World 链偏移发现", phaseTick);

    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    InitStepResult result = RunVectorPrecisionDiscoveryPhase();
    if (result != InitStepResult::Succeeded)
    {
        return result;
    }

    if (ctx.off.physicsBackend == UEOffsets::eChaos &&
        ctx.off.GWorld != 0)
    {
        phaseTick = GetTickCount64();
        InitChaosOffsets(ctx);
        LogSlowInitPhase("Chaos 偏移发现", phaseTick);
    }

    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace xrd
