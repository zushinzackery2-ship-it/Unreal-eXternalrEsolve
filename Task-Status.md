# Task-Status

## 1. 当前任务/需求/待办清单

- [已完成] 将 PhysX 类型、发现、拓扑和 Reader 拆分到独立子目录
- [已完成] 将 Chaos 类型、发现、初始化和 Reader 拆分到独立子目录
- [已完成] 将 `init_chaos.hpp` / `init_chaos_scan.hpp` 收敛为兼容入口
- [已完成] `ChaosReader` 复用 `world_levels` 的 Level / Actor 枚举
- [已完成] 保留 PhysX / Chaos 顶层头和公共 API
- [已完成] 所有本次新增或重构的物理模块头文件均不超过 300 行
- [已完成] 使用 MinGW C++20 对物理模块聚合入口及 9 个兼容头逐一执行语法验证
- [已完成] 以 Vernal 身份提交并推送 `cursor/refactor-physx-chaos-3800`
- [阻塞] 创建到 `main` 的 draft PR：当前运行令牌对 Pull Request API 返回 HTTP 403

## 2. 已解决问题/已完成需求

- `physx_reader.hpp` 从 450 行降为兼容 re-export，读取流程按 scene、pose、shape、convex 拆分
- `physx_types.hpp`、`physx_pe.hpp`、`physx_convex.hpp` 均改为薄兼容入口
- `chaos_reader.hpp` 从 656 行降为兼容 re-export，读取流程按 body、pose、primitive、convex 拆分
- `chaos_reflection.hpp` 从 305 行降为薄兼容入口，反射辅助、fallback 和发现流程解耦
- Chaos 初始化实现迁入 `chaos/discovery/`，`init/` 不再持有物理模块实现
- 删除 `ChaosReader` 内重复的 `ReadLoadedLevels` / `ReadLevelActors` / 去重逻辑
- `world_levels` 新增显式 `IMemoryAccessor` / `UEOffsets` 重载，保持 Reader 注入式访问器语义
- PhysX Convex 拓扑读取改用 `memcpy` 解析非对齐字段，避免未对齐指针解引用

## 3. 经验/教训/高价值信息

- 项目保持 header-only；兼容入口只 include 子模块，不引入编译单元
- PhysX 子模块：`types/`、`discovery/`、`topology/`、`reader/`
- Chaos 子模块：`types/`、`discovery/`、`reader/`
- `ChaosReader` 的 Actor 枚举入口为显式访问器版本 `GetAllActors(mem, offsets, world, out)`
- 验证命令：`x86_64-w64-mingw32-g++-posix -std=c++20 -fsyntax-only`
- 物理模块兼容头聚合验证和逐头独立验证均通过；临时大小写兼容头与私有访问器桩未提交
- 整体 `xrd.hpp` 的附加验证仍会命中既有 dump 代码的 MinGW `std::ofstream(std::wstring)` 不兼容，与本次物理模块改动无关
- 仓库中的 SharedMem/Driver 访问器被 `.gitignore` 标为私有；验证时使用了临时接口桩，未引入或提交跨项目代码
- Draft PR 创建入口：`https://github.com/zushinzackery2-ship-it/Xrd-eXternalrEsolve/pull/new/cursor/refactor-physx-chaos-3800`
