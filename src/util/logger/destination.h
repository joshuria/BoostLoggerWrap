#pragma once
#ifndef _JOSH_UTIL_LOGGER_DESTINATION_H_
#define _JOSH_UTIL_LOGGER_DESTINATION_H_

#include <string>
#include <memory>
#include <ostream>
#include "level.h"


namespace josh::util::logger {

class Logger;

/**Interface of logger destination.*/
struct DestinationBase {
    static constexpr Level DefaultLevel = Level::Error;

    explicit DestinationBase(Level severity);

    virtual ~DestinationBase() noexcept = default;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) = 0;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() = 0;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const = 0;

    /**Log severity.
     Severity greater than this value will be filtered. */
    Level severity;

protected:
    /**Set log message format.
     Current this method is not public to client. */
    virtual void setFormat(std::string const& format);
};


/**Log destination to file.*/
struct FileDestination: DestinationBase {
    /**Create file destination with specified path to log file.
     @param[in] path path to write log to.
     @param[in] severity log severity level.
     @note if folder in given path does not exist, the folder will be created. */
    explicit FileDestination(std::string const& path, Level severity = DefaultLevel);

    virtual ~FileDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


/**Log destination to file asynchronizely.*/
struct FileDestinationAsync: DestinationBase {
    /**Create file destination with specified path to log file.
     @param[in] path path to write log to.
     @param[in] severity log severity level.
     @note if folder in given path does not exist, the folder will be created. */
    explicit FileDestinationAsync(std::string const& path, Level severity = DefaultLevel);

    virtual ~FileDestinationAsync() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


/**Log destination to std::clog (generally stdout).*/
struct ConsoleDestination: DestinationBase {
    /**Create console log destination.
     @param[in] severity log severity level. */
    explicit ConsoleDestination(Level severity = DefaultLevel);

    virtual ~ConsoleDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


/**Log destination to std::clog asyncrhonizely.*/
struct ConsoleDestinationAsync: DestinationBase {
    /**Create console log destination.
     @param[in] severity log severity level. */
    explicit ConsoleDestinationAsync(Level severity = DefaultLevel);

    virtual ~ConsoleDestinationAsync() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


/**Log destination to std::ostream.
 @note given stream pointer will be maintained internally, do not delete it. */
struct StreamDestination: DestinationBase {
    /**Create output stream destination.
     @param[in] stream pointer to stream to be used.
     @param[in] deleteStreamWhenDestroy delete pointer to give n@p stream when destructor is called.
     @param[in] severity log severity level. */
    explicit StreamDestination(
        std::ostream* stream, bool deleteStreamWhenDestroy = true, Level severity = DefaultLevel);

    virtual ~StreamDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


/**Log destination to std::ostream asynchronizely.
 @note given stream pointer will be maintained internally, do not delete it. */
struct StreamDestinationAsync: DestinationBase {
    /**Create output stream destination.
     @param[in] stream pointer to stream to be used.
     @param[in] deleteStreamWhenDestroy delete pointer to give n@p stream when destructor is called.
     @param[in] severity log severity level. */
    explicit StreamDestinationAsync(
        std::ostream* stream, bool deleteStreamWhenDestroy = true, Level severity = DefaultLevel);

    virtual ~StreamDestinationAsync() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

/**Log destination to windows debug output.*/
struct DebugOutputDestination: DestinationBase {
    /**Create log destination to Windows debug output.
     @param[in] severity log severity level. */
    explicit DebugOutputDestination(Level severity = DefaultLevel);

    virtual ~DebugOutputDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


/**Log destination to windows debug output asynchronizely. */
struct DebugOutputDestinationAsync: DestinationBase {
    /**Create log destination to Windows debug output.
     @param[in] severity log severity level. */
    DebugOutputDestinationAsync(Level severity = DefaultLevel);

    virtual ~DebugOutputDestinationAsync() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif

} // ! namespace josh::util::logger

#endif // ! _JOSH_UTIL_LOGGER_DESTINATION_H_

