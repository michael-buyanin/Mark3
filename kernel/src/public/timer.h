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
=========================================================================== */
/**

    @file   timer.h

    @brief  Timer object declarations
 */

#pragma once
#include "kerneltypes.h"
#include "mark3cfg.h"

#include "ll.h"

namespace Mark3
{
class Thread;

//---------------------------------------------------------------------------
#define TIMERLIST_FLAG_ONE_SHOT (0x01) //!< Timer is one-shot
#define TIMERLIST_FLAG_ACTIVE (0x02)   //!< Timer is currently active
#define TIMERLIST_FLAG_CALLBACK (0x04) //!< Timer is pending a callback
#define TIMERLIST_FLAG_EXPIRED (0x08)  //!< Timer is actually expired.

//---------------------------------------------------------------------------
#define TIMER_INVALID_COOKIE (0x3C)
#define TIMER_INIT_COOKIE (0xC3)

//---------------------------------------------------------------------------
#define MAX_TIMER_TICKS (0x7FFFFFFF) //!< Maximum value to set
#define TIMER_TICKS_INVALID (0x80000000)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// add time because we don't know how far in an epoch we are when a call is made.
#define SECONDS_TO_TICKS(x) (((uint32_t)(x)*1000))
#define MSECONDS_TO_TICKS(x) ((uint32_t)(x))
#define USECONDS_TO_TICKS(x) (((uint32_t)(x + 999)) / 1000)

//---------------------------------------------------------------------------
/**
 * This type defines the callback function type for timer events.  Since these
 * are called from an interrupt context, they do not operate from within a
 * thread or object context directly -- as a result, the context must be
 * manually passed into the calls.
 *
 * pclOwner_ is a pointer to the thread that owns the timer
 * pvData_ is a pointer to some data or object that needs to know about the
 *         timer's expiry from within the timer interrupt context.
 */
using TimerCallback = void (*)(Thread* pclOwner_, void* pvData_);

//---------------------------------------------------------------------------
class TimerList;
class TimerScheduler;
class Quantum;

//---------------------------------------------------------------------------
/**
 * @brief Kernel-managed software timers.
 *
 * Kernel-managed timers, used to provide high-precision high-resolution
 * delays.  Functionality is useful to both user-code, and is used extensively
 * within the kernel and its blocking objects to implement round-robin
 * scheduling, thread sleep, and timeouts.  Relies on a single hardware timer,
 * which is multiplexed through the kernel.
 */
class Timer : public LinkListNode
{
public:
    void* operator new(size_t sz, void* pv) { return (Timer*)pv; }
    ~Timer() {}

    /**
     *  @brief Timer
     *
     *  Default Constructor - Do nothing.  Allow the init call to perform
     *  the necessary object initialization prior to use.
     */
    Timer();

    /**
     *  @brief Init
     *
     * Re-initialize the Timer to default values.
     */
    void Init();

    /**
     *  @brief Start
     *
     *  Start a timer using default ownership, using repeats as an option, and
     *  millisecond resolution.
     *
     *  @param bRepeat_ 0 - timer is one-shot.  1 - timer is repeating.
     *  @param u32IntervalMs_ - Interval of the timer in miliseconds
     *  @param pfCallback_ - Function to call on timer expiry
     *  @param pvData_ - Data to pass into the callback function
     */
    void Start(bool bRepeat_, uint32_t u32IntervalMs_, TimerCallback pfCallback_, void* pvData_);

    /**
     *  @brief Start
     *
     *  Start a timer using default ownership, using repeats as an option, and
     *  millisecond resolution.
     *
     *  @param bRepeat_ 0 - timer is one-shot.  1 - timer is repeating.
     *  @param u32IntervalMs_ - Interval of the timer in miliseconds
     *  @param u32ToleranceMs_ - Allow the timer expiry to be delayed by an additional maximum time, in
     *                         order to have as many timers expire at the same time as possible.
     *  @param pfCallback_ - Function to call on timer expiry
     *  @param pvData_ - Data to pass into the callback function
     */
    void
    Start(bool bRepeat_, uint32_t u32IntervalMs_, uint32_t u32ToleranceMs_, TimerCallback pfCallback_, void* pvData_);

    /**
     * @brief Start
     *
     * Start or restart a timer using parameters previously configured via
     * calls to Start(<with args>), or via the a-la-carte parameter setter
     * methods.  This is especially useful for retriggering one-shot timers
     * that have previously expired, using the timer's previous configuration.
     */
    void Start();

    /**
     *  @brief Stop
     *
     *  Stop a timer already in progress.   Has no effect on timers that have
     *  already been stopped.
     */
    void Stop();

    /**
     *  @brief SetFlags
     *
     * Set the timer's flags based on the bits in the u8Flags_ argument
     *
     * @param u8Flags_ Flags to assign to the timer object.
     *             TIMERLIST_FLAG_ONE_SHOT for a one-shot timer,
     *             0 for a continuous timer.
     */
    void SetFlags(uint8_t u8Flags_) { m_u8Flags = u8Flags_; }
    /**
     *  @brief SetCallback
     *
     *  Define the callback function to be executed on expiry of the timer
     *
     *  @param pfCallback_ Pointer to the callback function to call
     */
    void SetCallback(TimerCallback pfCallback_) { m_pfCallback = pfCallback_; }
    /**
     *  @brief SetData
     *
     *  Define a pointer to be sent to the timer callbcak on timer expiry
     *
     *  @param pvData_ Pointer to data to pass as argument into the callback
     */
    void SetData(void* pvData_) { m_pvData = pvData_; }
    /**
     *  @brief SetOwner
     *
     *  Set the owner-thread of this timer object (all timers must be owned by
     *  a thread).
     *
     *  @param pclOwner_ Owner thread of this timer object
     */
    void SetOwner(Thread* pclOwner_) { m_pclOwner = pclOwner_; }
    /**
     *  @brief SetIntervalTicks
     *
     *  Set the timer expiry in system-ticks (platform specific!)
     *
     *  @param u32Ticks_ Time in ticks
     */
    void SetIntervalTicks(uint32_t u32Ticks_);

    /**
     *  @brief SetIntervalSeconds
     *
     *  Set the timer expiry interval in seconds (platform agnostic)
     *
     *  @param u32Seconds_ Time in seconds
     */
    void SetIntervalSeconds(uint32_t u32Seconds_);

    /**
     * @brief GetInterval
     *
     * Return the timer's configured interval in ticks
     *
     * @return Timer interval in ticks.
     */
    uint32_t GetInterval() { return m_u32Interval; }
    /**
     *  @brief SetIntervalMSeconds
     *
     *  Set the timer expiry interval in milliseconds (platform agnostic)
     *
     *  @param u32MSeconds_ Time in milliseconds
     */
    void SetIntervalMSeconds(uint32_t u32MSeconds_);

    /**
     *  @brief SetIntervalUSeconds
     *
     *  Set the timer expiry interval in microseconds (platform agnostic)
     *
     *  @param u32USeconds_ Time in microseconds
     */
    void SetIntervalUSeconds(uint32_t u32USeconds_);

    /**
     *  @brief SetTolerance
     *
     *  Set the timer's maximum tolerance in order to synchronize timer
     *  processing with other timers in the system.
     *
     *  @param u32Ticks_ Maximum tolerance in ticks
     */
    void SetTolerance(uint32_t u32Ticks_);

private:
    friend class TimerList;

    /**
     * @brief SetInitialized
     */
    void SetInitialized() { m_u8Initialized = TIMER_INIT_COOKIE; }

    /**
     * @brief IsInitialized
     * @return
     */
    bool IsInitialized(void) { return (m_u8Initialized == TIMER_INIT_COOKIE); }

    //! Cookie used to determine whether or not the timer is initialized
    uint8_t m_u8Initialized;

    //! Flags for the timer, defining if the timer is one-shot or repeated
    uint8_t m_u8Flags;

    //! Pointer to the callback function
    TimerCallback m_pfCallback;

    //! Interval of the timer in timer ticks
    uint32_t m_u32Interval;

    //! Time remaining on the timer
    uint32_t m_u32TimeLeft;

    //! Maximum tolerance (usedd for timer harmonization)
    uint32_t m_u32TimerTolerance;

    //! Pointer to the owner thread
    Thread* m_pclOwner;

    //! Pointer to the callback data
    void* m_pvData;
};
} // namespace Mark3
