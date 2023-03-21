#pragma once
#include <string>

template <typename T>
class UpdateQueue{
    
public:
    using Queue = std::vector<T>;
    UpdateQueue() = default;

    void emplace(T&& update) {
        queue.emplace_back(update);
    }
    void  swap(Queue& other_queue) {
        std::swap(queue, other_queue);
    }
    Queue queue;
};