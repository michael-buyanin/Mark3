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
    @file eventflag.cpp
    @brief Event Flag Blocking Object/IPC-Object implementation.
*/

#include "mark3.h"

#if KERNEL_EVENT_FLAGS

namespace Mark3
{
namespace
{
    //---------------------------------------------------------------------------
    /**
     * @brief TimedEventFlag_Callback
     *
     * This funciton is called whenever a timed event flag wait operation fails
     * in the time provided.  This function wakes the thread for which the timeout
     * was requested on the blocking call, sets the thread's expiry flags, and
     * reschedules if necessary.
     *
     * @param pclOwner_ Thread to wake
     * @param pvData_ Pointer to the event-flag object
     */
    void TimedEventFlag_Callback(Thread* pclOwner_, void* pvData_)
    {
        KERNEL_ASSERT(nullptr != pclOwner_);
        KERNEL_ASSERT(nullptr != pvData_);

        auto* pclEventFlag = static_cast<EventFlag*>(pvData_);

        pclOwner_->SetExpired(true);
        pclOwner_->SetEventFlagMask(0);

        pclEventFlag->WakeMe(pclOwner_);
        if (pclOwner_->GetCurPriority() >= Scheduler::GetCurrentThread()->GetCurPriority()) {
            Thread::Yield();
        }
    }
} // anonymous namespace
//---------------------------------------------------------------------------
EventFlag::~EventFlag()
{
    // If there are any threads waiting on this object when it goes out
    // of scope, set a kernel panic.
    if (nullptr != m_clBlockList.HighestWaiter()) {
        Kernel::Panic(PANIC_ACTIVE_EVENTFLAG_DESCOPED);
    }
}

//---------------------------------------------------------------------------
void EventFlag::Init()
{
    KERNEL_ASSERT(!m_clBlockList.GetHead());
    m_u16SetMask = 0;
    SetInitialized();
}

//---------------------------------------------------------------------------
void EventFlag::WakeMe(Thread* pclChosenOne_)
{
    KERNEL_ASSERT(IsInitialized());
    KERNEL_ASSERT(nullptr != pclChosenOne_);

    UnBlock(pclChosenOne_);
}

//---------------------------------------------------------------------------
uint16_t EventFlag::Wait_i(uint16_t u16Mask_, EventFlagOperation eMode_, uint32_t u32TimeMS_)
{
    KERNEL_ASSERT(eMode_ <= EventFlagOperation::Pending_Unblock);
    KERNEL_ASSERT(IsInitialized());

    auto bThreadYield = false;
    auto bMatch       = false;

    auto clEventTimer = Timer {};
    auto bUseTimer    = false;

    // Ensure we're operating in a critical section while we determine
    // whether or not we need to block the current thread on this object.
    CS_ENTER();

    // Check to see whether or not the current mask matches any of the
    // desired bits.
    g_pclCurrent->SetEventFlagMask(u16Mask_);

    if ((EventFlagOperation::All_Set == eMode_) || (EventFlagOperation::All_Clear == eMode_)) {
        // Check to see if the flags in their current state match all of
        // the set flags in the event flag group, with this mask.
        if ((m_u16SetMask & u16Mask_) == u16Mask_) {
            bMatch = true;
            g_pclCurrent->SetEventFlagMask(u16Mask_);

            if (EventFlagOperation::All_Clear == eMode_) {
                m_u16SetMask &= ~u16Mask_;
                g_pclCurrent->SetExpired(false);
            }
        }
    } else if ((EventFlagOperation::Any_Set == eMode_) || (EventFlagOperation::Any_Clear == eMode_)) {
        // Check to see if the existing flags match any of the set flags in
        // the event flag group  with this mask
        if ((m_u16SetMask & u16Mask_) != 0) {
            bMatch = true;
            g_pclCurrent->SetEventFlagMask(m_u16SetMask & u16Mask_);

            if (EventFlagOperation::Any_Clear == eMode_) {
                m_u16SetMask &= ~u16Mask_;
                g_pclCurrent->SetExpired(false);
            }
        }
    }

    // We're unable to match this pattern as-is, so we must block.
    if (!bMatch) {
        // Reset the current thread's event flag mask & mode
        g_pclCurrent->SetEventFlagMask(u16Mask_);
        g_pclCurrent->SetEventFlagMode(eMode_);

        if (0u != u32TimeMS_) {
            g_pclCurrent->SetExpired(false);
            clEventTimer.Init();
            clEventTimer.Start(false, u32TimeMS_, TimedEventFlag_Callback, this);
            bUseTimer = true;
        }

        // Add the thread to the object's block-list.
        BlockPriority(g_pclCurrent);

        // Trigger that
        bThreadYield = true;
    }

    // If bThreadYield is set, it means that we've blocked the current thread,
    // and must therefore rerun the scheduler to determine what thread to
    // switch to.
    if (bThreadYield) {
        // Switch threads immediately
        Thread::Yield();
    }

    // Exit the critical section and return back to normal execution
    CS_EXIT();

    //!! If the Yield operation causes a new thread to be chosen, there will
    //!! Be a context switch at the above CS_EXIT().  The original calling
    //!! thread will not return back until a matching SetFlags call is made
    //!! or a timeout occurs.
    if (bUseTimer && bThreadYield) {
        clEventTimer.Stop();
    }

    return g_pclCurrent->GetEventFlagMask();
}

//---------------------------------------------------------------------------
uint16_t EventFlag::Wait(uint16_t u16Mask_, EventFlagOperation eMode_)
{
    KERNEL_ASSERT(eMode_ <= EventFlagOperation::Pending_Unblock);
    return Wait_i(u16Mask_, eMode_, 0);
}

//---------------------------------------------------------------------------
uint16_t EventFlag::Wait(uint16_t u16Mask_, EventFlagOperation eMode_, uint32_t u32TimeMS_)
{
    KERNEL_ASSERT(eMode_ <= EventFlagOperation::Pending_Unblock);
    return Wait_i(u16Mask_, eMode_, u32TimeMS_);
}

//---------------------------------------------------------------------------
void EventFlag::Set(uint16_t u16Mask_)
{
    KERNEL_ASSERT(IsInitialized());

    auto bReschedule = false;

    CS_ENTER();
    // Walk through the whole block list, checking to see whether or not
    // the current flag set now matches any/all of the masks and modes of
    // the threads involved.

    m_u16SetMask |= u16Mask_;
    auto u16NewMask = m_u16SetMask;

    // Start at the head of the list, and iterate through until we hit the
    // "head" element in the list again.  Ensure that we handle the case where
    // we remove the first or last elements in the list, or if there's only
    // one element in the list.

    auto* pclCurrent = static_cast<Thread*>(m_clBlockList.GetHead());
    // Do nothing when there are no objects blocking.
    if (nullptr != pclCurrent) {
        // First loop - process every thread in the block-list and check to
        // see whether or not the current flags match the event-flag conditions
        // on the thread.
        auto* pclPrev = static_cast<Thread*>(nullptr);
        do {
            pclPrev    = pclCurrent;
            pclCurrent = static_cast<Thread*>(pclCurrent->GetNext());

            // Read the thread's event mask/mode
            auto u16ThreadMask = pclPrev->GetEventFlagMask();
            auto eThreadMode   = pclPrev->GetEventFlagMode();

            // For the "any" mode - unblock the blocked threads if one or more bits
            // in the thread's bitmask match the object's bitmask
            if ((EventFlagOperation::Any_Set == eThreadMode) || (EventFlagOperation::Any_Clear == eThreadMode)) {
                if ((u16ThreadMask & m_u16SetMask) != 0) {
                    pclPrev->SetEventFlagMode(EventFlagOperation::Pending_Unblock);
                    pclPrev->SetEventFlagMask(m_u16SetMask & u16ThreadMask);
                    bReschedule = true;

                    // If the "clear" variant is set, then clear the bits in the mask
                    // that caused the thread to unblock.
                    if (EventFlagOperation::Any_Clear == eThreadMode) {
                        u16NewMask &= ~(u16ThreadMask & u16Mask_);
                    }
                }
            }
            // For the "all" mode, every set bit in the thread's requested bitmask must
            // match the object's flag mask.
            else if ((EventFlagOperation::All_Set == eThreadMode) || (EventFlagOperation::All_Clear == eThreadMode)) {
                if ((u16ThreadMask & m_u16SetMask) == u16ThreadMask) {
                    pclPrev->SetEventFlagMode(EventFlagOperation::Pending_Unblock);
                    pclPrev->SetEventFlagMask(u16ThreadMask);
                    bReschedule = true;

                    // If the "clear" variant is set, then clear the bits in the mask
                    // that caused the thread to unblock.
                    if (EventFlagOperation::All_Clear == eThreadMode) {
                        u16NewMask &= ~(u16ThreadMask & u16Mask_);
                    }
                }
            }
        }
        // To keep looping, ensure that there's something in the list, and
        // that the next item isn't the head of the list.
        while (pclPrev != m_clBlockList.GetTail());

        // Second loop - go through and unblock all of the threads that
        // were tagged for unblocking.
        pclCurrent   = static_cast<Thread*>(m_clBlockList.GetHead());
        auto bIsTail = false;
        do {
            pclPrev    = pclCurrent;
            pclCurrent = static_cast<Thread*>(pclCurrent->GetNext());

            // Check to see if this is the condition to terminate the loop
            if (pclPrev == m_clBlockList.GetTail()) {
                bIsTail = true;
            }

            // If the first pass indicated that this thread should be
            // unblocked, then unblock the thread
            if (pclPrev->GetEventFlagMode() == EventFlagOperation::Pending_Unblock) {
                UnBlock(pclPrev);
            }
        } while (!bIsTail);
    }

    // If we awoke any threads, re-run the scheduler
    if (bReschedule) {
        Thread::Yield();
    }

    // Update the bitmask based on any "clear" operations performed along
    // the way
    m_u16SetMask = u16NewMask;

    // Restore interrupts - will potentially cause a context switch if a
    // thread is unblocked.
    CS_EXIT();
}

//---------------------------------------------------------------------------
void EventFlag::Clear(uint16_t u16Mask_)
{
    KERNEL_ASSERT(IsInitialized());

    // Just clear the bitfields in the local object.
    CS_ENTER();
    m_u16SetMask &= ~u16Mask_;
    CS_EXIT();
}

//---------------------------------------------------------------------------
uint16_t EventFlag::GetMask()
{
    KERNEL_ASSERT(IsInitialized());

    // Return the presently held event flag values in this object.  Ensure
    // we get this within a critical section to guarantee atomicity.
    uint16_t u16Return;
    CS_ENTER();
    u16Return = m_u16SetMask;
    CS_EXIT();
    return u16Return;
}
} // namespace Mark3
#endif // #if KERNEL_EVENT_FLAGS
