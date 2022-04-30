
#pragma once

// std
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <limits>
#include <string>
#include <sstream>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>


// libs
#include <fmt/core.h>

// openpiv
#include "core/enum_helper.h"

namespace openpiv::core::logger {

    enum class Level : uint8_t {
        NONE    = 0,
        FATAL   = 1,
        ERROR   = 2,
        WARN    = 3,
        INFO    = 4,
        DEBUG   = 5,
        TEST    = std::numeric_limits<uint8_t>::max()
    };
    DECLARE_ENUM_HELPER( Level, {
            { Level::NONE,    "NONE"  },
            { Level::FATAL,   "FATAL" },
            { Level::ERROR,   "ERROR" },
            { Level::WARN,    "WARN"  },
            { Level::INFO,    "INFO"  },
            { Level::DEBUG,   "DEBUG" },
            { Level::TEST,    "TEST"  },
    } )

    template <
        typename Clock,
        typename Duration>
    static std::string to_string(const std::chrono::time_point<Clock, Duration>& tp)
    {
        return std::to_string(tp.time_since_epoch().count());
    }

    template <typename T>
    static std::string to_string(const T& t)
    {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    }

    /// core logging class; can register an arbitrary number
    /// of sinks, all of which will be called (ultimately)
    /// on Logger::add()
    ///
    /// a singleton that is lazily initialized, and will hold log messages
    /// until at least one sink is available, up to a configurable
    /// maximum
    ///
    /// conversion to string is done in an independent loging thread,
    /// calls to Logger::add() should be cheap
    class Logger
    {
    public:
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        static Logger& instance()
        {
            static Logger static_log;
            return static_log;
        }

        template <typename... Ts>
        void add(Level level, const std::string& fmt, Ts&&... args)
        {
            if (stop)
                return;

            auto now = std::chrono::high_resolution_clock::now();
            auto tid = std::this_thread::get_id();
            auto entry =
                [level, now, tid, fmt, args=std::make_tuple(args...)]() mutable
                {
                    std::string output = fmt::format(
                        "[{}] ({}) {}: ", // timestamp, threadid, level
                        to_string(now),
                        to_string(tid),
                        to_string(level));
                    std::apply(
                        [&output, fmt](const Ts&... ts) {
                            output += fmt::format(fmt, to_string(ts)...);
                        },
                        args);

                    return std::make_tuple(level, output);
                };

            // insert into queue
            {
                std::unique_lock<std::mutex> lock(entry_mutex);
                entries.emplace_back(std::move(entry));
                if (entries.size() > max_entries)
                    entries.pop_front();
            }
            entry_condition.notify_one();
        }

        // sink
        using sink_t = std::function<bool(Level, const std::string&)>;
        using sink_id_t = size_t;
        sink_id_t add_sink(sink_t sink)
        {
            sink_id_t id{};
            {
                std::unique_lock<std::mutex> lock(entry_mutex);
                id = sink_id++;
                sinks.emplace(id, std::move(sink));
            }
            entry_condition.notify_one();

            return id;
        }

        bool remove_sink(sink_id_t id)
        {
            std::unique_lock<std::mutex> lock(entry_mutex);
            return sinks.erase(id) == 1;
        }

        // resize message log
        void set_max_entries_size(size_t s)
        {
            max_entries = s;
        }

    private:
        using entry_return_t = std::tuple<Level, std::string>;
        using entry_t = std::function<entry_return_t()>;
        std::mutex entry_mutex;
        std::condition_variable entry_condition;
        std::deque<entry_t> entries;
        std::atomic<size_t> max_entries{100};
        std::atomic<bool> stop{false};
        std::unordered_map<sink_id_t, sink_t> sinks;
        std::atomic<size_t> sink_id{0};
        std::thread logger_thread;

        Logger()
        {
            logger_thread = std::thread(
                [this](){
                    while (!this->stop)
                    {
                        std::deque<entry_t> local_entries;

                        // lock/unlock and wait
                        {
                            std::unique_lock<std::mutex> lock(this->entry_mutex);
                            this->entry_condition.wait(
                                lock,
                                [this](){
                                    return
                                        this->stop ||
                                        (!this->sinks.empty() && !this->entries.empty());
                                });

                            // okay, got something and somewhere to put it
                            // get everything as quickly as possible
                            while (!this->entries.empty())
                            {
                                local_entries.push_front(std::move(entries.front()));
                                entries.pop_front();
                            }
                        }

                        // process/push to sinks
                        for (const auto& entry : local_entries)
                        {
                            auto [level, log_line] = entry();
                            for (auto& [id, sink] : sinks)
                                sink(level, log_line);
                        }
                    }
                });
        }

        ~Logger()
        {
            stop = true;
            entry_condition.notify_one();
            logger_thread.join();
        }
    };

    template <typename... Ts>
    static void fatal(const std::string& fmt, Ts&&... args)
    {
        Logger::instance().add(Level::FATAL, fmt, std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    static void error(const std::string& fmt, Ts&&... args)
    {
        Logger::instance().add(Level::ERROR, fmt, std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    static void warn(const std::string& fmt, Ts&&... args)
    {
        Logger::instance().add(Level::WARN, fmt, std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    static void info(const std::string& fmt, Ts&&... args)
    {
        Logger::instance().add(Level::INFO, fmt, std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    static void debug(const std::string& fmt, Ts&&... args)
    {
        Logger::instance().add(Level::DEBUG, fmt, std::forward<Ts>(args)...);
    }

} //
