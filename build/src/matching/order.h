/*
OrderBook Inner Layer
eg.
Price = 150.20$,
Qt = 100,
Order_count = 3;

so it broken down as:

FIFO Queue (Oldest at head and the newest at the tail)
head --->   order A: qty = 40; ts = 1001;
            order B: qty = 35; ts = 1002;
            order C: qty = 25; ts = 1003; <--- tail

Two important metrics:
    1. Price Priority
    2. Time Priority

*/

#pragma once

#include <cstddef>
#include "core/types.h"
#include <type_traits>

namespace exchange::matching
{
    struct Level;
}
namespace exchange::core
{
    struct alignas(64) Order
    {
        core::OrderId id{0};
        core::Side side{core::Side::BUY};
        core::Price price{0};
        core::Quantity qty{0};
        core::Quantity original_qty{0};
        core::Quantity display_qty{0};
        core::Quantity hidden_qty{0};
        core::Timestamp timestamp{0};
        core::OrderType type{core::OrderType::LIMIT};
        core::OrderStatus status{core::OrderStatus::NEW};
        core::ParticipantId participant_id{0};
        core::Price trigger_price{0};
        core::Quantity peak_qty{0};

        // now, every order we execute is doublyQueued with each other.
        Order *prev{nullptr};
        Order *next{nullptr};
        exchange::matching::Level *parent_level{nullptr};

        [[nodiscard]] bool is_stop_order() const noexcept
        {
            return type == core::OrderType::STOP || type == core::OrderType::STOP_LIMIT;
        }

        /*
            Matching Engine asks a few Questions:
            1. Should this order be allowed to rest on the book? (LIMIT, GTC, Icerberg, PostOnly, stoplimit - only these are allowed.)
            2. Should this order be allowed to cross immediately (Post Only is rejected here)
            3. is this a stop order?
            4. is this fill or Kill?
        */
    };

    static_assert(sizeof(Order) <= 128,
                  "Order should stay compact enough for cache-friendly access.");
    static_assert(std::is_standard_layout<Order>::value,
                  "Order must be a standard-layout type.");
}