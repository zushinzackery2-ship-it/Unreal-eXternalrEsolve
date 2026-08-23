#pragma once
// Xrd-eXternalrEsolve - 初始化关键值验证

#include "../../core/context.hpp"
#include <iostream>

namespace xrd
{
namespace detail
{

inline bool ValidateAutoInitCriticalValues(bool includeWorldChain)
{
    const Context& ctx = Ctx();
    bool allValid = true;

    auto check = [&](bool valid, const char* name)
    {
        if (!valid)
        {
            std::cerr << "[xrd] 缺少关键值: " << name << "\n";
            allValid = false;
        }
    };

    check(ctx.off.GNames != 0, "GNames");
    check(ctx.off.GObjects != 0, "GObjects");
    check(ctx.off.GWorld != 0, "GWorld");
    check(ctx.off.DebugCanvasObjCacheAddr != 0, "DebugCanvasObject");
    check(ctx.off.ProcessEvent_Addr != 0, "ProcessEvent");
    check(ctx.off.ProcessEvent_VTableIndex >= 0, "ProcessEventIdx");
    check(ctx.off.AppendNameToString != 0, "AppendString");

    if (includeWorldChain)
    {
        check(
            ctx.off.UWorld_PersistentLevel >= 0,
            "UWorld_PersistentLevel");
        check(
            ctx.off.UWorld_OwningGameInstance >= 0,
            "UWorld_OwningGameInstance");
        check(
            ctx.off.UGameInstance_LocalPlayers >= 0,
            "UGameInstance_LocalPlayers");
        check(
            ctx.off.ULocalPlayer_PlayerController >= 0,
            "ULocalPlayer_PlayerController");
        check(ctx.off.ULevel_Actors >= 0, "ULevel_Actors");
        check(
            ctx.off.APlayerController_Pawn >= 0,
            "APlayerController_Pawn");
    }

    return allValid;
}

} // namespace detail
} // namespace xrd
