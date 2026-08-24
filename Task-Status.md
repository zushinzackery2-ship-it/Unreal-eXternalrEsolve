# Task-Status

## 1. 当前任务/需求/待办清单

- [已完成] 全库品牌从 `Xrd-eXternalrEsolve` 统一改为 `Unreal-eXternalrEsolve`
- [已完成] 头文件目录 `include/xrd/` 重命名为 `include/unreal/`，主入口 `xrd.hpp` → `unreal.hpp`
- [已完成] C++ 命名空间 `xrd` 整词替换为 `unreal`，`#include` 路径同步更新
- [已完成] 日志前缀 `[xrd]` → `[unreal]`、SDK 导出署名与 LICENSE 版权行同步替换
- [已完成] `.gitignore` 中的私有访问器路径与注释同步为 `unreal`
- [已完成] 以 Unreal-eXternalrEsolve 品牌重写 README.md
- [已完成] 语法自检并确认无 `xrd` 残留
- [已完成] 以 Vernal 身份提交并直接推送到 `main`

## 2. 已解决问题/已完成需求

- 目录重命名遇到 overlay 文件系统 `Invalid cross-device link`，改用 `cp -a` + `rm -rf` + `git add -A` 完成，git 正确识别为 rename
- 品牌替换采用整词边界（`\bxrd\b`），已确认 `xrd` 不作为更长标识符子串出现，替换安全
- README 依据实际 `include/` 目录树重写，补充项目简介、runtime 缓存组件说明与 Allman 代码风格说明
- 全库大小写不敏感扫描 `xrd` 为零匹配

## 3. 经验/教训/高价值信息

- 项目保持 header-only；命名空间与目录名统一为 `unreal`，主入口 `#include <unreal.hpp>`
- 品牌串三种形态：完整品牌 `Unreal-eXternalrEsolve`、命名空间/路径 `unreal`、日志前缀 `[unreal]`
- 私有访问器 `memory_driver.hpp` / `memory_shmem.hpp` 被 `.gitignore` 标为私有，不入库
- 验证命令：`g++ -std=c++20 -fsyntax-only`（Linux 环境仅做整体语法自检，Windows 相关 API 无法完全编译属预期）
- 提交者身份：Vernal <zushinzackery2@gmail.com>；本次直接推送 `main`，未创建 PR
