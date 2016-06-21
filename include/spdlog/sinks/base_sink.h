//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once
//
// base sink templated over a mutex (either dummy or realy)
// concrete implementation should only overrid the _sink_it method.
// all locking is taken care of here so no locking needed by the implementors..
//

#include <spdlog/sinks/sink.h>
#include <spdlog/formatter.h>
#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>

#include <mutex>

namespace spdlog
{
namespace sinks
{
template<class Mutex>
class base_sink:public sink
{
public:
    base_sink():_mutex(),_enabled_level_min(spdlog::level::trace),_enabled_level_max(spdlog::level::off) {}
    virtual ~base_sink() = default;

    base_sink(const base_sink&) = delete;
    base_sink& operator=(const base_sink&) = delete;

    void log(const details::log_msg& msg) override
    {
        if(should_sink_it(msg)){
            std::lock_guard<Mutex> lock(_mutex);
            _sink_it(msg);
        }
    }
    // set log_msg level for this shink. not thread-safe
    // defalut msg level : [spdlog::level::trace,spdlog::level::off]
    void set_level(level::level_enum min,level::level_enum max)override
    {
        _enabled_level_min=min;
        _enabled_level_max=max;
    }

    bool  should_sink_it(const details::log_msg& msg)const
    {
        return (msg.level >= _enabled_level_min)  && (msg.level <= _enabled_level_max);
    }
protected:
    virtual void _sink_it(const details::log_msg& msg) = 0;
    Mutex _mutex;
    spdlog::level::level_enum _enabled_level_min;
    spdlog::level::level_enum _enabled_level_max;
};
}
}
