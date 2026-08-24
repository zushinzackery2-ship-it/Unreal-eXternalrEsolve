#pragma once
// Unreal-eXternalrEsolve - AutoInit 完成摘要

#include "../../core/context.hpp"
#include "../../resolve/uobject/scan_offsets.hpp"
#include <format>
#include <iostream>

namespace unreal
{
namespace detail
{

inline void PrintAutoInitSummary()
{
    Context& ctx = Ctx();
    uptr moduleBase = ctx.mainModule.base;

    std::cerr << "========================================\n";
    std::cerr << "Important Static Offsets:\n";
    std::cerr << "========================================\n";
    std::cerr << std::format(
        "GObjects:           0x{:08X}\n",
        ctx.off.GObjects ? ctx.off.GObjects - moduleBase : 0);
    std::cerr << std::format(
        "GNames:             0x{:08X}\n",
        ctx.off.GNames ? ctx.off.GNames - moduleBase : 0);
    std::cerr << std::format(
        "GWorld:             0x{:08X}\n",
        ctx.off.GWorld ? ctx.off.GWorld - moduleBase : 0);
    std::cerr << std::format(
        "ProcessEvent:       0x{:08X}\n",
        ctx.off.ProcessEvent_Addr);
    std::cerr << std::format(
        "ProcessEventIdx:    {}\n",
        ctx.off.ProcessEvent_VTableIndex);
    std::cerr << std::format(
        "AppendString:       0x{:08X}\n",
        ctx.off.AppendNameToString);
    std::cerr << "========================================\n";
    std::cerr << "ChunkSize:          " << ctx.off.ChunkSize << "\n";
    std::cerr << "FUObjectItemSize:   " << ctx.off.FUObjectItemSize << "\n";
    std::cerr << "========================================\n";
    std::cerr << "[unreal] 精度: "
              << (ctx.off.bUseDoublePrecision ? "double (UE5)" : "float (UE4)")
              << "\n";
    std::cerr << "[unreal] 对象数: "
              << resolve::GetObjectCount(*ctx.mem, ctx.off) << "\n";
}

} // namespace detail
} // namespace unreal
