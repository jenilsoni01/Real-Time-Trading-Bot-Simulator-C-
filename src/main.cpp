#include "../include/MarketFeed.hpp"
#include "../include/Strategy.hpp"
#include "../include/Portfolio.hpp"
#include "../include/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <csignal>

namespace {
    volatile std::sig_atomic_t gRunning = 1;
}

void signalHandler(int) {
    gRunning = 0;
}

int main(int argc, char* argv[]) {
    using namespace trading;
    
    // Register signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);
    
    try {
        // Initialize components
        auto& logger = Logger::getInstance();
        logger.init("trading_bot.log");
        logger.log("Trading Bot Simulator Starting...");
        
        // Create market feed (default to sample data file if none provided)
        std::string dataFile = (argc > 1) ? argv[1] : "data/btc_usd_ticks.csv";
        MarketFeed feed(dataFile);
        
        // Create strategy and portfolio
        auto strategy = createStrategy("MovingAverage");
        Portfolio portfolio(100000.0, 1.0); // $100k initial capital, 1 BTC per trade
        
        // Setup market data handler
        feed.registerCallback([&](const Tick& tick) {
            logger.log("Tick: " + std::to_string(tick.price));
            
            // Generate trading signal
            auto signal = strategy->processTick(tick);
            
            if (signal.action != Action::HOLD) {
                logger.log("Signal: " + 
                    std::string(signal.action == Action::BUY ? "BUY" : "SELL") +
                    " - " + signal.reason);
                
                // Execute trade
                portfolio.processSignal(signal, tick);
            }
        });
        
        // Start simulation
        feed.start();
        
        // Wait for shutdown signal
        while (gRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Cleanup and print summary
        feed.stop();
        
        // Print final statistics
        std::stringstream summary;
        summary << "\n=== Trading Session Summary ===\n"
                << "Total Trades: " << portfolio.getTotalTrades() << "\n"
                << "Winning Trades: " << portfolio.getWinningTrades() << "\n"
                << "Win Rate: " << std::fixed << std::setprecision(2) 
                << (portfolio.getWinRate() * 100.0) << "%\n"
                << "Net PnL: $" << portfolio.getTotalPnL() << "\n"
                << "Final Cash: $" << portfolio.getCurrentCash() << "\n"
                << "Average Tick Latency: " << feed.getAverageLatency() << "ms\n"
                << "Total Ticks Processed: " << feed.getTickCount() << "\n";
        
        logger.log(summary.str());
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 