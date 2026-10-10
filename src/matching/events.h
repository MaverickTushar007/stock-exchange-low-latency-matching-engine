#pragma once

#include <cstdint>
#include <type_traits>

#include "core/types.h"

namespace exchange::matching
{

    using core::MatchId;
    using core::OrderId;
    using core::OrderType;
    using core::Price;
    using core::Quantity;
    using core::SequenceNumber;
    using core::Side;
    using core::Symbol;
    using core::Timestamp;

    enum class ReasonCode : std::uint16_t
    {
        None = 0,
        ZERO_QUANTITY,
        NEGATIVE_PRICE,
        EMPTY_BOOK,
        BOOK_EMPTY = EMPTY_BOOK,
        DUPLICATE_ORDER_ID,
        ORDER_NOT_FOUND,
        ORDER_NOT_ACTIVE,
        FOK_INSUFFICIENT_LIQUIDITY,
        POST_ONLY_WOULD_CROSS,
        INVALID_MODIFICATION,
        INVALID_ICEBERG_DISPLAY,
        ORDER_POOL_EXHAUSTED,
    };

    enum class EventType : std::uint8_t
    {
        ORDER_ACCEPTED,
        ORDER_RESTED,
        ORDER_REJECTED,
        ORDER_REDUCED,
        ORDER_PARTIALLY_FILLED,
        ORDER_FILLED,
        ORDER_CANCELED,
        TRADE,
    };

    struct OrderAccepted
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        Symbol symbol;
        Side side;
        Price price;
        Quantity qty;
        OrderType order_type;
    };

    struct OrderRested
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        Symbol symbol;
        Side side;
        Price price;
        Quantity qty;
    };

    struct OrderRejected
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        ReasonCode reason_code;
    };

    struct OrderReduced
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        Quantity new_qty;
    };

    struct OrderPartiallyFilled
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        Quantity filled_qty;
        Quantity remaining_qty;
        Price price;
    };

    struct OrderFilled
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        Quantity filled_qty;
        Price price;
    };

    struct OrderCanceled
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderId order_id;
        Quantity canceled_qty;
    };

    struct Trade
    {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        MatchId match_id;
        Symbol symbol;
        OrderId buy_order_id;
        OrderId sell_order_id;
        Price price;
        Quantity qty;
    };

    struct Event
    {
        EventType type{EventType::ORDER_REJECTED};

        union Payload
        {
            OrderAccepted order_accepted;
            OrderRested order_rested;
            OrderRejected order_rejected;
            OrderReduced order_reduced;
            OrderPartiallyFilled order_partially_filled;
            OrderFilled order_filled;
            OrderCanceled order_canceled;
            Trade trade;

        } payload{};

        static Event make(const OrderAccepted &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_ACCEPTED;
            event.payload.order_accepted = value;
            return event;
        }
        static Event make(const OrderRested &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_RESTED;
            event.payload.order_rested = value;
            return event;
        }
        static Event make(const OrderRejected &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_REJECTED;
            event.payload.order_rejected = value;
            return event;
        }
        static Event make(const OrderReduced &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_REDUCED;
            event.payload.order_reduced = value;
            return event;
        }
        static Event make(const OrderPartiallyFilled &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_PARTIALLY_FILLED;
            event.payload.order_partially_filled = value;
            return event;
        }
        static Event make(const OrderFilled &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_FILLED;
            event.payload.order_filled = value;
            return event;
        }
        static Event make(const OrderCanceled &value) noexcept
        {
            Event event{};
            event.type = EventType::ORDER_CANCELED;
            event.payload.order_canceled = value;
            return event;
        }
        static Event make(const Trade &value) noexcept
        {
            Event event{};
            event.type = EventType::TRADE;
            event.payload.trade = value;
            return event;
        }
    };

    static_assert(std::is_trivially_copyable<ReasonCode>::value,
                  "ReasonCode must be trivially copyable");
    static_assert(std::is_trivially_copyable<EventType>::value,
                  "EventType must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderAccepted>::value,
                  "OrderAccepted must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderRested>::value,
                  "OrderRested must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderRejected>::value,
                  "OrderRejected must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderReduced>::value,
                  "OrderReduced must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderPartiallyFilled>::value,
                  "OrderPartiallyFilled must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderFilled>::value,
                  "OrderFilled must be trivially copyable");
    static_assert(std::is_trivially_copyable<OrderCanceled>::value,
                  "OrderCanceled must be trivially copyable");
    static_assert(std::is_trivially_copyable<Trade>::value,
                  "Trade must be trivially copyable");
    static_assert(std::is_trivially_copyable<Event>::value,
                  "Event must be trivially copyable");
}
