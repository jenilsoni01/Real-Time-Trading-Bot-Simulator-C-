#pragma once

#include "Types.hpp"
#include <vector>
#include <deque>
#include <memory>

namespace trading {

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual Signal processTick(const Tick& tick) = 0;
    virtual std::string getName() const = 0;
};

class MovingAverageStrategy : public Strategy {
public:
    MovingAverageStrategy(size_t shortPeriod = 10, size_t longPeriod = 20)
        : shortPeriod_(shortPeriod)
        , longPeriod_(longPeriod) {}

    Signal processTick(const Tick& tick) override;
    std::string getName() const override { return "Moving Average Crossover"; }

private:
    double calculateMA(const std::deque<double>& prices, size_t period) const;
    
    size_t shortPeriod_;
    size_t longPeriod_;
    std::deque<double> prices_;
    bool lastShortAboveLong_{false};
};

// Factory function to create strategies
std::unique_ptr<Strategy> createStrategy(const std::string& name);

} // namespace trading 