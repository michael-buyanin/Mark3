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
=========================================================================== */
/**

    @file   threadlist.h

    @brief  Thread linked-list declarations

 */

#pragma once

#include "kerneltypes.h"
#include "priomap.h"
#include "ll.h"

//---------------------------------------------------------------------------
namespace Mark3
{
class Thread;

/**
 * @brief The ThreadList Class.
 * This class is used for building thread-management facilities, such as
 * schedulers, and blocking objects.
 */
class ThreadList : public TypedLinkListNode<ThreadList>, public TypedCircularLinkList<Thread>
{
public:
    void* operator new(size_t sz, void* pv) { return reinterpret_cast<ThreadList*>(pv); };
    /**
     *  @brief ThreadList
     *  Default constructor - zero-initializes the data.
     */
    ThreadList();

    /**
     *  @brief SetPriority
     *  Set the priority of this threadlist (if used for a scheduler).
     *
     *  @param uXPriority_ Priority level of the thread list
     */
    void SetPriority(PORT_PRIO_TYPE uXPriority_);

    /**
     *  @brief SetMapPointer
     *  Set the pointer to a bitmap to use for this threadlist.  Once again,
     *  only needed when the threadlist is being used for scheduling purposes.
     *
     *  @param pclMap_ Pointer to the priority map object used to track this
     *                 thread.
     */
    void SetMapPointer(PriorityMap* pclMap_);

    /**
     *  @brief Add
     *  Add a thread to the threadlist.
     *
     *  @param node_ Pointer to the thread (link list node) to add to the list
     */
    void Add(Thread* node_);

    /**
     *  @brief Add
     *  Add a thread to the threadlist, specifying the flag and priority at
     *  the same time.
     *
     *  @param node_        Pointer to the thread to add (link list node)
     *  @param pclMap_      Pointer to the bitmap flag to set (if used in
     *                      a scheduler context), or nullptr for non-scheduler.
     *  @param uXPriority_  Priority of the threadlist
     */
    void Add(Thread* node_, PriorityMap* pclMap_, PORT_PRIO_TYPE uXPriority_);

    /**
     * @brief AddPriority
     * Add a thread to the list such that threads are ordered from highest to
     * lowest priority from the head of the list.
     *
     * @param node_         Pointer to a thread to add to the list.
     */
    void AddPriority(Thread* node_);

    /**
     *  @brief Remove
     *  Remove the specified thread from the threadlist
     *
     *  @param node_ Pointer to the thread to remove
     */
    void Remove(Thread* node_);

    /**
     *  @brief HighestWaiter
     *  Return a pointer to the highest-priority thread in the thread-list.
     *
     *  @return Pointer to the highest-priority thread
     */
    Thread* HighestWaiter();

private:
    //! Priority of the threadlist
    PORT_PRIO_TYPE m_uXPriority;

    //! Pointer to the bitmap/flag to set when used for scheduling.
    PriorityMap* m_pclMap;
};
} // namespace Mark3
