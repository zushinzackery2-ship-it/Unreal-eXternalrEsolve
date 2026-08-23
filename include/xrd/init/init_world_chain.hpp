#pragma once
// Xrd-eXternalrEsolve - World 链偏移反射发现

#include "../core/context.hpp"
#include "../engine/objects/objects_search.hpp"
#include <format>
#include <iostream>

namespace xrd
{

inline void DiscoverPlayerControllerOffsets(
    Context& ctx,
    uptr localPlayer);

inline void PrintWorldChainOffsets(const UEOffsets& offsets);

inline void DiscoverWorldChainOffsets(Context& ctx)
{
    IMemoryAccessor& memory = *ctx.mem;
    UEOffsets& offsets = ctx.off;

    if (offsets.GWorld == 0)
    {
        return;
    }

    uptr world = 0;
    if (!ReadPtr(memory, offsets.GWorld, world) ||
        !IsCanonicalUserPtr(world))
    {
        return;
    }

    uptr worldClass = GetObjectClass(world);
    if (worldClass != 0)
    {
        if (offsets.UWorld_OwningGameInstance == -1)
        {
            offsets.UWorld_OwningGameInstance = GetPropertyOffsetByName(
                worldClass,
                "OwningGameInstance");
        }
        if (offsets.UWorld_PersistentLevel == -1)
        {
            offsets.UWorld_PersistentLevel = GetPropertyOffsetByName(
                worldClass,
                "PersistentLevel");
        }
        if (offsets.UWorld_Levels == -1)
        {
            offsets.UWorld_Levels = GetPropertyOffsetByName(
                worldClass,
                "Levels");
        }
    }

    if (offsets.UWorld_OwningGameInstance != -1)
    {
        uptr gameInstance = 0;
        if (ReadPtr(
            memory,
            world + offsets.UWorld_OwningGameInstance,
            gameInstance) &&
            IsCanonicalUserPtr(gameInstance))
        {
            uptr gameInstanceClass = GetObjectClass(gameInstance);
            if (gameInstanceClass != 0 &&
                offsets.UGameInstance_LocalPlayers == -1)
            {
                offsets.UGameInstance_LocalPlayers = GetPropertyOffsetByName(
                    gameInstanceClass,
                    "LocalPlayers");
            }

            if (offsets.UGameInstance_LocalPlayers != -1)
            {
                uptr localPlayersData = 0;
                i32 localPlayersCount = 0;
                ReadPtr(
                    memory,
                    gameInstance + offsets.UGameInstance_LocalPlayers,
                    localPlayersData);
                ReadI32(
                    memory,
                    gameInstance + offsets.UGameInstance_LocalPlayers + 8,
                    localPlayersCount);

                if (IsCanonicalUserPtr(localPlayersData) &&
                    localPlayersCount > 0)
                {
                    uptr localPlayer = 0;
                    if (ReadPtr(memory, localPlayersData, localPlayer) &&
                        IsCanonicalUserPtr(localPlayer))
                    {
                        DiscoverPlayerControllerOffsets(
                            ctx,
                            localPlayer);
                    }
                }
            }
        }
    }

    PrintWorldChainOffsets(offsets);
}

inline void DiscoverPlayerControllerOffsets(
    Context& ctx,
    uptr localPlayer)
{
    UEOffsets& offsets = ctx.off;
    IMemoryAccessor& memory = *ctx.mem;

    uptr localPlayerClass = GetObjectClass(localPlayer);
    if (localPlayerClass != 0 &&
        offsets.ULocalPlayer_PlayerController == -1)
    {
        offsets.ULocalPlayer_PlayerController = GetPropertyOffsetByName(
            localPlayerClass,
            "PlayerController");
    }

    if (offsets.ULocalPlayer_PlayerController == -1)
    {
        return;
    }

    uptr playerController = 0;
    if (!ReadPtr(
        memory,
        localPlayer + offsets.ULocalPlayer_PlayerController,
        playerController) ||
        !IsCanonicalUserPtr(playerController))
    {
        return;
    }

    uptr playerControllerClass = GetObjectClass(playerController);
    if (playerControllerClass == 0)
    {
        return;
    }

    if (offsets.APlayerController_Pawn == -1)
    {
        i32 pawnOffset = GetPropertyOffsetByName(
            playerControllerClass,
            "Pawn");
        if (pawnOffset == -1)
        {
            pawnOffset = GetPropertyOffsetByName(
                playerControllerClass,
                "AcknowledgedPawn");
        }
        offsets.APlayerController_Pawn = pawnOffset;
    }

    if (offsets.APlayerController_PlayerCameraManager == -1)
    {
        offsets.APlayerController_PlayerCameraManager =
            GetPropertyOffsetByName(
                playerControllerClass,
                "PlayerCameraManager");
    }
}

inline void PrintWorldChainOffsets(const UEOffsets& offsets)
{
    std::cerr << "[xrd] World链偏移:\n";
    std::cerr << std::format(
        "  OwningGameInstance:  +0x{:X}\n",
        offsets.UWorld_OwningGameInstance);
    std::cerr << std::format(
        "  PersistentLevel:     +0x{:X}\n",
        offsets.UWorld_PersistentLevel);
    std::cerr << std::format(
        "  Levels:              +0x{:X}\n",
        offsets.UWorld_Levels);
    std::cerr << std::format(
        "  LocalPlayers:        +0x{:X}\n",
        offsets.UGameInstance_LocalPlayers);
    std::cerr << std::format(
        "  PlayerController:    +0x{:X}\n",
        offsets.ULocalPlayer_PlayerController);
    std::cerr << std::format(
        "  Pawn:                +0x{:X}\n",
        offsets.APlayerController_Pawn);
    std::cerr << std::format(
        "  PlayerCameraManager: +0x{:X}\n",
        offsets.APlayerController_PlayerCameraManager);
}

} // namespace xrd
