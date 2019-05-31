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

    @file   ll.cpp

    @brief  Core Linked-List implementation, from which all kernel objects
            are derived
*/

#include "mark3.h"

namespace Mark3
{
//---------------------------------------------------------------------------
void LinkListNode::ClearNode()
{
    next = nullptr;
    prev = nullptr;
}

//---------------------------------------------------------------------------
void DoubleLinkList::Add(LinkListNode* node_)
{
    KERNEL_ASSERT(nullptr != node_);

    node_->prev = m_pclTail;
    node_->next = nullptr;

    // If the list is empty, initilize the head
    if (nullptr == m_pclHead) {
        m_pclHead = node_;
    }
    // Otherwise, adjust the tail's next pointer
    else {
        m_pclTail->next = node_;
    }

    // Move the tail node, and assign it to the new node just passed in
    m_pclTail = node_;
}

//---------------------------------------------------------------------------
void DoubleLinkList::Remove(LinkListNode* node_)
{
    KERNEL_ASSERT(nullptr != node_);

    if (nullptr != node_->prev) {
        if (node_->prev->next != node_) {
            Kernel::Panic(PANIC_LIST_UNLINK_FAILED);
        }
        node_->prev->next = node_->next;
    }
    if (nullptr != node_->next) {
        if (node_->next->prev != node_) {
            Kernel::Panic(PANIC_LIST_UNLINK_FAILED);
        }
        node_->next->prev = node_->prev;
    }
    if (node_ == m_pclHead) {
        m_pclHead = node_->next;
    }
    if (node_ == m_pclTail) {
        m_pclTail = node_->prev;
    }
    node_->ClearNode();
}

//---------------------------------------------------------------------------
void CircularLinkList::Add(LinkListNode* node_)
{
    KERNEL_ASSERT(nullptr != node_);

    if (nullptr == m_pclHead) {
        // If the list is empty, initilize the nodes
        m_pclHead = node_;
        m_pclTail = node_;
    } else {
        // Move the tail node, and assign it to the new node just passed in
        m_pclTail->next = node_;
    }

    // Add a node to the end of the linked list.
    node_->prev = m_pclTail;
    node_->next = m_pclHead;

    m_pclTail       = node_;
    m_pclHead->prev = node_;
}

//---------------------------------------------------------------------------
void CircularLinkList::Remove(LinkListNode* node_)
{
    KERNEL_ASSERT(nullptr != node_);

    // Check to see if this is the head of the list...
    if ((node_ == m_pclHead) && (m_pclHead == m_pclTail)) {
        // Clear the head and tail pointers - nothing else left.
        m_pclHead = nullptr;
        m_pclTail = nullptr;
        return;
    }

    // Verify that all nodes are properly connected
    if ((node_->prev->next != node_) || (node_->next->prev != node_)) {
        Kernel::Panic(PANIC_LIST_UNLINK_FAILED);
    }

    // This is a circularly linked list - no need to check for connection,
    // just remove the node.
    node_->next->prev = node_->prev;
    node_->prev->next = node_->next;

    if (node_ == m_pclHead) {
        m_pclHead = m_pclHead->next;
    }
    if (node_ == m_pclTail) {
        m_pclTail = m_pclTail->prev;
    }
    node_->ClearNode();
}

//---------------------------------------------------------------------------
void CircularLinkList::PivotForward()
{
    if (nullptr != m_pclHead) {
        m_pclHead = m_pclHead->next;
        m_pclTail = m_pclTail->next;
    }
}

//---------------------------------------------------------------------------
void CircularLinkList::PivotBackward()
{
    if (nullptr != m_pclHead) {
        m_pclHead = m_pclHead->prev;
        m_pclTail = m_pclTail->prev;
    }
}

//---------------------------------------------------------------------------
void CircularLinkList::InsertNodeBefore(LinkListNode* node_, LinkListNode* insert_)
{
    KERNEL_ASSERT(nullptr != node_);
    KERNEL_ASSERT(nullptr != insert_);

    node_->next = insert_;
    node_->prev = insert_->prev;

    if (nullptr != insert_->prev) {
        insert_->prev->next = node_;
    }
    insert_->prev = node_;
}
} // namespace Mark3
