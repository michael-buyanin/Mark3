/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012 - 2018 m0slevin, all rights reserved.
See license.txt for more information
===========================================================================*/
/**

    @file   notify.cpp

    @brief  Lightweight thread notification - blocking object

*/
#include "mark3.h"
namespace Mark3
{
namespace
{
    //---------------------------------------------------------------------------
    void TimedNotify_Callback(Thread* pclOwner_, void* pvData_)
    {
        KERNEL_ASSERT(pclOwner_ != nullptr);
        KERNEL_ASSERT(pvData_ != nullptr);

        auto* pclNotify = static_cast<Notify*>(pvData_);

        // Indicate that the semaphore has expired on the thread
        pclOwner_->SetExpired(true);

        // Wake up the thread that was blocked on this semaphore.
        pclNotify->WakeMe(pclOwner_);

        if (pclOwner_->GetCurPriority() >= Scheduler::GetCurrentThread()->GetCurPriority()) {
            Thread::Yield();
        }
    }
} // anonymous namespace

//---------------------------------------------------------------------------
Notify::~Notify()
{
    // If there are any threads waiting on this object when it goes out
    // of scope, set a kernel panic.
    if (m_clBlockList.GetHead() != nullptr) {
        Kernel::Panic(PANIC_ACTIVE_NOTIFY_DESCOPED);
    }
}

//---------------------------------------------------------------------------
void Notify::Init(void)
{
    KERNEL_ASSERT(!m_clBlockList.GetHead());
    SetInitialized();

    m_bPending = false;
}

//---------------------------------------------------------------------------
void Notify::Signal(void)
{
    KERNEL_ASSERT(IsInitialized());

    auto bReschedule = false;

    CS_ENTER();
    auto* pclCurrent = static_cast<Thread*>(m_clBlockList.GetHead());
    if (pclCurrent == nullptr) {
        m_bPending = true;
    } else {
        while (pclCurrent != nullptr) {
            UnBlock(pclCurrent);
            if (!bReschedule && (pclCurrent->GetCurPriority() >= Scheduler::GetCurrentThread()->GetCurPriority())) {
                bReschedule = true;
            }
            pclCurrent = (Thread*)m_clBlockList.GetHead();
        }
        m_bPending = false;
    }
    CS_EXIT();

    if (bReschedule) {
        Thread::Yield();
    }
}

//---------------------------------------------------------------------------
void Notify::Wait(bool* pbFlag_)
{
    KERNEL_ASSERT(pbFlag_ != nullptr);
    KERNEL_ASSERT(IsInitialized());

    auto bEarlyExit = false;
    CS_ENTER();
    if (!m_bPending) {
        Block(g_pclCurrent);
        if (pbFlag_ != nullptr) {
            *pbFlag_ = false;
        }
    } else {
        m_bPending = false;
        bEarlyExit = true;
    }
    CS_EXIT();

    if (bEarlyExit) {
        return;
    }

    Thread::Yield();
    if (pbFlag_ != nullptr) {
        *pbFlag_ = true;
    }
}

//---------------------------------------------------------------------------
bool Notify::Wait(uint32_t u32WaitTimeMS_, bool* pbFlag_)
{
    KERNEL_ASSERT(pbFlag_ != nullptr);
    KERNEL_ASSERT(IsInitialized());

    auto  bUseTimer  = false;
    auto  bEarlyExit = false;
    Timer clNotifyTimer;

    CS_ENTER();
    if (!m_bPending) {
        if (u32WaitTimeMS_ != 0u) {
            bUseTimer = true;
            g_pclCurrent->SetExpired(false);

            clNotifyTimer.Init();
            clNotifyTimer.Start(false, u32WaitTimeMS_, TimedNotify_Callback, this);
        }

        Block(g_pclCurrent);

        if (pbFlag_ != nullptr) {
            *pbFlag_ = false;
        }
    } else {
        m_bPending = false;
        bEarlyExit = true;
    }
    CS_EXIT();

    if (bEarlyExit) {
        return true;
    }

    Thread::Yield();

    if (bUseTimer) {
        clNotifyTimer.Stop();
        return (static_cast<int>(g_pclCurrent->GetExpired()) == 0);
    }

    if (pbFlag_ != nullptr) {
        *pbFlag_ = true;
    }

    return true;
}

//---------------------------------------------------------------------------
void Notify::WakeMe(Thread* pclChosenOne_)
{
    KERNEL_ASSERT(pclChosenOne_ != nullptr);
    KERNEL_ASSERT(IsInitialized());

    UnBlock(pclChosenOne_);
}
} // namespace Mark3
