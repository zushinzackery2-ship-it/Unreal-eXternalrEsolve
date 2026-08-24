#pragma once
// ============================================================
// Unreal-eXternalrEsolve — Header-Only UE SDK 外部导出工具库 v2
// 单一头文件入口，include 即用
// ============================================================

// 核心层：类型、内存抽象、进程操作、PE 段缓存、全局上下文
#include "unreal/core/types.hpp"
#include "unreal/memory/memory.hpp"
#include "unreal/memory/memory_driver.hpp"
#include "unreal/memory/memory_shmem.hpp"
#include "unreal/core/process.hpp"
#include "unreal/core/process_sections.hpp"
#include "unreal/core/context.hpp"

// 偏移解析层
// globals: GObjects/GNames/GWorld/DebugCanvas
#include "unreal/resolve/globals/scan_gobjects.hpp"
#include "unreal/resolve/globals/scan_gnames.hpp"
#include "unreal/resolve/globals/scan_world.hpp"
#include "unreal/resolve/globals/scan_debug_canvas.hpp"
// uobject: UObject/UStruct/UFunction/UClass 偏移
#include "unreal/resolve/uobject/scan_offsets.hpp"
#include "unreal/resolve/uobject/scan_struct_offsets.hpp"
#include "unreal/resolve/uobject/scan_ufunction_offsets.hpp"
#include "unreal/resolve/uobject/scan_uclass_offsets.hpp"
// property: 属性偏移
#include "unreal/resolve/property/scan_property_offsets.hpp"
#include "unreal/resolve/property/scan_property_base_offsets.hpp"
#include "unreal/resolve/property/scan_property_base_offsets2.hpp"
#include "unreal/resolve/property/scan_property_offsets_extra.hpp"
// runtime: 运行时扫描
#include "unreal/resolve/runtime/scan_process_event.hpp"
#include "unreal/resolve/runtime/scan_append_string.hpp"
#include "unreal/resolve/runtime/scan_bones.hpp"

// 引擎封装层
#include "unreal/engine/names.hpp"
// objects: UObject/UStruct/UClass/FProperty
#include "unreal/engine/objects/objects.hpp"
#include "unreal/engine/objects/objects_search.hpp"
// world: UWorld/Actor/Pawn
#include "unreal/engine/world/world.hpp"
#include "unreal/engine/world/world_access.hpp"
#include "unreal/engine/world/world_levels.hpp"
#include "unreal/engine/world/world_actors.hpp"
// bones: 骨骼读取、批量操作、名称解析
#include "unreal/engine/bones/bones.hpp"
#include "unreal/engine/bones/bones_batch.hpp"
#include "unreal/engine/bones/bones_names.hpp"

// 碰撞通用类型（引擎无关）
#include "unreal/collision/collision_types.hpp"

// PhysX 碰撞模块：类型定义、PE 导出解析、内存读取器
#include "unreal/physx/physx_types.hpp"
#include "unreal/physx/physx_pe.hpp"
#include "unreal/physx/physx_reader.hpp"

// Chaos 碰撞模块：类型定义、内存读取器
#include "unreal/chaos/chaos_types.hpp"
#include "unreal/chaos/chaos_reader.hpp"

// Embree 建模模块：三角形化（header-only，无需 Embree SDK）
#include "unreal/embree/tessellation.hpp"
// 注意：unreal/embree/raycast_scene.hpp 需要链接 embree3，按需 include

// 便利函数层
#include "unreal/helpers/w2s.hpp"
#include "unreal/runtime/channel_pool.hpp"
#include "unreal/runtime/view_state.hpp"
#include "unreal/runtime/scene_watch.hpp"
#include "unreal/runtime/actor_tracker.hpp"
#include "unreal/runtime/bone_runtime.hpp"
// dump: SDK 导出、类型解析、枚举收集
#include "unreal/helpers/dump/dump_type_resolve.hpp"
#include "unreal/helpers/dump/dump_enum.hpp"
#include "unreal/helpers/dump/dump_collect.hpp"
#include "unreal/helpers/dump/dump_sdk.hpp"
#include "unreal/helpers/dump/dump_extra.hpp"

// AutoInit 放最后，因为它依赖上面所有模块
#include "unreal/init/auto_init.hpp"
