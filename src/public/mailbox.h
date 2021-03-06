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

    @file   mailbox.h

    @brief  Mailbox + Envelope IPC Mechanism
*/
#pragma once

#include "mark3cfg.h"
#include "kerneltypes.h"
#include "ithreadport.h"
#include "ksemaphore.h"

namespace Mark3
{
/**
 * @brief The Mailbox class.
 * This class implements an IPC mechnism based on sending/receiving envelopes
 * containing data of a fixed size, configured at initialization) that reside
 * within a buffer of memory provided by the user.
 */
class Mailbox
{
public:
    void* operator new(size_t sz, void* pv) { return reinterpret_cast<Mailbox*>(pv); }
    ~Mailbox();

    /**
     * @brief Init
     * Initialize the mailbox object prior to its use.  This must be called before
     * any calls can be made to the object.
     *
     * @param pvBuffer_         Pointer to the static buffer to use for the mailbox
     * @param u16BufferSize_    Size of the mailbox buffer, in bytes
     * @param u16ElementSize_   Size of each envelope, in bytes
     */
    void Init(void* pvBuffer_, uint16_t u16BufferSize_, uint16_t u16ElementSize_);

    /**
     * @brief Init
     * Create and initialize the mailbox object prior to its use.  This must be
     * called before any calls can be made to the object.  This version of the API
     * alloctes the buffer space from the kernel's Auto-Allocation heap, which cannot
     * be returned back.  As a result, this is only suitable for cases where the
     * mailbox will be created once on startup, and persist for the duration of the
     * system.
     *
     * @param u16BufferSize_    Size of the mailbox buffer, in bytes
     * @param u16ElementSize_   Size of each envelope, in bytes
     */
    static Mailbox* Init(uint16_t u16BufferSize_, uint16_t u16ElementSize_);

    /**
     * @brief Send
     * Send an envelope to the mailbox.  This safely copies the data contents of the
     * datastructure to the previously-initialized mailbox buffer.  If there is a
     * thread already blocking, awaiting delivery to the mailbox, it will be unblocked
     * at this time.
     *
     * This method delivers the envelope at the head of the mailbox.
     *
     * @param pvData_           Pointer to the data object to send to the mailbox.
     * @return                  true - envelope was delivered, false - mailbox is full.
     */
    bool Send(void* pvData_);

    /**
     * @brief SendTail
     * Send an envelope to the mailbox.  This safely copies the data contents of the
     * datastructure to the previously-initialized mailbox buffer.  If there is a
     * thread already blocking, awaiting delivery to the mailbox, it will be unblocked
     * at this time.
     *
     * This method delivers the envelope at the tail of the mailbox.
     *
     * @param pvData_           Pointer to the data object to send to the mailbox.
     * @return                  true - envelope was delivered, false - mailbox is full.
     */
    bool SendTail(void* pvData_);

    /**
     * @brief Send
     * Send an envelope to the mailbox.  This safely copies the data contents of the
     * datastructure to the previously-initialized mailbox buffer.  If there is a
     * thread already blocking, awaiting delivery to the mailbox, it will be unblocked
     * at this time.
     *
     * This method delivers the envelope at the head of the mailbox.
     *
     * @param pvData_           Pointer to the data object to send to the mailbox.
     * @param u32TimeoutMS_      Maximum time to wait for a free transmit slot
     * @return                  true - envelope was delivered, false - mailbox is full.
     */
    bool Send(void* pvData_, uint32_t u32TimeoutMS_);

    /**
     * @brief SendTail
     * Send an envelope to the mailbox.  This safely copies the data contents of the
     * datastructure to the previously-initialized mailbox buffer.  If there is a
     * thread already blocking, awaiting delivery to the mailbox, it will be unblocked
     * at this time.
     *
     * This method delivers the envelope at the tail of the mailbox.
     *
     * @param pvData_           Pointer to the data object to send to the mailbox.
     * @param u32TimeoutMS_      Maximum time to wait for a free transmit slot
     * @return                  true - envelope was delivered, false - mailbox is full.
     */
    bool SendTail(void* pvData_, uint32_t u32TimeoutMS_);

    /**
     * @brief Receive
     * Read one envelope from the head of the mailbox.  If the mailbox is currently
     * empty, the calling thread will block until an envelope is delivered.
     *
     * @param pvData_ Pointer to a buffer that will have the envelope's contents
     *                copied into upon delivery.
     */
    void Receive(void* pvData_);

    /**
     * @brief ReceiveTail
     * Read one envelope from the tail of the mailbox.  If the mailbox is currently
     * empty, the calling thread will block until an envelope is delivered.
     *
     * @param pvData_ Pointer to a buffer that will have the envelope's contents
     *                copied into upon delivery.
     */
    void ReceiveTail(void* pvData_);

    /**
     * @brief Receive
     * Read one envelope from the head of the mailbox.  If the mailbox is currently
     * empty, the calling thread will block until an envelope is delivered, or the
     * specified time has elapsed without delivery.
     *
     * @param pvData_ Pointer to a buffer that will have the envelope's contents
     *                copied into upon delivery.
     * @param u32TimeoutMS_ Maximum time to wait for delivery.
     * @return true - envelope was delivered, false - delivery timed out.
     */
    bool Receive(void* pvData_, uint32_t u32TimeoutMS_);

    /**
     * @brief ReceiveTail
     * Read one envelope from the tail of the mailbox.  If the mailbox is currently
     * empty, the calling thread will block until an envelope is delivered, or the
     * specified time has elapsed without delivery.
     *
     * @param pvData_ Pointer to a buffer that will have the envelope's contents
     *                copied into upon delivery.
     * @param u32TimeoutMS_ Maximum time to wait for delivery.
     * @return true - envelope was delivered, false - delivery timed out.
     */
    bool ReceiveTail(void* pvData_, uint32_t u32TimeoutMS_);

    uint16_t GetFreeSlots(void)
    {
        const auto cs = CriticalGuard{};
        return m_u16Free;
    }

    bool IsFull(void) { return (GetFreeSlots() == 0); }
    bool IsEmpty(void) { return (GetFreeSlots() == m_u16Count); }

private:
    /**
     * @brief GetHeadPointer
     * Return a pointer to the current head of the mailbox's internal
     * circular buffer.
     *
     * @return pointer to the head element in the mailbox
     */
    void* GetHeadPointer(void)
    {
        auto uAddr = reinterpret_cast<K_ADDR>(m_pvBuffer);
        uAddr += static_cast<K_ADDR>(m_u16ElementSize) * static_cast<K_ADDR>(m_u16Head);
        return reinterpret_cast<void*>(uAddr);
    }

    /**
     * @brief GetTailPointer
     * Return a pointer to the current tail of the mailbox's internal
     * circular buffer.
     *
     * @return pointer to the tail element in the mailbox
     */
    void* GetTailPointer(void)
    {
        auto uAddr = reinterpret_cast<K_ADDR>(m_pvBuffer);
        uAddr += static_cast<K_ADDR>(m_u16ElementSize) * static_cast<K_ADDR>(m_u16Tail);
        return reinterpret_cast<void*>(uAddr);
    }

    /**
     * @brief CopyData
     * Perform a direct byte-copy from a source to a destination object.
     *
     * @param src_  Pointer to an object to read from
     * @param dst_  Pointer to an object to write to
     * @param len_  Length to copy (in bytes)
     */
    void CopyData(const void* src_, void* dst_, uint16_t len_)
    {
        auto* u8Src = reinterpret_cast<const uint8_t*>(src_);
        auto* u8Dst = reinterpret_cast<uint8_t*>(dst_);
        while (len_--) { *u8Dst++ = *u8Src++; }
    }

    /**
     * @brief MoveTailForward
     * Move the tail index forward one element
     */
    void MoveTailForward(void)
    {
        m_u16Tail++;
        if (m_u16Tail == m_u16Count) {
            m_u16Tail = 0;
        }
    }

    /**
     * @brief MoveHeadForward
     * Move the head index forward one element
     */
    void MoveHeadForward(void)
    {
        m_u16Head++;
        if (m_u16Head == m_u16Count) {
            m_u16Head = 0;
        }
    }

    /**
     * @brief MoveTailBackward
     * Move the tail index backward one element
     */
    void MoveTailBackward(void)
    {
        if (m_u16Tail == 0) {
            m_u16Tail = m_u16Count;
        }
        m_u16Tail--;
    }

    /**
     * @brief MoveHeadBackward
     * Move the head index backward one element
     */
    void MoveHeadBackward(void)
    {
        if (m_u16Head == 0) {
            m_u16Head = m_u16Count;
        }
        m_u16Head--;
    }

    /**
     * @brief Send_i
     * Internal method which implements all Send() methods in the class.
     *
     * @param pvData_   Pointer to the envelope data
     * @param bTail_    true - write to tail, false - write to head
     * @param u32TimeoutMS_ Time to wait before timeout (in ms).
     * @return          true - data successfully written, false - buffer full
     */
    bool Send_i(const void* pvData_, bool bTail_, uint32_t u32TimeoutMS_);

    /**
     * @brief Receive_i
     * Internal method which implements all Read() methods in the class.
     *
     * @param pvData_       Pointer to the envelope data
     * @param bTail_        true - read from tail, false - read from head
     * @param u32WaitTimeMS_ Time to wait before timeout (in ms).
     * @return              true - read successfully, false - timeout.
     */
    bool Receive_i(void* pvData_, bool bTail_, uint32_t u32WaitTimeMS_);

    uint16_t m_u16Head; //!< Current head index
    uint16_t m_u16Tail; //!< Current tail index

    uint16_t          m_u16Count; //!< Count of items in the mailbox
    volatile uint16_t m_u16Free;  //!< Current number of free slots in the mailbox

    uint16_t    m_u16ElementSize; //!< Size of the objects tracked in this mailbox
    const void* m_pvBuffer;       //!< Pointer to the data-buffer managed by this mailbox

    Semaphore m_clRecvSem; //!< Counting semaphore used to synchronize threads on the object
    Semaphore m_clSendSem; //!< Binary semaphore for send-blocked threads.
};
} // namespace Mark3
