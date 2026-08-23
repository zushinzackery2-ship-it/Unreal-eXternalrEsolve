#pragma once
// Xrd-eXternalrEsolve - AutoInit 事务式生命周期状态机

#include "attach.hpp"
#include "diagnostics.hpp"
#include "reset.hpp"
#include "state.hpp"
#include "types.hpp"
#include "../init_cancel.hpp"
#include "../phases/pipeline.hpp"
#include "../phases/validation.hpp"
#include <iostream>
#include <mutex>

namespace xrd
{
namespace detail
{

class AutoInitController
{
public:
    explicit AutoInitController(AutoInitConfiguration configuration)
        : configuration(configuration)
    {
    }

    bool Run()
    {
        Context& ctx = Ctx();
        std::unique_lock<std::mutex> lifecycleLock(ctx.mtx);
        AutoInitTransaction transaction;

        stateMachine.TransitionTo(AutoInitStage::Resetting);
        while (true)
        {
            switch (stateMachine.CurrentStage())
            {
            case AutoInitStage::Resetting:
                ResetAutoInitContext();
                PrintStartBanner();
                if (IsAutoInitCancellationRequested())
                {
                    return FinishCancelled();
                }
                stateMachine.TransitionTo(AutoInitStage::AttachingProcess);
                break;

            case AutoInitStage::AttachingProcess:
                if (!AttachAutoInitTarget(configuration))
                {
                    return FinishFailed();
                }
                ctx.initializing = true;
                ctx.initializationThreadId = GetCurrentThreadId();
                if (IsAutoInitCancellationRequested())
                {
                    return FinishCancelled();
                }
                stateMachine.TransitionTo(AutoInitStage::ScanningGlobals);
                break;

            case AutoInitStage::ScanningGlobals:
                if (!RunCoreScan())
                {
                    return FinishCancelled();
                }
                break;

            case AutoInitStage::ValidatingCore:
                if (ValidateAutoInitCriticalValues(false))
                {
                    stateMachine.TransitionTo(
                        AutoInitStage::DiscoveringWorld);
                }
                else
                {
                    ScheduleRetry("关键值不完整");
                }
                break;

            case AutoInitStage::DiscoveringWorld:
                if (!RunWorldDiscovery())
                {
                    return FinishCancelled();
                }
                break;

            case AutoInitStage::ValidatingFinal:
                if (ValidateAutoInitCriticalValues(true))
                {
                    stateMachine.TransitionTo(AutoInitStage::Completing);
                }
                else
                {
                    ScheduleRetry("World 链关键值不完整");
                }
                break;

            case AutoInitStage::WaitingForRetry:
                if (!WaitForAutoInitRetry(kRetryDelayMs))
                {
                    return FinishCancelled();
                }

                ++attempt;
                PrepareAutoInitRetry();
                std::cerr << "[xrd] === Init 第 " << attempt << " 轮 ===\n";
                stateMachine.TransitionTo(AutoInitStage::ScanningGlobals);
                break;

            case AutoInitStage::Completing:
                if (IsAutoInitCancellationRequested())
                {
                    return FinishCancelled();
                }
                ctx.initializing = false;
                ctx.initializationThreadId = 0;
                ctx.inited = true;
                PrintAutoInitSummary();
                std::cerr << "[xrd] === " << configuration.modeTag
                          << " 完成 ===\n";
                stateMachine.TransitionTo(AutoInitStage::Completed);
                transaction.Commit();
                return true;

            case AutoInitStage::Completed:
                transaction.Commit();
                return true;

            case AutoInitStage::Cancelled:
            case AutoInitStage::Failed:
                return false;

            default:
                return FinishFailed();
            }
        }
    }

private:
    static constexpr DWORD kRetryDelayMs = 300;

    void PrintStartBanner() const
    {
        std::cerr << "[xrd] === Xrd-eXternalrEsolve "
                  << configuration.modeTag << " ===\n";
    }

    void ScheduleRetry(const char* reason)
    {
        std::cerr << "[xrd] " << reason << "，"
                  << kRetryDelayMs << "ms 后重试\n";
        stateMachine.TransitionTo(AutoInitStage::WaitingForRetry);
    }

    bool RunCoreScan()
    {
        InitStepResult result = RunAutoInitCoreScan(stateMachine);
        if (result == InitStepResult::Cancelled)
        {
            return false;
        }
        if (result == InitStepResult::RetryRequired)
        {
            ScheduleRetry("扫描失败");
            return true;
        }

        stateMachine.TransitionTo(AutoInitStage::ValidatingCore);
        return true;
    }

    bool RunWorldDiscovery()
    {
        InitStepResult result = RunAutoInitWorldDiscovery(stateMachine);
        if (result == InitStepResult::Cancelled)
        {
            return false;
        }
        if (result == InitStepResult::RetryRequired)
        {
            ScheduleRetry("World 链发现失败");
            return true;
        }

        stateMachine.TransitionTo(AutoInitStage::ValidatingFinal);
        return true;
    }

    bool FinishCancelled()
    {
        stateMachine.TransitionTo(AutoInitStage::Cancelled);
        std::cerr << "[xrd] " << configuration.modeTag
                  << " 收到取消请求，已回滚当前初始化\n";
        return false;
    }

    bool FinishFailed()
    {
        stateMachine.TransitionTo(AutoInitStage::Failed);
        std::cerr << "[xrd] " << configuration.modeTag
                  << " 初始化失败，已回滚上下文\n";
        return false;
    }

    AutoInitConfiguration configuration;
    AutoInitStateMachine stateMachine;
    int attempt = 1;
};

inline bool RunAutoInit(const AutoInitConfiguration& configuration)
{
    AutoInitController controller(configuration);
    return controller.Run();
}

} // namespace detail
} // namespace xrd
