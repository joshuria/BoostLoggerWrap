#include "logger.h"
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <locale>
#include <codecvt>
#include <fstream>
#include <ostream>
#include <exception>
#include <ctime>
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
    std::vector<std::shared_ptr<IDestination>> destinationList;

#ifdef LOG_USE_MT
    blsrc::severity_logger_mt<Logger::Level> instance;
#else
    blsrc::severity_logger<Level> instance;
#endif


    LoggerImpl(
        Logger* container, std::string name, std::vector<std::shared_ptr<IDestination>>&& destinations
    ): container(container), name(name), destinationList(destinations), instance() {
        instance.add_attribute("Tag", bl::attributes::constant<std::string>(name));
        //fileBackend = boost::make_shared<boost::log::sinks::text_ostream_backend>();

        //fileSink = boost::make_shared<bl::sinks::synchronous_sink<bl::sinks::text_ostream_backend>>(fileBackend);
        //fileSink->set_formatter(bl::expressions::stream
        //    << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "[%Y-%m-%d %T.%f]")
        //    << " [" << severity.or_default(Level::Info) << "] "
        //    //<< " [" << std::setw(1) << boost::log::trivial::severity << std::setw(0) << "] "
        //    << bl::expressions::message
        //);
        //fileSink->imbue(std::locale(".UTF-8"));
        //bl::core::get()->add_sink(fileSink);

        //fileSink->set_filter([this](bl::attribute_value_set const& attrs) {
        //    //return attrs["Tag"].extract<std::string>() == settings.name;
        //    Level lv = *attrs["Severity"].extract<Level>();
        //    return attrs["Tag"].extract<std::string>() == settings.name &&
        //        static_cast<int>(lv) <= static_cast<int>(settings.severity);
        //});

        ////fileSink->set_filter(
        ////    bl::expressions::attr<std::string>("Tag") == name &&
        ////    bl::expressions::attr<Level>("Severity") <= settings.severity);

        //auto fileStream = boost::make_shared<std::ofstream>(
        //    settings.logPath, StreamType::app | StreamType::binary);
        //if (!settings.isFileBuffering)
        //    fileStream->rdbuf()->pubsetbuf(nullptr, 0);
        //fileBackend->add_stream(fileStream);


        //consoleBackend = boost::make_shared<boost::log::sinks::text_ostream_backend>();

        //consoleSink = boost::make_shared<bl::sinks::synchronous_sink<bl::sinks::text_ostream_backend>>(consoleBackend);
        //consoleSink->set_formatter(bl::expressions::stream
        //    << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "[%Y-%m-%d %T.%f]")
        //    << " [" << severity.or_default(Level::Info) << "] "
        //    << bl::expressions::message
        //);
        //consoleSink->imbue(std::locale(".UTF-8"));
        //bl::core::get()->add_sink(consoleSink);

        //consoleSink->set_filter(
        //    bl::expressions::attr<std::string>("Tag") == name &&
        //    bl::expressions::attr<Level>("Severity") <= settings.severity);

        //auto consoleStream = boost::shared_ptr<StreamType>{ &std::clog, boost::null_deleter() };
        //if (!settings.isConsoleBuffering)
        //    consoleStream->rdbuf()->pubsetbuf(nullptr, 0);
        //consoleBackend->add_stream(consoleStream);

        bl::add_common_attributes();
    }
};


/*************************************************************/
// Logger
/*************************************************************/
Logger::Logger(std::string name, std::vector<std::shared_ptr<IDestination>>&& destinations) : impl() {
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

