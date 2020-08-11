#include "logger.h"
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <locale>
#include <codecvt>
#include <fstream>
#include <ostream>
#include <exception>
#include <ctime>
#include <shared_mutex>
#include "../text.h"

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>        // OutputDebugStringA
#endif

namespace bl = boost::log;
namespace blsrc = bl::sources;

using namespace josh::util::logger;


namespace {

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Level)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

/**General logger settings.*/
struct LoggerSettings {
    std::string name;
    std::string logPath;
    bool isToFile;
    bool isToConsole;
    bool isFileBuffering;
    bool isConsoleBuffering;
    Level severity;
//#ifdef LOG_USE_MT
//    blsrc::severity_logger_mt<Logger::Level> instance;
//#else
//    blsrc::severity_logger<Logger::Level> instance;
//#endif

    LoggerSettings(
        std::string name, std::string path, bool toFile, bool toConsole,
        bool fileBufferedIO, bool consoleBufferedIO, Level severity
    ) : name(std::move(name)), logPath(std::move(path)), isToFile(toFile), isToConsole(toConsole),
        isFileBuffering(fileBufferedIO), isConsoleBuffering(consoleBufferedIO),
        severity(severity) {}
};

//! Global logger hash table
std::map<std::string, Logger*> LogTable;

//! Mutex for LogTable, used as ReadWriteLock
std::shared_mutex LogTableMutex;

//! Global settings
LoggerSettings LogGlobalSettings(
    "", "log.log",
    true, false, false, false,
    Level::Error);

} // ! anonymous namespace


namespace josh::util::logger {

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& operator<< (
    std::basic_ostream<CharT, Traits>& stream, Level lv
    ) {
    static constexpr char const* LevelMapping[] = { "T", "D", "I", "W", "E", "F" };
    return stream << LevelMapping[static_cast<int>(lv)];
}

} // ! namespace josh::util


/*************************************************************/
// Logger::Impl
/*************************************************************/
struct Logger::LoggerImpl {
    using StreamType = std::ostream;
    LoggerSettings settings;

    boost::shared_ptr<bl::sinks::text_ostream_backend> backend;
    boost::shared_ptr<std::ostream> consoleStream;
    boost::shared_ptr<std::ostream> fileStream;

    boost::shared_ptr<bl::sinks::synchronous_sink<bl::sinks::text_ostream_backend>> sink;
    std::unique_ptr<blsrc::severity_logger<Level>> logger;

    LoggerImpl(
        std::string name, std::string path,
        bool toFile = true, bool toConsole = false,
        bool fileBuffering = false, bool consoleBuffering = false,
        Level level = Level::Error
    ) : settings(name, path, toFile, toConsole, fileBuffering, consoleBuffering, level) {
        //backend = boost::make_shared<boost::log::sinks::text_ostream_backend>();
        //consoleStream = boost::shared_ptr<StreamType>{ &std::clog, boost::null_deleter() };
        //if (!settings.isConsoleBuffering)
        //    consoleStream->rdbuf()->pubsetbuf(nullptr, 0);

        //fileStream = boost::make_shared<std::ofstream>(
        //    settings.logPath, StreamType::app | StreamType::binary);
        //if (!settings.isFileBuffering)
        //    fileStream->rdbuf()->pubsetbuf(nullptr, 0);

        //if (settings.isToFile)
        //    backend->add_stream(fileStream);
        //if (settings.isToConsole)
        //    backend->add_stream(consoleStream);
        //backend->auto_flush(!settings.isFileBuffering);

        //sink = boost::make_shared<bl::sinks::synchronous_sink<bl::sinks::text_ostream_backend>>(backend);
        //// sink format
        //sink->set_formatter(bl::expressions::stream
        //    << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "[%Y-%m-%d %T.%f]")
        //    << " [" << severity.or_default(Level::Info) << "] "
        //    //<< " [" << std::setw(1) << boost::log::trivial::severity << std::setw(0) << "] "
        //    << bl::expressions::message
        //);
        //sink->imbue(std::locale(".UTF-8"));
        //bl::core::get()->add_sink(sink);
        //bl::add_common_attributes();

        //logger.reset(new blsrc::severity_logger<Level>());
    }
};


/*************************************************************/
// Logger
/*************************************************************/
Logger::Logger(
    std::string name, std::string path, bool toFile, bool toConsole,
    bool fileBuffered, bool consoleBuffered, Level severity
) : impl() {
    try {
        impl.reset(new LoggerImpl(
        std::move(name), std::move(path), toFile, toConsole,
        fileBuffered, consoleBuffered, severity));
    }
    catch (std::exception const& e) {
#ifdef _MSC_VER
        OutputDebugStringA("Fail to create logger: ");
        OutputDebugStringA(e.what());
#else
        std::cerr << "Fail to create logger: " << e.what();
#endif
    }
}


Logger::~Logger() noexcept = default;


std::string const& Logger::getLogPath() const noexcept { return impl->settings.logPath; }


bool Logger::isToFile() const noexcept { return impl->settings.isToFile; }


bool Logger::isToConsole() const noexcept { return impl->settings.isToConsole; }


bool Logger::isFileBuffered() const noexcept { return impl->settings.isFileBuffering; }


bool Logger::isConsoleBuffered() const noexcept { return impl->settings.isConsoleBuffering; }


void Logger::setSeverity(Level level) {
    impl->sink->set_filter([=] (bl::attribute_value_set const& attr) {
        return attr["severity"].extract<int>() > static_cast<int>(level);
    });
    impl->settings.severity = level;
}


Level Logger::getSeverity() const noexcept { return impl->settings.severity; }


void Logger::enableLog(bool enable) { bl::core::get()->set_logging_enabled(enable); }


bool Logger::isEnabled() const noexcept { return bl::core::get()->get_logging_enabled(); }


void Logger::write(Level level, std::string const& msg) const {
    try {
        BOOST_LOG_SEV(*impl->logger, level) << msg;
    }
    catch (boost::io::format_error const& e) {
        BOOST_LOG_SEV(*impl->logger, level) << e.what();
    }
}


void Logger::write(Level level, std::wstring const& msg) const {
    write(level, Text::toString(msg));
}


void Logger::write(Level level, boost::format & fmt) const {
    try {
        BOOST_LOG_SEV(*impl->logger, level) << fmt.str();
    }
    catch (boost::io::format_error const& e) {
        BOOST_LOG_SEV(*impl->logger, level) << e.what();
    }
}


void Logger::write(Level level, boost::wformat & fmt) const {
    write(level, fmt.str());
}

