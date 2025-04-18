#include "../include/MarketFeed.hpp"
#include "../include/Logger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace trading {

MarketFeed::MarketFeed(const std::string& dataFile)
    : dataFile_(dataFile) {
    loadData();
}

MarketFeed::~MarketFeed() {
    stop();
}

void MarketFeed::start() {
    if (!running_) {
        running_ = true;
        streamThread_ = std::thread(&MarketFeed::streamingThread, this);
    }
}

void MarketFeed::stop() {
    if (running_) {
        running_ = false;
        cv_.notify_one();
        if (streamThread_.joinable()) {
            streamThread_.join();
        }
    }
}

void MarketFeed::registerCallback(TickCallback callback) {
    callback_ = std::move(callback);
}

void MarketFeed::loadData() {
    std::ifstream file(dataFile_);
    if (!file) {
        throw std::runtime_error("Could not open data file: " + dataFile_);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string timestamp;
        std::string price;
        std::string volume;

        if (std::getline(ss, timestamp, ',') &&
            std::getline(ss, price, ',') &&
            std::getline(ss, volume, ',')) {
            
            Tick tick;
            tick.timestamp = parseTimestamp(timestamp);
            tick.price = std::stod(price);
            tick.volume = std::stod(volume);
            
            tickQueue_.push(tick);
        }
    }

    if (tickQueue_.empty()) {
        throw std::runtime_error("No valid tick data found in file");
    }
}

void MarketFeed::streamingThread() {
    auto& logger = Logger::getInstance();
    logger.log("Market feed started");

    while (running_ && !tickQueue_.empty()) {
        auto start = std::chrono::high_resolution_clock::now();
        
        processNextTick();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
        
        // Update average latency
        avgLatency_ = (avgLatency_ * tickCount_ + latency) / (tickCount_ + 1);
        tickCount_++;
        
        // Simulate real-time delay between ticks
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    logger.log("Market feed stopped");
}

void MarketFeed::processNextTick() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!tickQueue_.empty() && callback_) {
        Tick tick = tickQueue_.front();
        tickQueue_.pop();
        lock.unlock();
        
        callback_(tick);
    }
}

TimePoint MarketFeed::parseTimestamp(const std::string& timestamp) {
    std::tm tm = {};
    std::stringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse timestamp: " + timestamp);
    }
    
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    // Parse milliseconds if present
    if (ss.peek() == '.') {
        int ms;
        ss.ignore() >> ms;
        timePoint += std::chrono::milliseconds(ms);
    }
    
    return timePoint;
}

} // namespace trading 