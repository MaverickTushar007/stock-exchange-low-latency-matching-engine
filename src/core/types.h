// VOCABULARY EACH FILE WILL BE USING...

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace exchange::core
{

    using Price = std::int64_t; // price * 10^4
    using Quantity = std::uint32_t;
    using OrderId = std::uint64_t;
    using SequenceNumber = std::uint64_t; // used for every state changing Event
    using Timestamp = std::uint64_t;
    using ParticipantId = std::uint32_t;
    using MatchId = std::uint64_t; // identifies one buyer-seller Pairing

    inline constexpr Price PRICE_SCALE = 10'000;

    enum class Side : std::uint8_t
    { // 8bits = 1byte only!!!! otherwise it would have been 4 bytes
        BUY,
        SELL,
    };

    // BID: highest price someone is willing to BUY at!!
    // ASK: lowest price someone is willing to SELL at!!
    // SPREAD: Ask-Bid!!

    enum class OrderType : std::uint8_t
    {
        LIMIT, // adds a limit to the price
        MARKET,
        IOC,  // Immediate-OR-Cancel: fill now and cancel the leftover. Whatever is available is Taken right at the moment, and the share which were not taken are immediately cancelled!
        FOK,  // Fill-Or-Kill: more strict than IOC, there are no partial fills!!!
        GTC,  // Good_Till_Cancel: tells broker to keep pending buy/sell orders active until its completely filled or you manually cancel it!
        STOP, // StopLoss! or a trigger price
        STOP_LIMIT,
        ICEBERG,   // hides the bulk orders
        POST_ONLY, // a limit trade that adds liquidity to an orderBook, Prevents players to pay Taker Fee
    };

    enum class OrderStatus : std::uint8_t
    {
        NEW,
        ACCEPTED,
        PARTIALLY_FILED,
        PARTIALLY_FILLED = PARTIALLY_FILED,
        FILLED,
        CANCELED,
    };

    using Symbol = std::array<char, 8>; // 8 byte Array. >>> this basically stores Tickers.
    // we didnt use string cause it carries meta data as well, and memory allocation is in heap, causing it a huge no of 24 byte just to hold 4 bytes. & there is no heap allocation while using array!!!

    constexpr Symbol make_symbol(std::string_view text) noexcept
    {
        Symbol symbol{};
        const auto length = std::min(symbol.size(), text.size());
        for (std::size_t index = 0; index < length; index++)
        {
            symbol[index] = text[index];
        }
        return symbol;
    }

    constexpr std::string_view symbol_view(const Symbol &symbol) noexcept
    {
        std::size_t length = 0;
        while (length < symbol.size() && symbol[length] != '\0')
        {
            ++length;
        }
        return std::string_view(symbol.data(), length);
    }

    struct SymbolLess
    {
        constexpr bool operator()(const Symbol &lhs,
                                  const Symbol &rhs) const noexcept
        {
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end());
        }
    };

    /*
    Why need this?? ---> cause trivially_copyable types can be directly store directly in a fixed SPSC ring Buffer

    In C++, static assets typically refer to data (like textures, models, or particle systems) managed via static class
    members or static functions to ensure a single shared instance across all class objects.


    */
    static_assert(std::is_trivially_copyable<Price>::value,
                  "Price must be trivially copyable");
    static_assert(std::is_trivially_copyable<Quantity>::value,
                  "Quantity must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderId>::value,
                  "OrderId must be trivially copyable");
    static_assert(std::is_trivially_copyable<SequenceNumber>::value,
                  "SequenceNumber must be trivially copyable");
    static_assert(std::is_trivially_copyable<Timestamp>::value,
                  "Timestamp must be trivially copyable");
    static_assert(std::is_trivially_copyable<MatchId>::value,
                  "MatchId must be trivially copyable");
    static_assert(std::is_trivially_copyable<Side>::value,
                  "Side must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderType>::value,
                  "OrderType must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderStatus>::value,
                  "OrderStatus must be trivially copyable");
    static_assert(std::is_trivially_copyable<Symbol>::value,
                  "Symbol must be trivially copyable");

}