#ifndef THREADSAFE_RINGBUFFER_HPP
#define THREADSAFE_RINGBUFFER_HPP

#include "RingBuffer.hpp"
#include <mutex>
#include <condition_variable>
#include <optional>

namespace async_logging
{

    template <typename T>
    class ThreadSafeRingBuffer
    {
    private:
        RingBuffer<T> m_buffer;
        mutable std::mutex m_mutex;
        std::condition_variable m_condNotEmpty;
        std::condition_variable m_condNotFull;
        bool m_stopped;

    public:
        explicit ThreadSafeRingBuffer(std::size_t capacity)
            : m_buffer{capacity}, m_stopped{false}
        {
        }

        ThreadSafeRingBuffer(const ThreadSafeRingBuffer &) = delete;
        ThreadSafeRingBuffer &operator=(const ThreadSafeRingBuffer &) = delete;

        ThreadSafeRingBuffer(ThreadSafeRingBuffer &&) = delete;
        ThreadSafeRingBuffer &operator=(ThreadSafeRingBuffer &&) = delete;

        ~ThreadSafeRingBuffer() = default;

        // TODO 1: Implement push()
        // - Lock the mutex
        // - Wait while buffer is full AND not stopped (use m_condNotFull.wait())
        // - If stopped, return false
        // - Push the item using m_buffer.tryPush()
        // - Notify one waiting consumer (m_condNotEmpty.notify_one())
        // - Return true
        bool push(T item)
        {
         std::unique_lock<std::mutex> lock(m_mutex);

            m_condNotFull.wait(lock, [this]{
                return !m_buffer.isFull() || m_stopped ;
            });

            if(m_stopped){
                return false;
            }

            m_buffer.tryPush(std::move(item));
            m_condNotEmpty.notify_one();

            return true;
        }

        // TODO 2: Implement pop()
        // - Lock the mutex
        // - Wait while buffer is empty AND not stopped (use m_condNotEmpty.wait())
        // - If stopped AND buffer is empty, return std::nullopt
        // - Pop and return the item using m_buffer.tryPop()
        // - Notify one waiting producer (m_condNotFull.notify_one())
        std::optional<T> pop()
        {
            
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condNotEmpty.wait(lock,[this]{
                return !m_buffer.isEmpty() || m_stopped ;
            });
            if(m_stopped && m_buffer.isEmpty()){
                return std::nullopt;
            }

            auto item =  m_buffer.tryPop();
            m_condNotFull.notify_one();
            return item;
        }

        // TODO 3: Implement stop()
        // - Lock the mutex
        // - Set m_stopped to true
        // - Notify ALL waiting threads (both conditions)
        void stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopped = true;
            m_condNotEmpty.notify_one();
            m_condNotFull.notify_one();
        }

        bool isStopped() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_stopped;
        }

        bool isEmpty() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_buffer.isEmpty();
        }

        std::size_t size() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_buffer.size();
        }
    };

} // namespace async_logging

#endif // THREADSAFE_RINGBUFFER_HPP