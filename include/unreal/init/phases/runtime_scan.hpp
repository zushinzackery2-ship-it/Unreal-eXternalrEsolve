#pragma once
// Unreal-eXternalrEsolve - 运行时符号与物理后端扫描阶段

#include "phase_support.hpp"
#include "../../core/context.hpp"
#include "../../physx/physx_pe.hpp"
#include "../../resolve/globals/scan_debug_canvas.hpp"
#include "../../resolve/globals/scan_world.hpp"
#include "../../resolve/runtime/scan_append_string.hpp"
#include "../../resolve/runtime/scan_process_event.hpp"
#include <format>
#include <iostream>

namespace unreal
{
namespace detail
{

inline InitStepResult RunRuntimeSymbolScanPhase()
{
    Context& ctx = Ctx();

    ULONGLONG phaseTick = GetTickCount64();
    resolve::ScanGWorld(ctx.sections, *ctx.mem, ctx.off, ctx.off.GWorld);
    LogSlowInitPhase("GWorld 扫描", phaseTick);

    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    phaseTick = GetTickCount64();
    resolve::ScanProcessEvent(ctx.sections, *ctx.mem, ctx.off);
    LogSlowInitPhase("ProcessEvent 扫描", phaseTick);

    if (AutoInitCancellationPoint() == InitStepResult::Cancelled)
    {
        return InitStepResult::Cancelled;
    }

    phaseTick = GetTickCount64();
    resolve::ScanAppendString(ctx.sections, *ctx.mem, ctx.off);
    LogSlowInitPhase("AppendString 扫描", phaseTick);

    if (ctx.off.DebugCanvasObjCacheAddr == 0)
    {
        uptr debugCanvasAddress = 0;
        if (resolve::ScanDebugCanvasObject(
            ctx.sections,
            *ctx.mem,
            ctx.off,
            debugCanvasAddress))
        {
            ctx.off.DebugCanvasObjCacheAddr = debugCanvasAddress;
            std::cerr << std::format(
                "[unreal] DebugCanvasObject 找到: 0x{:X} (RVA=0x{:X})\n",
                debugCanvasAddress,
                debugCanvasAddress - ctx.mainModule.base);
        }
        else
        {
            std::cerr << "[unreal] DebugCanvasObject 未找到（ViewProj 链路不可用）\n";
        }
    }

    return AutoInitCancellationPoint();
}

inline InitStepResult RunPhysicsBackendDiscoveryPhase()
{
    Context& ctx = Ctx();
    if (ctx.off.physicsBackend != UEOffsets::ePhysicsUnknown)
    {
        return AutoInitCancellationPoint();
    }

    uptr physXBase = 0;
    uptr physXGlobal = 0;
    if (ResolvePhysXGlobalInstance(
        *ctx.mem,
        ctx.pid,
        physXBase,
        physXGlobal))
    {
        ctx.off.physicsBackend = UEOffsets::ePhysX;
        ctx.off.PhysXDllBase = physXBase;
        ctx.off.PhysXGlobalPtr = physXGlobal;
        std::cerr << "[unreal] 物理后端: PhysX\n";
        std::cerr << std::format(
            "[unreal] PhysX DLL base: 0x{:X}\n",
            physXBase);
        std::cerr << std::format(
            "[unreal] PhysX NpPhysics*: 0x{:X}\n",
            physXGlobal);
    }
    else
    {
        ctx.off.physicsBackend = UEOffsets::eChaos;
        std::cerr << "[unreal] 物理后端: Chaos（PhysX DLL 未找到）\n";
    }

    return AutoInitCancellationPoint();
}

} // namespace detail
} // namespace unreal
