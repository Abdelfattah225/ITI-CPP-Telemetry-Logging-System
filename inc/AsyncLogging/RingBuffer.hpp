#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <vector>
#include <optional>
#include <cstddef>

namespace async_logging
{

template <typename T>
class RingBuffer
{
private:
    std::vector<std::optional<T>> m_buffer;
    std::size_t m_head;
    std::size_t m_tail;
    std::size_t m_count;
    std::size_t m_capacity;

public:
    explicit RingBuffer(std::size_t capacity) 
        : m_buffer(capacity, std::nullopt)
        , m_head{0}
        , m_tail{0}
        , m_count{0}
        , m_capacity{capacity}
    {
    }

    RingBuffer(const RingBuffer& ref) = delete;
    RingBuffer& operator=(const RingBuffer& ref) = delete;

    RingBuffer(RingBuffer&& ref) = default;
    RingBuffer& operator=(RingBuffer&& ref) = default;

    ~RingBuffer() = default;

    bool tryPush(T item)
    {
        if (isFull())
        {
            return false;
        }
        m_buffer[m_tail] = std::move(item);
        m_tail = (m_tail + 1) % m_capacity;
        ++m_count;
        return true;
    }

    std::optional<T> tryPop()
    {
        if (isEmpty())
        {
            return std::nullopt;
        }
        T item = std::move(m_buffer[m_head].value());
        m_buffer[m_head] = std::nullopt;
        m_head = (m_head + 1) % m_capacity;
        --m_count;
        return item;
    }

    bool isEmpty() const
    {
        return m_count == 0;
    }

    bool isFull() const
    {
        return m_count == m_capacity;
    }

    std::size_t size() const
    {
        return m_count;
    }

    std::size_t capacity() const
    {
        return m_capacity;
    }
};

} // namespace async_logging

#endif // RINGBUFFER_HPP