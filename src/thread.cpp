/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012 - 2019 m0slevin, all rights reserved.
See license.txt for more information
===========================================================================*/
/**

    @file   thread.cpp

    @brief  Platform-Independent thread class Definition

*/

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "mark3.h"

namespace Mark3
{
//---------------------------------------------------------------------------
Thread::~Thread()
{
    // On destruction of a thread located on a stack,
    // ensure that the thread is either stopped, or exited.
    // If the thread is stopped, move it to the exit state.
    // If not in the exit state, kernel panic -- it's catastrophic to have
    // running threads on stack suddenly disappear.
    if (ThreadState::Stop == m_eState) {
        const auto cs = CriticalGuard{};
        m_pclCurrent->Remove(this);
        m_pclCurrent = nullptr;
        m_pclOwner   = nullptr;
        m_eState     = ThreadState::Exit;        
    } else if (ThreadState::Exit != m_eState) {
        Kernel::Panic(PANIC_RUNNING_THREAD_DESCOPED);
    }
}

//---------------------------------------------------------------------------
void Thread::Init(
    K_WORD* pwStack_, uint16_t u16StackSize_, PORT_PRIO_TYPE uXPriority_, ThreadEntryFunc pfEntryPoint_, void* pvArg_)
{
    static auto u8ThreadID = uint8_t { 0 };

    KERNEL_ASSERT(pwStack_);
    KERNEL_ASSERT(pfEntryPoint_);

    ClearNode();

    m_u8ThreadID = u8ThreadID++;

    // Initialize the thread parameters to their initial values.
    m_pwStack    = pwStack_;
    m_pwStackTop = PORT_TOP_OF_STACK(pwStack_, u16StackSize_);

    m_u16StackSize  = u16StackSize_;
    m_uXPriority    = uXPriority_;
    m_uXCurPriority = m_uXPriority;
    m_pfEntryPoint  = pfEntryPoint_;
    m_pvArg         = pvArg_;

#if KERNEL_NAMED_THREADS
    m_szName = nullptr;
#endif
#if KERNEL_ROUND_ROBIN
    m_u16Quantum = THREAD_QUANTUM_DEFAULT;
#endif

    m_clTimer.Init();

    // Call CPU-specific stack initialization
    ThreadPort::InitStack(this);

    // Add to the global "stop" list.
    { // Begin critical section
        const auto cs = CriticalGuard{};
        m_pclOwner   = Scheduler::GetThreadList(m_uXPriority);
        m_pclCurrent = Scheduler::GetStopList();
        m_eState     = ThreadState::Stop;
        m_pclCurrent->Add(this);
    } // End critical section

#if KERNEL_THREAD_CREATE_CALLOUT
    ThreadCreateCallout pfCallout = Kernel::GetThreadCreateCallout();
    if (nullptr != pfCallout) {
        pfCallout(this);
    }
#endif
}

//---------------------------------------------------------------------------
Thread* Thread::Init(uint16_t u16StackSize_, PORT_PRIO_TYPE uXPriority_, ThreadEntryFunc pfEntryPoint_, void* pvArg_)
{
    auto* pclNew  = AutoAlloc::NewObject<Thread, AutoAllocType::Thread>();
    auto* pwStack = static_cast<K_WORD*>(AutoAlloc::NewRawData(u16StackSize_));
    pclNew->Init(pwStack, u16StackSize_, uXPriority_, pfEntryPoint_, pvArg_);
    return pclNew;
}

//---------------------------------------------------------------------------
void Thread::Start(void)
{
    KERNEL_ASSERT(IsInitialized());

    // Remove the thread from the scheduler's "stopped" list, and add it
    // to the scheduler's ready list at the proper priority.

    const auto cs = CriticalGuard{};
    Scheduler::GetStopList()->Remove(this);
    Scheduler::Add(this);
    m_pclOwner   = Scheduler::GetThreadList(m_uXPriority);
    m_pclCurrent = m_pclOwner;
    m_eState     = ThreadState::Ready;

#if KERNEL_ROUND_ROBIN
    if (Kernel::IsStarted()) {
        if (GetCurPriority() >= Scheduler::GetCurrentThread()->GetCurPriority()) {
            // Deal with the thread Quantum
            Quantum::Update(this);
        }
    }
#endif

    if (Kernel::IsStarted()) {
        if (GetCurPriority() >= Scheduler::GetCurrentThread()->GetCurPriority()) {
            Thread::Yield();
        }
    }    
}

//---------------------------------------------------------------------------
void Thread::Stop()
{
    KERNEL_ASSERT(IsInitialized());

    auto bReschedule = false;
    if (ThreadState::Stop == m_eState) {
        return;
    }

    { // Begin critical section
        const auto cs = CriticalGuard{};

        // If a thread is attempting to stop itself, ensure we call the scheduler
        if (this == Scheduler::GetCurrentThread()) {
            bReschedule = true;
#if KERNEL_ROUND_ROBIN
            // Cancel RR scheduling
            Quantum::Cancel();
#endif
        }

        // Add this thread to the stop-list (removing it from active scheduling)
        // Remove the thread from scheduling
        if (ThreadState::Ready == m_eState) {
            Scheduler::Remove(this);
        } else if (ThreadState::Blocked == m_eState) {
            m_pclCurrent->Remove(this);
        }

        m_pclOwner   = Scheduler::GetStopList();
        m_pclCurrent = m_pclOwner;
        m_pclOwner->Add(this);
        m_eState = ThreadState::Stop;

        // Just to be safe - attempt to remove the thread's timer
        // from the timer-scheduler (does no harm if it isn't
        // in the timer-list)
        TimerScheduler::Remove(&m_clTimer);
    } // End Critical Section

    if (bReschedule) {
        Thread::Yield();
    }
}

//---------------------------------------------------------------------------
void Thread::Exit()
{
    KERNEL_ASSERT(IsInitialized());

    auto bReschedule = false;

    if (ThreadState::Exit == m_eState) {
        return;
    }

    { // Begin critical section
        const auto cs = CriticalGuard{};

        // If this thread is the actively-running thread, make sure we run the
        // scheduler again.
        if (this == Scheduler::GetCurrentThread()) {
            bReschedule = true;
#if KERNEL_ROUND_ROBIN
            // Cancel RR scheduling
            Quantum::Cancel();
#endif
        }

        // Remove the thread from scheduling
        if (ThreadState::Ready == m_eState) {
            Scheduler::Remove(this);
        } else if ((ThreadState::Blocked == m_eState) || (ThreadState::Stop == m_eState)) {
            m_pclCurrent->Remove(this);
        }

        m_pclCurrent = nullptr;
        m_pclOwner   = nullptr;
        m_eState     = ThreadState::Exit;

        // We've removed the thread from scheduling, but interrupts might
        // trigger checks against this thread's currently priority before
        // we get around to scheduling new threads.  As a result, set the
        // priority to idle to ensure that we always wind up scheduling
        // new threads.
        m_uXCurPriority = 0;
        m_uXPriority    = 0;

        // Just to be safe - attempt to remove the thread's timer
        // from the timer-scheduler (does no harm if it isn't
        // in the timer-list)
        TimerScheduler::Remove(&m_clTimer);
    } // End Critical Section

#if KERNEL_THREAD_EXIT_CALLOUT
    ThreadExitCallout pfCallout = Kernel::GetThreadExitCallout();
    if (nullptr != pfCallout) {
        pfCallout(this);
    }
#endif

    if (bReschedule) {
        // Choose a new "next" thread if we must
        Thread::Yield();
    }
}

//---------------------------------------------------------------------------
void Thread::Sleep(uint32_t u32TimeMs_)
{
    auto  clSemaphore    = Semaphore {};
    auto* pclTimer       = g_pclCurrent->GetTimer();
    auto  lTimerCallback = [](Thread* /*pclOwner*/, void* pvData_) {
        auto* pclSemaphore = static_cast<Semaphore*>(pvData_);
        pclSemaphore->Post();
    };

    // Create a semaphore that this thread will block on
    clSemaphore.Init(0, 1);

    // Create a one-shot timer that will call a callback that posts the
    // semaphore, waking our thread.
    pclTimer->Init();
    pclTimer->Start(false, u32TimeMs_, lTimerCallback, &clSemaphore);

    clSemaphore.Pend();
}

#if KERNEL_STACK_CHECK
//---------------------------------------------------------------------------
uint16_t Thread::GetStackSlack()
{
    KERNEL_ASSERT(IsInitialized());

    auto wBottom = uint16_t { 0 };
    auto wTop    = static_cast<uint16_t>((m_u16StackSize - 1) / sizeof(K_WORD));
    auto wMid    = static_cast<uint16_t>(((wTop + wBottom) + 1) / 2);

    { // Begin critical section
        const auto cs = CriticalGuard{};

        // Logarithmic bisection - find the point where the contents of the
        // stack go from 0xFF's to non 0xFF.  Not Definitive, but accurate enough
        while ((wTop - wBottom) > 1) {
#if PORT_STACK_GROWS_DOWN
            if (m_pwStack[wMid] != static_cast<K_WORD>(-1))
#else
            if (m_pwStack[wMid] == static_cast<K_WORD>(-1))
#endif
            {
                wTop = wMid;
            } else {
                wBottom = wMid;
            }
            wMid = (wTop + wBottom + 1) / 2;
        }
    } // End Critical Section

    return wMid * sizeof(K_WORD);
}
#endif

//---------------------------------------------------------------------------
void Thread::Yield()
{
    const auto cs = CriticalGuard{};
    // Run the scheduler
    if (Scheduler::IsEnabled()) {
        Scheduler::Schedule();

        // Only switch contexts if the new task is different than the old task
        if (g_pclCurrent != g_pclNext) {
#if KERNEL_ROUND_ROBIN
            Quantum::Update(g_pclNext);
#endif
            Thread::ContextSwitchSWI();
        }
    } else {
        Scheduler::QueueScheduler();
    }
}

//---------------------------------------------------------------------------
void Thread::CoopYield(void)
{
    g_pclCurrent->GetCurrent()->PivotForward();
    Yield();
}

//---------------------------------------------------------------------------
void Thread::SetPriorityBase(PORT_PRIO_TYPE /*uXPriority_*/)
{
    KERNEL_ASSERT(IsInitialized());

    GetCurrent()->Remove(this);
    SetCurrent(Scheduler::GetThreadList(m_uXPriority));
    GetCurrent()->Add(this);
}

//---------------------------------------------------------------------------
void Thread::SetPriority(PORT_PRIO_TYPE uXPriority_)
{
    KERNEL_ASSERT(IsInitialized());
    auto bSchedule = false;

    { // Begin critical section
        const auto cs = CriticalGuard{};

        // If this is the currently running thread, it's a good idea to reschedule
        // Or, if the new priority is a higher priority than the current thread's.
        if ((this == g_pclCurrent) || (uXPriority_ > g_pclCurrent->GetPriority())) {
            bSchedule = true;
#if KERNEL_ROUND_ROBIN
            Quantum::Cancel();
#endif
        }
        Scheduler::Remove(this);

        m_uXCurPriority = uXPriority_;
        m_uXPriority    = uXPriority_;

        Scheduler::Add(this);
    } // End critical section

    if (bSchedule) {
        if (Scheduler::IsEnabled()) {
            { // Begin critical section
                const auto cs = CriticalGuard{};
                Scheduler::Schedule();
#if KERNEL_ROUND_ROBIN
                Quantum::Update(g_pclNext);
#endif
            } // End critical sectin
            Thread::ContextSwitchSWI();
        } else {
            Scheduler::QueueScheduler();
        }
    }
}

//---------------------------------------------------------------------------
void Thread::InheritPriority(PORT_PRIO_TYPE uXPriority_)
{
    KERNEL_ASSERT(IsInitialized());

    SetOwner(Scheduler::GetThreadList(uXPriority_));
    m_uXCurPriority = uXPriority_;
}

//---------------------------------------------------------------------------
void Thread::ContextSwitchSWI()
{
    // Call the context switch interrupt if the scheduler is enabled.
    if (Scheduler::IsEnabled()) {
#if KERNEL_STACK_CHECK
        if (g_pclCurrent && (g_pclCurrent->GetStackSlack() <= Kernel::GetStackGuardThreshold())) {
            Kernel::Panic(PANIC_STACK_SLACK_VIOLATED);
        }
#endif
#if KERNEL_CONTEXT_SWITCH_CALLOUT
        auto pfCallout = Kernel::GetThreadContextSwitchCallout();
        if (nullptr != pfCallout) {
            pfCallout(g_pclCurrent);
        }
#endif
        KernelSWI::Trigger();
    }
}

//---------------------------------------------------------------------------
Timer* Thread::GetTimer()
{
    KERNEL_ASSERT(IsInitialized());
    return &m_clTimer;
}
//---------------------------------------------------------------------------
void Thread::SetExpired(bool bExpired_)
{
    KERNEL_ASSERT(IsInitialized());
    m_bExpired = bExpired_;
}

//---------------------------------------------------------------------------
bool Thread::GetExpired()
{
    KERNEL_ASSERT(IsInitialized());
    return m_bExpired;
}
} // namespace Mark3
