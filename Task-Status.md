# Task-Status

## 1. 当前任务/需求/待办清单

- [已完成] 重构 `include/xrd/init/` 初始化生命周期（WinAPI / SharedMem / Driver）
- [已完成] 拆分扫描阶段，所有 `init/` 头文件均不超过 300 行
- [已完成] 明确取消、失败、重试和 `ResetContext()` 回滚边界
- [已完成] 保持原公共入口并新增 `GetAutoInitStage()` 状态查询
- [已完成] 更新 README 生命周期和目录说明
- [已完成] 使用 MinGW C++20 对生命周期公共头执行语法验证

## 2. 已解决问题/已完成需求

- `AutoInit` / `AutoInitSharedMem` 共用同一状态机；`AutoInitDriver` 保持兼容别名
- 删除 445 行、混合多阶段职责的 `init_common.hpp`
- 初始化使用事务回滚：失败或取消时统一释放后端、句柄和发现缓存
- `ctx.inited` 只在最终验证完成后置位；扫描期临时访问仅允许初始化线程
- 每轮重试刷新 PE 段快照、偏移和名称/属性缓存，避免复用失败轮次的旧数据
- 取消回调改为原子存取，且不再直接调用 `ResetContext()`
- 取消检查覆盖重试等待、主要扫描阶段边界和 UEnum 长枚举循环
- 生命周期入口使用 `Context::mtx` 串行化，避免多个 AutoInit 同时重置全局上下文

## 3. 经验/教训/高价值信息

- 项目：Xrd-eXternalrEsolve header-only C++ 库，生命周期核心在 `include/xrd/init/`
- 生命周期控制：`include/xrd/init/lifecycle/controller.hpp`
- 生命周期状态：`include/xrd/init/lifecycle/state.hpp`
- 扫描阶段：`include/xrd/init/phases/`
- 验证命令：`x86_64-w64-mingw32-g++-posix -std=c++20 -fsyntax-only`
- 仓库中的 SharedMem/Driver 访问器被 `.gitignore` 标为私有；验证时使用了临时接口桩，未引入或提交跨项目代码
