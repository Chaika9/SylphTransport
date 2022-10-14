#pragma once

#include "KapMirror/Runtime/ArraySegment.hpp"
#include "Platform.hpp"
#include <memory>
#include <queue>
#include <mutex>

namespace KapMirror {
namespace Sylph {
    class MagnificentSendPipe {
        private:
        std::queue<std::shared_ptr<ArraySegment<byte>>> queue;
        std::mutex queueMutex;

        public:
        void push(std::shared_ptr<ArraySegment<byte>> data) {
            std::lock_guard<std::mutex> lock(queueMutex);
            queue.push(data);
        }

        bool pop(std::shared_ptr<ArraySegment<byte>>& data) {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (queue.empty()) {
                return false;
            }

            auto entry = queue.front();
            queue.pop();

            data = entry;

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
