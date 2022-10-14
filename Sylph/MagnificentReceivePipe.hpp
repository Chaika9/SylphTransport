#pragma once

#include "KapMirror/Runtime/ArraySegment.hpp"
#include "Platform.hpp"
#include <memory>
#include <queue>
#include <mutex>

namespace KapMirror {
namespace Sylph {
    class MagnificentReceivePipe {
        public:
        enum EventType {
            Connected,
            Data,
            Disconnected
        };

        struct Entry {
            int connectionId;
            EventType eventType;
            std::shared_ptr<ArraySegment<byte>> data;
        };

        private:
        std::queue<Entry> queue;
        std::mutex queueMutex;

        public:
        void push(int connectionId, EventType eventType, std::shared_ptr<ArraySegment<byte>> data) {
            std::lock_guard<std::mutex> lock(queueMutex);
            queue.push({connectionId, eventType, data});
        }

        void push(int connectionId, EventType eventType) {
            std::lock_guard<std::mutex> lock(queueMutex);
            queue.push({connectionId, eventType, nullptr});
        }

        bool pop(int& connectionId, EventType& eventType, std::shared_ptr<ArraySegment<byte>>& data) {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (queue.empty()) {
                return false;
            }

            auto entry = queue.front();
            queue.pop();

            connectionId = entry.connectionId;
            eventType = entry.eventType;
            data = entry.data;

            return true;
        }

        int getSize() {
            std::lock_guard<std::mutex> lock(queueMutex);
            return queue.size();
        }

        bool isEmpty() {
            std::lock_guard<std::mutex> lock(queueMutex);
            return queue.empty();
        }

        void clear() {
            std::lock_guard<std::mutex> lock(queueMutex);
            while (!queue.empty()) {
                queue.pop();
            }
        }
    };
}
}
