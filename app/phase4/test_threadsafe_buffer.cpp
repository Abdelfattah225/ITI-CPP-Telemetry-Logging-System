#include "inc/AsyncLogging/ThreadSafeRingBuffer.hpp"
#include <iostream>
#include <thread>
#include <vector>

int main()
{
    async_logging::ThreadSafeRingBuffer<int> buffer(5);

    // Producer thread: pushes 10 items
    std::thread producer([&buffer]() {
        for (int i = 1; i <= 10; ++i)
        {
            std::cout << "[Producer] Pushing: " << i << std::endl;
            buffer.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "[Producer] Done, stopping buffer" << std::endl;
        buffer.stop();
    });

    // Consumer thread: pops until stopped
    std::thread consumer([&buffer]() {
        while (true)
        {
            auto item = buffer.pop();
            if (!item.has_value())
            {
                std::cout << "[Consumer] Buffer stopped, exiting" << std::endl;
                break;
            }
            std::cout << "[Consumer] Popped: " << item.value() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    producer.join();
    consumer.join();

    std::cout << "All threads finished!" << std::endl;
    return 0;
}