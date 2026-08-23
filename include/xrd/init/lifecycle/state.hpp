#pragma once
// Xrd-eXternalrEsolve - AutoInit 生命周期状态

#include "../../core/types.hpp"
#include <atomic>

namespace xrd
{

enum class AutoInitStage : u8
{
    Idle,
    Resetting,
    AttachingProcess,
    ScanningGlobals,
    DiscoveringObjectOffsets,
    ScanningRuntime,
    ValidatingCore,
    DiscoveringWorld,
    ValidatingFinal,
    WaitingForRetry,
    Completing,
    Completed,
    Cancelled,
    Failed
};

namespace detail
{

inline std::atomic<AutoInitStage>& AutoInitStageStorage()
{
    static std::atomic<AutoInitStage> stage{ AutoInitStage::Idle };
    return stage;
}

inline void PublishAutoInitStage(AutoInitStage stage)
{
    AutoInitStageStorage().store(stage, std::memory_order_release);
}

class AutoInitStateMachine
{
public:
    void TransitionTo(AutoInitStage stage)
    {
        currentStage = stage;
        PublishAutoInitStage(stage);
    }

    AutoInitStage CurrentStage() const
    {
        return currentStage;
    }

private:
    AutoInitStage currentStage = AutoInitStage::Idle;
};

} // namespace detail

inline AutoInitStage GetAutoInitStage()
{
    return detail::AutoInitStageStorage().load(std::memory_order_acquire);
}

} // namespace xrd
