#include "../include/Strategy.hpp"
#include <numeric>

namespace trading {

Signal MovingAverageStrategy::processTick(const Tick& tick) {
    prices_.push_back(tick.price);
    
    // Keep only necessary price history
    while (prices_.size() > longPeriod_) {
        prices_.pop_front();
    }
    
    Signal signal;
    signal.timestamp = tick.timestamp;
    signal.action = Action::HOLD;
    
    // Wait until we have enough data
    if (prices_.size() < longPeriod_) {
        return signal;
    }
    
    // Calculate moving averages
    double shortMA = calculateMA(prices_, shortPeriod_);
    double longMA = calculateMA(prices_, longPeriod_);
    
    bool shortAboveLong = shortMA > longMA;
    
    // Generate signals on crossovers
    if (shortAboveLong && !lastShortAboveLong_) {
        signal.action = Action::BUY;
        signal.reason = "Short MA crossed above Long MA";
    }
    else if (!shortAboveLong && lastShortAboveLong_) {
        signal.action = Action::SELL;
        signal.reason = "Short MA crossed below Long MA";
    }
    
    lastShortAboveLong_ = shortAboveLong;
    return signal;
}

double MovingAverageStrategy::calculateMA(const std::deque<double>& prices, size_t period) const {
    if (prices.size() < period) {
        return 0.0;
    }
    
    auto start = prices.end() - period;
    auto sum = std::accumulate(start, prices.end(), 0.0);
    return sum / period;
}

std::unique_ptr<Strategy> createStrategy(const std::string& name) {
    if (name == "MovingAverage") {
        return std::make_unique<MovingAverageStrategy>();
    }
    throw std::runtime_error("Unknown strategy: " + name);
}

} // namespace trading 