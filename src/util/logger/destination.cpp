#include "destination.h"
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <locale>
#include "level.h"
#include "logger.h"

namespace bl = boost::log;
namespace kw = bl::keywords;
using namespace josh::util::logger;


namespace {

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Level)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

/**Commonly set format to given boost::log sink.*/
template<typename SinkT>
void setFormat(SinkT& sink) {
    sink.set_formatter(bl::expressions::stream
        << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "[%Y-%m-%d %T.%f]")
        << " [" << severity.or_default(Level::Info) << "] "
        << bl::expressions::message
    );
}

} // ! anonymous namespace


/*************************************************************/
// FileDestination::Impl
/*************************************************************/
struct FileDestination::Impl {
    using BackendT = bl::sinks::text_file_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;

    boost::shared_ptr<BackendT> backend;
    boost::shared_ptr<SinkT> sink;
    bool isEnabled;

    Impl(std::string const& path) :
        backend(boost::make_shared<BackendT>(
            kw::file_name = path,
            kw::open_mode = std::ios::app | std::ios::out,
            kw::enable_final_rotation = false)),
        sink(boost::make_shared<SinkT>(backend)),
        isEnabled(false)
    {
        ::setFormat(*sink);
        sink->imbue(std::locale(".UTF-8"));
        bl::core::get()->add_sink(sink);
        isEnabled = true;
    }
};


/*************************************************************/
// FileDestination
/*************************************************************/
FileDestination::FileDestination(std::string const& path): impl(new Impl(path)) {}


FileDestination::~FileDestination() noexcept = default;


void FileDestination::enable(Logger& logger) {
    impl->sink->set_filter(
        bl::expressions::attr<std::string>("Tag") == logger.getName() &&
        bl::expressions::attr<Level>("Severity") <= severity);
    bl::core::get()->add_sink(impl->sink);
    impl->isEnabled = true;
}


void FileDestination::disable() {
    bl::core::get()->remove_sink(impl->sink);
    impl->isEnabled = false;
}


bool FileDestination::isEnabled() const {
    return impl->isEnabled;
}


void FileDestination::setFormat(std::string const& format) {
    ;
}


/*************************************************************/
// ConsoleDestination::Impl
/*************************************************************/
struct ConsoleDestination::Impl {
    ;
};


/*************************************************************/
// ConsoleDestination
/*************************************************************/
ConsoleDestination::~ConsoleDestination() noexcept = default;


void ConsoleDestination::enable(Logger& logger) {
    ;
}


void ConsoleDestination::disable() {
    ;
}


bool ConsoleDestination::isEnabled() const {
    return false;
}


void ConsoleDestination::setFormat(std::string const& format) {
    ;
}


/*************************************************************/
// StreamDestination::Impl
/*************************************************************/
struct StreamDestination::Impl {
    ;
};


/*************************************************************/
// StreamDestination
/*************************************************************/
StreamDestination::~StreamDestination() noexcept = default;


void StreamDestination::enable(Logger& logger) {
    ;
}


void StreamDestination::disable() {
    ;
}


bool StreamDestination::isEnabled() const {
    return false;
}


void StreamDestination::setFormat(std::string const& format) {
    ;
}


/*************************************************************/
// DebugOutputDestination::Impl
/*************************************************************/
struct DebugOutputDestination::Impl {
    ;
};


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

/*************************************************************/
// DebugOutputDestination
/*************************************************************/
DebugOutputDestination::~DebugOutputDestination() noexcept = default;


void DebugOutputDestination::enable(Logger& logger) {
    ;
}


void DebugOutputDestination::disable() {
    ;
}


bool DebugOutputDestination::isEnabled() const {
    return false;
}


void DebugOutputDestination::setFormat(std::string const& format) {
    ;
}

#endif // ! defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

