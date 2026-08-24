#pragma once
// Unreal-eXternalrEsolve - AutoInit 上下文与扫描缓存回滚

#include "../../core/context.hpp"
#include "../../engine/names.hpp"
#include "../../engine/objects/objects.hpp"
#include "../../engine/objects/objects_search.hpp"

namespace unreal
{
namespace detail
{

inline void ClearAutoInitDiscoveryCaches()
{
    ClearResolvedNameCache();
    ClearNameCaches();
    ClearPropertyOffsetCache();
}

inline void ResetAutoInitContext()
{
    ResetContext();
    ClearAutoInitDiscoveryCaches();
}

inline void PrepareAutoInitRetry()
{
    Context& ctx = Ctx();
    ctx.sections.clear();
    ctx.off = UEOffsets{};
    ctx.chaosOff = ChaosOffsets{};
    ctx.inited = false;
    ctx.initializing = true;
    ctx.initializationThreadId = GetCurrentThreadId();
    ClearAutoInitDiscoveryCaches();
}

class AutoInitTransaction
{
public:
    AutoInitTransaction() = default;

    AutoInitTransaction(const AutoInitTransaction&) = delete;
    AutoInitTransaction& operator=(const AutoInitTransaction&) = delete;

    ~AutoInitTransaction()
    {
        if (!committed)
        {
            ResetAutoInitContext();
        }
    }

    void Commit()
    {
        committed = true;
    }

private:
    bool committed = false;
};

} // namespace detail
} // namespace unreal
