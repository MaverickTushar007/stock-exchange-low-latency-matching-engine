# Stock Exchange

A lightweight C++ matching-engine prototype for an exchange-style order book. The project is structured around a core limit-order-book implementation, a symbol-aware matching engine, and a small set of executable entry points used for smoke testing and future trading terminal work.

This repository is not a full brokerage or live-market system; it is a codebase for experimenting with exchange primitives, price/quantity matching, event generation, and book-state logic in a compact C++20 design.

## What is in the project

The codebase currently includes:

- a price and symbol type system under `src/core`
- a memory pool allocator for preallocated, cache-friendly order storage
- a matching engine under `src/matching`
- a fixed-capacity order book with price levels and FIFO matching
- event generation for accepted, rejected, filled, canceled, and rested orders
- a simple executable driver in `src/exchange_main.cpp`
- placeholder executables for a trading terminal and a liquidity bot under `tools/`

## High-level design

The matching stack is organized as follows:

- `core/types.h` defines the exchange vocabulary: prices, quantities, symbols, order types, sides, and statuses.
- `core/memory_pool.h` provides a fixed-capacity object pool to avoid runtime heap churn for order and level objects.
- `matching/order.h` defines the per-order state used by the book.
- `matching/level.h` defines the price level structure and FIFO queue for orders at the same price.
- `matching/events.h` defines event payloads emitted during matching and order lifecycle changes.
- `matching/order_book.h` implements the matching logic and order book behavior.
- `matching/matching_engine.h` wraps the order book by symbol and exposes a simple API for inbound order requests.

## Supported mechanics

The current implementation includes the following exchange concepts:

- buy/sell order sides
- limit orders
- market orders
- stop and stop-limit handling
- iceberg-order display/hidden quantity logic
- FIFO matching within each price level
- price-time book behavior through price levels and linked order lists
- event output for accepted/rested/rejected/filled/canceled orders

## Current state

This is a working prototype rather than a production-grade exchange engine.

Notable current limitations:

- there is no network layer, websocket API, or persistent storage
- there is no real trading UI or market data feed
- the terminal and bot binaries are still minimal placeholders
- no automated test suite is present yet
- the engine is designed as a library/core to be expanded, not as a complete exchange system out of the box

## Build

Requirements:

- CMake 3.24+
- a modern C++20 compiler
- macOS/Linux environment is expected; the project currently targets C++20 with the standard CMake toolchain

From the project root:

```bash
cmake -S . -B build
cmake --build build --parallel
```

This produces the following binaries:

- `build/src/exchange`
- `build/tools/trader_terminal`
- `build/tools/liquidity_bot`

## Run

The sample executable can be launched with:

```bash
./build/src/exchange
```

It exercises a simple buy-order flow and prints a basic confirmation message plus the number of generated events.

## Project layout

```text
.
├── CMakeLists.txt
├── README.md
├── src/
│   ├── exchange_main.cpp
│   ├── core/
│   │   ├── clock.h
│   │   ├── memory_pool.h
│   │   ├── types.h
│   └── matching/
│       ├── events.h
│       ├── level.h
│       ├── matching_engine.h
│       ├── order.h
│       ├── order_book.h
├── tools/
│   ├── liquidity_bot.cpp
│   └── trader_terminal.cpp
└── build/
    └── generated CMake output
```

## Development notes

The code is intentionally structured to be cache-conscious and allocation-light. The order storage model relies on fixed-size pools and compact structures, which makes it a useful starting point for a C++-based low-latency matching engine.

If you plan to extend it, the most natural next steps are:

1. add a real market-data feed or inbound gateway
2. expand the terminal and bot entry points
3. add unit tests for order matching behavior
4. add validation for more exchange edge cases
5. turn the static engine into a multi-symbol, multi-participant simulator

## Bottom line

This project is a compact C++ exchange engine prototype focused on book matching, price levels, order lifecycle events, and low-churn allocation patterns. It is suitable as a learning project, a simulation base, and a starting point for building a more complete trading system.
