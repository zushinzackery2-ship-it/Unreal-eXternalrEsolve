#pragma once
// Xrd-eXternalrEsolve - AutoInit 进程与内存后端附加

#include "types.hpp"
#include "../../core/context.hpp"
#include "../../memory/memory_shmem.hpp"
#include <iostream>
#include <limits>
#include <memory>

namespace xrd
{
namespace detail
{

inline u32 FindAutoInitTargetProcess(const wchar_t* processName)
{
    u32 pid = 0;

    if (processName != nullptr && processName[0] != L'\0')
    {
        pid = FindProcessId(processName);
    }
    else
    {
        pid = FindProcessByWindowClass(L"UnrealWindow");
        if (pid == 0)
        {
            pid = FindUnrealProcessId();
        }
    }

    if (pid == 0)
    {
        std::cerr << "[xrd] 进程未找到\n";
    }
    else
    {
        std::cerr << "[xrd] PID: " << pid << "\n";
    }

    return pid;
}

inline void PrintAttachedModule(const ModuleInfo& module)
{
    std::cerr << "[xrd] 模块: ";
    for (wchar_t character : module.name)
    {
        std::cerr << static_cast<char>(character);
    }
    std::cerr << " 基址: 0x" << std::hex << module.base
              << " 大小: 0x" << module.size << std::dec << "\n";
}

inline bool AttachWinApiProcess(Context& ctx)
{
    ctx.process = OpenProcessForRead(ctx.pid);
    if (ctx.process == nullptr)
    {
        std::cerr << "[xrd] 打开进程失败（尝试以管理员运行）\n";
        return false;
    }

    ctx.mem = std::make_unique<WinApiMemoryAccessor>(ctx.process);
    if (!GetMainModule(ctx.pid, ctx.mainModule))
    {
        std::cerr << "[xrd] 获取主模块失败\n";
        return false;
    }

    PrintAttachedModule(ctx.mainModule);
    return true;
}

inline bool AttachSharedMemoryProcess(
    Context& ctx,
    const wchar_t* processName)
{
    auto memory = std::make_unique<SharedMemoryAccessor>();
    if (!memory->Open(ctx.pid))
    {
        std::cerr << "[xrd] 共享内存初始化失败（确认 ReiVM 驱动已加载）\n";
        return false;
    }

    u64 moduleSize = 0;
    u64 moduleBase = memory->GetMainModule(&moduleSize);
    if (moduleBase == 0)
    {
        std::cerr << "[xrd] 通过驱动获取主模块失败\n";
        return false;
    }

    ctx.mem = std::move(memory);
    ctx.mainModule.base = static_cast<uptr>(moduleBase);
    constexpr u64 kMaximumModuleSize =
        static_cast<u64>(std::numeric_limits<u32>::max());
    ctx.mainModule.size = moduleSize > kMaximumModuleSize
        ? std::numeric_limits<u32>::max()
        : static_cast<u32>(moduleSize);

    if (processName != nullptr && processName[0] != L'\0')
    {
        ctx.mainModule.name = processName;
    }

    PrintAttachedModule(ctx.mainModule);
    return true;
}

inline bool AttachAutoInitTarget(const AutoInitConfiguration& configuration)
{
    Context& ctx = Ctx();
    ctx.pid = FindAutoInitTargetProcess(configuration.processName);
    if (ctx.pid == 0)
    {
        return false;
    }

    if (configuration.mode == AutoInitMode::SharedMemory)
    {
        return AttachSharedMemoryProcess(ctx, configuration.processName);
    }

    return AttachWinApiProcess(ctx);
}

} // namespace detail
} // namespace xrd
