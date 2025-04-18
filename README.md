# Real-Time Trading Bot Simulator

A C++ project that simulates a real-time trading bot using historical tick data. The bot operates in "paper trading" mode and logs signals, orders, and portfolio performance.

## Features

- Real-time market data simulation from CSV files
- Pluggable trading strategy system
- Moving Average Crossover strategy implementation
- Position tracking and PnL calculation
- Performance metrics and trade logging
- Thread-safe design with minimal latency

## Requirements

- C++17 compatible compiler
- Make build system
- POSIX-compliant operating system (Linux/macOS/WSL)

## Building the Project

```bash
# Clone the repository
git clone <repository-url>
cd trading-bot-simulator

# Build the project
make

# Clean build files
make clean
```

## Usage

1. Prepare your tick data CSV file in the format:
```
2023-01-01 09:00:00.000,30105.32,0.005
2023-01-01 09:00:00.100,30106.12,0.002
```

2. Run the trading bot:
```bash
./bin/trading_bot [path_to_csv_file]
```

If no CSV file is specified, it will look for `data/btc_usd_ticks.csv` by default.

## Configuration

The bot can be configured by modifying the following parameters in the source code:

- Initial capital: Set in `main.cpp`
- Trade size: Set in `main.cpp`
- Moving average periods: Set in `Strategy.hpp`

## Output

The bot generates two types of output:

1. Real-time console logging:
```
[09:00:00.000] Tick: 30105.32
[09:00:01.512] Signal: BUY
[09:00:01.520] Executed Buy @ 30105.32
```

2. Trading session summary:
```
=== Trading Session Summary ===
Total Trades: 56
Winning Trades: 38
Win Rate: 67.86%
Net PnL: $192.80
Average Tick Latency: 1.3ms
```

## Project Structure

- `include/` - Header files
  - `Types.hpp` - Common data structures
  - `MarketFeed.hpp` - Market data simulation
  - `Strategy.hpp` - Trading strategy interface
  - `Portfolio.hpp` - Position and PnL tracking
  - `Logger.hpp` - Logging system

- `src/` - Source files
  - `main.cpp` - Application entry point
  - Implementation files for each component

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

Jenil Soni 