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

/**Macro for general implementation.*/
#define IMPL_TEMPLATE(ClassName) \
    ClassName::~ClassName() noexcept = default;                             \
    void ClassName::enable(Logger& logger) { impl->base.enable(logger); }   \
    void ClassName::disable() { impl->base.disable(); }                     \
    bool ClassName::isEnabled() const { return impl->base.isEnabled(); }

} // ! anonymous namespace


/*************************************************************/
// DestinationBase
/*************************************************************/
DestinationBase::DestinationBase(Level severity): severity(severity) {}


void DestinationBase::setFormat(std::string const& format) { }


/*************************************************************/
// FileDestination::Impl
/*************************************************************/
struct FileDestination::Impl {
    using BackendT = bl::sinks::text_file_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;
    using ContainerT = FileDestination;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner, std::string const& path) :
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
FileDestination::FileDestination(std::string const& path, Level severity)
    : DestinationBase(severity), impl(new Impl(this, path)) {}


IMPL_TEMPLATE(FileDestination)


/*************************************************************/
// FileDestinationAsync::Impl
/*************************************************************/
struct FileDestinationAsync::Impl {
    using BackendT = bl::sinks::text_file_backend;
    using SinkT = bl::sinks::asynchronous_sink<BackendT>;
    using ContainerT = FileDestinationAsync;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner, std::string const& path) :
        backend(boost::make_shared<BackendT>(
            kw::file_name = path,
            kw::open_mode = std::ios::app | std::ios::out,
            kw::enable_final_rotation = false)),
        base(backend, owner)
    { }
};


/*************************************************************/
// FileDestinationAsync
/*************************************************************/
FileDestinationAsync::FileDestinationAsync(std::string const& path, Level severity)
    : DestinationBase(severity), impl(new Impl(this, path)) {}


IMPL_TEMPLATE(FileDestinationAsync)


/*************************************************************/
// ConsoleDestination::Impl
/*************************************************************/
struct ConsoleDestination::Impl {
    using BackendT = bl::sinks::text_ostream_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;
    using ContainerT = ConsoleDestination;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    {
        backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    }
};


/*************************************************************/
// ConsoleDestination
/*************************************************************/
ConsoleDestination::ConsoleDestination(Level severity)
    : DestinationBase(severity), impl(new Impl(this)) { }


IMPL_TEMPLATE(ConsoleDestination)


/*************************************************************/
// ConsoleDestinationAsync::Impl
/*************************************************************/
struct ConsoleDestinationAsync::Impl {
    using BackendT = bl::sinks::text_ostream_backend;
    using SinkT = bl::sinks::asynchronous_sink<BackendT>;
    using ContainerT = ConsoleDestinationAsync;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    {
        backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    }
};


/*************************************************************/
// ConsoleDestinationAsync
/*************************************************************/
ConsoleDestinationAsync::ConsoleDestinationAsync(Level severity)
    : DestinationBase(severity), impl(new Impl(this)) { }


IMPL_TEMPLATE(ConsoleDestinationAsync)


/*************************************************************/
// StreamDestination::Impl
/*************************************************************/
struct StreamDestination::Impl {
    using BackendT = bl::sinks::text_ostream_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;
    using ContainerT = StreamDestination;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner, std::ostream* stream, bool deleteStreamLater) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    {
        if (deleteStreamLater)
            backend->add_stream(boost::shared_ptr<std::ostream>(stream));
        else
            backend->add_stream(boost::shared_ptr<std::ostream>(stream, boost::null_deleter()));
    }
};


/*************************************************************/
// StreamDestination
/*************************************************************/
StreamDestination::StreamDestination(
    std::ostream* stream, bool deleteStreamWhenDestroy, Level severity
) : DestinationBase(severity), impl(new Impl(this, stream, deleteStreamWhenDestroy)) { }


IMPL_TEMPLATE(StreamDestination)


/*************************************************************/
// StreamDestinationAsync::Impl
/*************************************************************/
struct StreamDestinationAsync::Impl {
    using BackendT = bl::sinks::text_ostream_backend;
    using SinkT = bl::sinks::asynchronous_sink<BackendT>;
    using ContainerT = StreamDestinationAsync;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner, std::ostream* stream, bool deleteStreamLater) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    {
        if (deleteStreamLater)
            backend->add_stream(boost::shared_ptr<std::ostream>(stream));
        else
            backend->add_stream(boost::shared_ptr<std::ostream>(stream, boost::null_deleter()));
    }
};


/*************************************************************/
// StreamDestinationAsync
/*************************************************************/
StreamDestinationAsync::StreamDestinationAsync(
    std::ostream* stream, bool deleteStreamWhenDestroy, Level severity
) : DestinationBase(severity), impl(new Impl(this, stream, deleteStreamWhenDestroy)) { }


IMPL_TEMPLATE(StreamDestinationAsync)


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

/*************************************************************/
// DebugOutputDestination::Impl
/*************************************************************/
struct DebugOutputDestination::Impl {
    using BackendT = bl::sinks::debug_output_backend;
    using SinkT = bl::sinks::synchronous_sink<BackendT>;
    using ContainerT = DebugOutputDestination;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    { }
};


/*************************************************************/
// DebugOutputDestination
/*************************************************************/
DebugOutputDestination::DebugOutputDestination(Level severity)
    : DestinationBase(severity), impl(new Impl(this)) {}


IMPL_TEMPLATE(DebugOutputDestination)


/*************************************************************/
// DebugOutputDestinationAsync::Impl
/*************************************************************/
struct DebugOutputDestinationAsync::Impl {
    using BackendT = bl::sinks::debug_output_backend;
    using SinkT = bl::sinks::asynchronous_sink<BackendT>;
    using ContainerT = DebugOutputDestinationAsync;

    boost::shared_ptr<BackendT> backend;
    ImplBase<BackendT, SinkT, ContainerT> base;

    Impl(ContainerT* owner) :
        backend(boost::make_shared<BackendT>()),
        base(backend, owner)
    { }
};


/*************************************************************/
// DebugOutputDestinationAsync
/*************************************************************/
DebugOutputDestinationAsync::DebugOutputDestinationAsync(Level severity)
    : DestinationBase(severity), impl(new Impl(this)) {}


IMPL_TEMPLATE(DebugOutputDestinationAsync)

#endif // ! defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

