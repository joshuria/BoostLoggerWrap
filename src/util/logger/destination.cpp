#include "destination.h"
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/core/null_deleter.hpp>
#include <locale>
#include <iostream>

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

template<typename BackendT, typename SinkT, typename DestinationT>
struct ImplBase {
    boost::shared_ptr<SinkT> sink;
    bool enabled;
    DestinationT* container;

    ImplBase(boost::shared_ptr<BackendT> backend, DestinationT* container)
        : sink(boost::make_shared<SinkT>(backend)), enabled(false), container(container)
    {
        ::setFormat(*sink);
        sink->imbue(std::locale(".UTF-8"));
        bl::core::get()->add_sink(sink);
    }

    void enable(Logger& logger) {
        sink->set_filter([this, &logger](bl::attribute_value_set const& attrs) {
            return attrs["Tag"].extract<std::string>() == logger.getName() &&
                static_cast<int>(*attrs["Severity"].extract<Level>()) <= static_cast<int>(container->severity);
        });
        bl::core::get()->add_sink(sink);
        enabled = true;
    }

    void disable() {
        bl::core::get()->remove_sink(sink);
        enabled = false;
    }

    bool isEnabled() const {
        return enabled;
    }
};

} // ! anonymous namespace


/*************************************************************/
// FileDestination::Impl
/*************************************************************/
struct FileDestination::Impl {
    using BackendT = bl::sinks::text_file_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, FileDestination> base;

    Impl(FileDestination* owner, std::string const& path) :
        backend(boost::make_shared<BackendT>(
            kw::file_name = path,
            kw::open_mode = std::ios::app | std::ios::out,
            kw::enable_final_rotation = false)),
        base(backend, owner)
    { }
};


/*************************************************************/
// FileDestination
/*************************************************************/
FileDestination::FileDestination(std::string const& path)
    : impl(new Impl(this, path)) {}


FileDestination::~FileDestination() noexcept = default;


void FileDestination::enable(Logger& logger) {
    impl->base.enable(logger);
}


void FileDestination::disable() {
    impl->base.disable();
}


bool FileDestination::isEnabled() const {
    return impl->base.isEnabled();
}


void FileDestination::setFormat(std::string const& format) {
    ;
}


/*************************************************************/
// ConsoleDestination::Impl
/*************************************************************/
struct ConsoleDestination::Impl {
    using BackendT = bl::sinks::text_ostream_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ConsoleDestination> base;

    Impl(ConsoleDestination* owner) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    {
        backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    }
};


/*************************************************************/
// ConsoleDestination
/*************************************************************/
ConsoleDestination::ConsoleDestination(): impl(new Impl(this)) { }


ConsoleDestination::~ConsoleDestination() noexcept = default;


void ConsoleDestination::enable(Logger& logger) {
    impl->base.enable(logger);
}


void ConsoleDestination::disable() {
    impl->base.disable();
}


bool ConsoleDestination::isEnabled() const {
    return impl->base.isEnabled();
}


void ConsoleDestination::setFormat(std::string const& format) {
    ;
}


/*************************************************************/
// StreamDestination::Impl
/*************************************************************/
struct StreamDestination::Impl {
    using BackendT = bl::sinks::text_ostream_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, StreamDestination> base;

    Impl(StreamDestination* owner, std::ostream* stream) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    {
        backend->add_stream(boost::shared_ptr<std::ostream>(stream));
    }
};


/*************************************************************/
// StreamDestination
/*************************************************************/
StreamDestination::StreamDestination(std::ostream* stream): impl(new Impl(this, stream)) { }


StreamDestination::~StreamDestination() noexcept = default;


void StreamDestination::enable(Logger& logger) {
    impl->base.enable(logger);
}


void StreamDestination::disable() {
    impl->base.disable();
}


bool StreamDestination::isEnabled() const {
    return impl->base.isEnabled();
}


void StreamDestination::setFormat(std::string const& format) {
    ;
}


/*************************************************************/
// DebugOutputDestination::Impl
/*************************************************************/
struct DebugOutputDestination::Impl {
    using BackendT = bl::sinks::debug_output_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, DebugOutputDestination> base;

    Impl(DebugOutputDestination* owner) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    { }
};


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

/*************************************************************/
// DebugOutputDestination
/*************************************************************/
DebugOutputDestination::DebugOutputDestination(): impl(new Impl(this)) {}


DebugOutputDestination::~DebugOutputDestination() noexcept = default;


void DebugOutputDestination::enable(Logger& logger) {
    impl->base.enable(logger);
}


void DebugOutputDestination::disable() {
    impl->base.disable();
}


bool DebugOutputDestination::isEnabled() const {
    return impl->base.isEnabled();
}


void DebugOutputDestination::setFormat(std::string const& format) {
    ;
}

#endif // ! defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

