#pragma once
#ifndef _JOSH_UTIL_LOGGER_LOGGERMANAGER_H_
#define _JOSH_UTIL_LOGGER_LOGGERMANAGER_H_

#include <string>
#include <memory>


namespace josh::util::logger {

class Logger;
struct DestinationBase;

/**Manager for Logger instances.
 @example
    std::shared_ptr<Logger> logger = LoggerManager::getInstance().newBuilder()
        .setName("TestingLog")
        .appendDestination()
        .appendDestination(Destination::Stream, std::cerr, Level::Warn)
        .appendDestination(Destination::DebugOutput, Level::Info)
        .create();
    logger->info("This is info message");

    auto logger2 = LoggerManager::getInstance().get("TestingLog");
    if (!logger2) {
        // Fail to get log
    }
    logger2->warn("This is warn message");
 */
class LoggerManager {
public:
    class Builder;
    friend class Builder;

    ~LoggerManager() noexcept;
    LoggerManager(LoggerManager const&) = delete;
    LoggerManager(LoggerManager&&) = delete;
    void operator= (LoggerManager const&) = delete;
    void operator= (LoggerManager&&) = delete;

    /**Get singleton instance.*/
    [[nodiscard]]
    static LoggerManager& getInstance() noexcept;

    /**Create new builder instance.
     @warning client muse call Builder::create() or Builder::cancel() in order to prevent resource
        leak. */
    static Builder newBuilder();

    /**Get Logger instance.
     @return a shared pointer to Logger instance. nullptr if not found. */
    std::shared_ptr<Logger> get(std::string const& name);

    /**Globally enable logging.
     If individual logger instance was disabled before, this method will NOT turn on it. */
    void enableLog();
    /**Globally stop logging.
     All log instances will stop writing to underlying buffer. */
    void disableLog();
    /**Get if logger is globally enabled.*/
    [[nodiscard]]
    bool isEnabled() const;

private:
    LoggerManager(); 

    std::shared_ptr<Logger> addLogger(Builder& builder);

    struct Impl;
    std::unique_ptr<Impl> impl;
}; // ! class Logger


/**Builder of Logger instance.*/
class LoggerManager::Builder {
public:
    friend class LoggerManager;

    ~Builder() noexcept;
    Builder(Builder&& o) noexcept;
    Builder(Builder const&) = delete;
    void operator= (Builder const&) = delete;
    void operator= (Builder&&) = delete;

    /**Create Logger instance.*/
    std::shared_ptr<Logger> create();

    /**Set name of this logger.*/
    Builder&& setName(std::string const& name);

    /**Append destination by given a shared pointer.*/
    Builder&& appendDestination(std::shared_ptr<DestinationBase> dest);
    /**Append destination.*/
    Builder&& appendDestination(DestinationBase* dest);

private:
    Builder();

    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // ! namespace josh::util

#endif // ! _JOSH_UTIL_LOGGER_LOGGERMANAGER_H_

