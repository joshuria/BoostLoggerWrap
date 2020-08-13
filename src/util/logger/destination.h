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
struct IDestination {
    virtual ~IDestination() noexcept = default;

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
    Level severity = Level::Error;

protected:
    /**Set log message format.
     Current this method is not public to client. */
    virtual void setFormat(std::string const& format) = 0;
};


/**Log destination to file.*/
struct FileDestination: IDestination {
    friend class Logger;

    /**Create file destination with specified path to log file.
     @note if folder in given path does not exist, the folder will be created. */
    FileDestination(std::string const& path);

    virtual ~FileDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

protected:
    /**Set log message format. */
    virtual void setFormat(std::string const& format) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


struct ConsoleDestination: IDestination {
    friend class Logger;

    explicit ConsoleDestination();

    virtual ~ConsoleDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

protected:
    /**Set log message format. */
    virtual void setFormat(std::string const& format) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


struct StreamDestination: IDestination {
    /**Create output stream destination.
     @note this class will maintain given stream pointer internally, so do not delete it or give a
        local variable's pointer. */
    StreamDestination(std::ostream* stream);

    virtual ~StreamDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

protected:
    /**Set log message format. */
    virtual void setFormat(std::string const& format) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

struct DebugOutputDestination: IDestination {
    DebugOutputDestination();

    virtual ~DebugOutputDestination() noexcept;

    /**Apply log destination.*/
    virtual void enable(Logger& logger) override;
    /**Disable log destination filter.
     Log messages will not write to this destination. */
    virtual void disable() override;
    /**Get if this destination is enabled.*/
    [[nodiscard]]
    virtual bool isEnabled() const override;

protected:
    /**Set log message format. */
    virtual void setFormat(std::string const& format) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif

} // ! namespace josh::util::logger

#endif // ! _JOSH_UTIL_LOGGER_DESTINATION_H_

