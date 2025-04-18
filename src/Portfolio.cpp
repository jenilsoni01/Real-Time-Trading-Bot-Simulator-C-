#include "../include/Portfolio.hpp"
#include "../include/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace trading {

void Portfolio::processSignal(const Signal& signal, const Tick& currentTick) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& logger = Logger::getInstance();
    
    switch (signal.action) {
        case Action::BUY:
            if (!currentPosition_.isOpen) {
                executeEntry(currentTick);
                std::stringstream ss;
                ss << "Executed Buy @ " << std::fixed << std::setprecision(2) 
                   << currentTick.price;
                logger.log(ss.str());
            }
            break;
            
        case Action::SELL:
            if (currentPosition_.isOpen) {
                executeExit(currentTick);
                std::stringstream ss;
                ss << "Executed Sell @ " << std::fixed << std::setprecision(2) 
                   << currentTick.price << " — PnL: $" 
                   << std::fixed << std::setprecision(2) 
                   << trades_.back().pnl;
                logger.log(ss.str());
            }
            break;
            
        case Action::HOLD:
            break;
    }
}

Position Portfolio::getPosition() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentPosition_;
}

double Portfolio::getWinRate() const {
    if (trades_.empty()) {
        return 0.0;
    }
    return static_cast<double>(winningTrades_) / trades_.size();
}

void Portfolio::executeEntry(const Tick& tick) {
    currentPosition_.isOpen = true;
    currentPosition_.entryPrice = tick.price;
    currentPosition_.quantity = tradeSize_;
    currentPosition_.entryTime = tick.timestamp;
    
    cash_ -= (tick.price * tradeSize_);
}

void Portfolio::executeExit(const Tick& tick) {
    cash_ += (tick.price * currentPosition_.quantity);
    recordTrade(tick);
    
    currentPosition_.isOpen = false;
    currentPosition_.quantity = 0.0;
}

void Portfolio::recordTrade(const Tick& exitTick) {
    Trade trade;
    trade.entryTime = currentPosition_.entryTime;
    trade.exitTime = exitTick.timestamp;
    trade.entryPrice = currentPosition_.entryPrice;
    trade.exitPrice = exitTick.price;
    trade.quantity = currentPosition_.quantity;
    
    trade.pnl = (exitTick.price - currentPosition_.entryPrice) * trade.quantity;
    trade.isWin = trade.pnl > 0;
    
    if (trade.isWin) {
        winningTrades_++;
    }
    
    totalPnL_ += trade.pnl;
    trades_.push_back(trade);
}

} // namespace trading 