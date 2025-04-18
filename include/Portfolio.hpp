#pragma once

#include "Types.hpp"
#include <vector>
#include <mutex>

namespace trading {

class Portfolio {
public:
    Portfolio(double initialCash = 100000.0, double tradeSize = 1.0)
        : cash_(initialCash)
        , tradeSize_(tradeSize) {}

    // Execute a trade based on signal
    void processSignal(const Signal& signal, const Tick& currentTick);
    
    // Get current position
    Position getPosition() const;
    
    // Get performance metrics
    double getTotalPnL() const { return totalPnL_; }
    size_t getTotalTrades() const { return trades_.size(); }
    size_t getWinningTrades() const { return winningTrades_; }
    double getWinRate() const;
    double getCurrentCash() const { return cash_; }
    
    // Get trade history
    const std::vector<Trade>& getTrades() const { return trades_; }

private:
    void executeEntry(const Tick& tick);
    void executeExit(const Tick& tick);
    void recordTrade(const Tick& exitTick);

    double cash_;
    double tradeSize_;
    double totalPnL_{0.0};
    size_t winningTrades_{0};
    
    Position currentPosition_;
    std::vector<Trade> trades_;
    mutable std::mutex mutex_;
};

} // namespace trading 