#pragma once

#include "Types.hpp"
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace trading {

class MarketFeed {
public:
    using TickCallback = std::function<void(const Tick&)>;

    MarketFeed(const std::string& dataFile);
    ~MarketFeed();

    // Start streaming data
    void start();
    
    // Stop streaming data
    void stop();
    
    // Register callback for tick data
    void registerCallback(TickCallback callback);
    
    // Get statistics
    double getAverageLatency() const { return avgLatency_; }
    size_t getTickCount() const { return tickCount_; }

private:
    void loadData();
    void streamingThread();
    void processNextTick();
    TimePoint parseTimestamp(const std::string& timestamp);

    std::string dataFile_;
    std::queue<Tick> tickQueue_;
    std::thread streamThread_;
    std::atomic<bool> running_{false};
    TickCallback callback_;
    
    // Synchronization
    std::mutex mutex_;
    std::condition_variable cv_;
    
    // Statistics
    std::atomic<double> avgLatency_{0.0};
    std::atomic<size_t> tickCount_{0};
};

} // namespace trading 