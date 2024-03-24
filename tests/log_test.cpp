
// catch
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

// std
#include <future>
#include <iostream>

// to be tested
#include "core/log.h"

using namespace std::string_literals;
using namespace Catch;
using namespace Catch::Matchers;
using namespace openpiv::core;
using namespace openpiv::core::logger;
using namespace std::literals;

TEST_CASE("log_test - add and remove sink")
{
    auto& logger = Logger::instance();

    auto sink =
        [](Level, const std::string&) -> bool
        {
            return true;
        };
    auto id = logger.add_sink(std::move(sink));
    REQUIRE(logger.remove_sink(id) == true);
    REQUIRE(logger.remove_sink(++id) == false);
}

TEST_CASE("log_test - add message then sink")
{
    auto& logger = Logger::instance();
    logger.add(Level::TEST, "{}", 1);

    std::string msg;
    Level level;
    std::promise<bool> called;
    auto f = called.get_future();

    auto sink =
        [&msg, &level, &called](Level l, const std::string& m) -> bool
        {
            if ( l != Level::TEST )
                return false;

            msg = m;
            level = l;
            called.set_value(true);

            return true;
        };
    auto id = logger.add_sink(std::move(sink));

    REQUIRE(f.get());
    REQUIRE(level == Level::TEST);
    REQUIRE_THAT(msg, EndsWith("TEST: 1"s));

    logger.remove_sink(id);
}

TEST_CASE("log_test - add sink then message")
{
    auto& logger = Logger::instance();

    std::string msg;
    Level level;
    std::promise<bool> called;
    auto f = called.get_future();

    auto sink =
        [&msg, &level, &called](Level l, const std::string& m) -> bool
        {
            if ( l != Level::TEST )
                return false;

            std::cout << m << "\n";
            msg = m;
            level = l;
            called.set_value(true);

            return true;
        };
    auto id = logger.add_sink(std::move(sink));

    logger.add(Level::TEST, "{}", 2);

    REQUIRE(f.get());
    REQUIRE(level == Level::TEST);
    REQUIRE_THAT(msg, EndsWith("TEST: 2"s));
    logger.remove_sink(id);
}

TEST_CASE("log_test - test primitives")
{
    auto& logger = Logger::instance();

    std::string msg;
    Level level;
    std::promise<bool> called;
    auto f = called.get_future();

    auto sink =
        [&msg, &level, &called](Level l, const std::string& m) -> bool
        {
            if ( l != Level::TEST )
                return false;

            msg = m;
            level = l;
            called.set_value(true);

            return true;
        };
    auto id = logger.add_sink(std::move(sink));

    logger.add(Level::TEST, "{} {} {} {}", 1, 3.141, "foo!"sv, "bar"sv);

    REQUIRE(f.get());
    REQUIRE(level == Level::TEST);
    REQUIRE_THAT(msg, EndsWith("TEST: 1 3.141 foo! bar"s));
    logger.remove_sink(id);
}

TEST_CASE("log_test - add multiple messages")
{
    auto& logger = Logger::instance();

    std::vector<std::string> msgs;
    std::atomic<size_t> msg_count{};

    auto sink =
        [&msgs, &msg_count](Level l, const std::string& m) -> bool
        {
            if ( l != Level::TEST )
                return false;

            msgs.push_back(m);
            msg_count += 1;

            return true;
        };
    auto id = logger.add_sink(std::move(sink));

    const size_t count = 10;
    for (size_t i=0; i<count; ++i)
        logger.add(Level::TEST, "count: {}", count);

    while ( msg_count < count ) {}

    // check we got all of the messages
    REQUIRE(msgs.size() == count);

    // check that they're in order
    for (size_t i=0; i<count; ++i)
        REQUIRE_THAT(msgs[i], EndsWith(std::to_string(count)));

    logger.remove_sink(id);
}

