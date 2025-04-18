#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace trading {

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void init(const std::string& logFile = "trading_bot.log") {
        std::lock_guard<std::mutex> lock(mutex_);
        outFile_.open(logFile, std::ios::out | std::ios::trunc);
    }

    template<typename T>
    void log(const T& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::system_clock::now();
        auto timestamp = formatTimestamp(now);
        
        std::stringstream ss;
        ss << "[" << timestamp << "] " << message;
        
        std::cout << ss.str() << std::endl;
        if (outFile_.is_open()) {
            outFile_ << ss.str() << std::endl;
        }
    }

    ~Logger() {
        if (outFile_.is_open()) {
            outFile_.close();
        }
    }

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string formatTimestamp(const std::chrono::system_clock::time_point& time) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            time.time_since_epoch()
        ).count() % 1000;
        
        auto timer = std::chrono::system_clock::to_time_t(time);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&timer), "%H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms;
        return ss.str();
    }

    std::mutex mutex_;
    std::ofstream outFile_;
};

} // namespace trading 