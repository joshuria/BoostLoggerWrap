#include "logger.h"
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/logger.hpp>
#include <locale>
#include <codecvt>
#include <exception>
#include <shared_mutex>
#include "../text.h"
#include "destination.h"

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>        // OutputDebugStringA
#endif

namespace bl = boost::log;
namespace blsrc = bl::sources;

using namespace josh::util::logger;


/*************************************************************/
// Logger::Impl
/*************************************************************/
struct Logger::LoggerImpl {
    Logger* container;
    std::string name;
    std::vector<std::shared_ptr<DestinationBase>> destinationList;

#ifdef LOG_USE_MT
    blsrc::severity_logger_mt<Logger::Level> instance;
#else
    blsrc::severity_logger<Level> instance;
#endif


    LoggerImpl(
        Logger* container, std::string name, std::vector<std::shared_ptr<DestinationBase>>&& destinations
    ): container(container), name(name), destinationList(destinations), instance() {
        instance.add_attribute("Tag", bl::attributes::constant<std::string>(name));
    }
};


/*************************************************************/
// Logger
/*************************************************************/
Logger::Logger(std::string name, std::vector<std::shared_ptr<DestinationBase>>&& destinations) : impl() {
    try {
        impl.reset(new LoggerImpl(this, std::move(name), std::move(destinations)));
    }
    catch (std::exception const& e) {
#ifdef _MSC_VER
        OutputDebugStringA("Fail to create logger: ");
        OutputDebugStringA(e.what());
#else
        std::cerr << "Fail to create logger: " << e.what();
#endif
    }
    for (auto dest: impl->destinationList)
        dest->enable(*this);
}


Logger::~Logger() noexcept = default;


std::string const& Logger::getName() const { return impl->name; }


void Logger::enable(bool enable) {
    if (!enable) disable();
    else {
        for (auto dest : impl->destinationList)
            dest->enable(*this);
    }
}


void Logger::disable() {
    for (auto dest : impl->destinationList) dest->disable();
}


void Logger::setSeverity(Level level) {
    for (auto dest : impl->destinationList)
        dest->severity = level;
}


bool Logger::isEnabled() const noexcept {
    return impl->destinationList[0]->isEnabled();
}


void Logger::write(Level level, std::string const& msg) const {
    try {
        BOOST_LOG_SEV(impl->instance, level) << msg;
    }
    catch (boost::io::format_error const& e) {
        BOOST_LOG_SEV(impl->instance, level) << e.what();
    }
}


void Logger::write(Level level, std::wstring const& msg) const {
    write(level, Text::toString(msg));
}


void Logger::write(Level level, boost::format & fmt) const {
    try {
        BOOST_LOG_SEV(impl->instance, level) << fmt.str();
    }
    catch (boost::io::format_error const& e) {
        BOOST_LOG_SEV(impl->instance, level) << e.what();
    }
}


void Logger::write(Level level, boost::wformat & fmt) const {
    write(level, fmt.str());
}

