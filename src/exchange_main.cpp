#include <array>
#include <iostream>
#include <span>

#include "matching/matching_engine.h"

int main()
{
    using namespace exchange;

    const auto symbol = core::make_symbol("AAPL");
    std::array<core::Symbol, 1> symbols{symbol};

    matching::MatchingEngine<> engine;
    engine.prepare_symbols(std::span<const core::Symbol>{symbols});

    matching::InboundOrder buy_order{};
    buy_order.action = matching::RequestAction::ADD;
    buy_order.symbol = symbol;
    buy_order.side = core::Side::BUY;
    buy_order.order_id = 1;
    buy_order.price = 10000;
    buy_order.qty = 10;
    buy_order.order_type = core::OrderType::LIMIT;
    buy_order.timestamp = 1;
    buy_order.participant_id = 42;

    const auto events = engine.process_order(buy_order);
    std::cout << "Exchange Engine Started!\n";
    std::cout << "Events generated: " << events.size() << "\n";
    return 0;
}