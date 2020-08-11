#pragma once
#ifndef _UTIL_LOGGER_H_
#define _UTIL_LOGGER_H_

#include <boost/format.hpp>
#include <string>
#include <memory>


namespace josh::util {

/**Boost::Logger based logging.
 @example
    Logger.initialize(
        true, // is log to file
        true, // is log write to console (std::clog or stdout)
        "log/%n%.log", // logger file path template, the %n% will be replaced by logger's name
        false, // file use buffered IO
        false  // console use buffered IO
    );
    auto& logger = Logger.get("loggerName");  // Get or create new logger
    logger.setSeverity(Logger::Level::Warn);  // Change severity to warn
    logger.info("this is info");              // This will NOT appear in log
    logger.warn("this is warning");
    logger.error("this is error");

 Available variable in logger name template:
   - %n%: logger's name.
   - %t%: current local date time: yyyyMMdd-hhmmss
 */
class Logger {
public:
    ~Logger() noexcept;
    Logger(Logger const&) = delete;
    Logger(Logger&&) = delete;
    void operator= (Logger const&) = delete;
    void operator= (Logger&&) = delete;

    /**Severity level.*/
    enum class Level {
        Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Fatal = 5
    };

    /**Get or create logger instance.
     If specified @p name does not found, new logger with @p name and pre-configured instance will
     be returned.
     Thread-safe: yes, guarded by read-write lock. */
    static Logger& get(std::string const& name);

    /**Create new logger instance.
     If specified @p name has already existed, the existed instance will be returned.
     Thread-safe: yes, guarded by read-write lock. */
    static Logger& create(
        std::string const& name,
        bool toFile = true, bool toConsole = false,
        std::string const& logPath = "", Level severity = Level::Error,
        bool fileBuffering = false, bool consoleBuffering = false);

    /**Get log file path.*/
    [[nodiscard]]
    std::string const& getLogPath() const noexcept;
    /**Get if write to file.*/
    [[nodiscard]]
    bool isToFile() const noexcept;
    /**Get if write to console.*/
    [[nodiscard]]
    bool isToConsole() const noexcept;
    /**Get if log to file is buffered.*/
    [[nodiscard]]
    bool isFileBuffered() const noexcept;
    /**Get if log to console is buffered.*/
    [[nodiscard]]
    bool isConsoleBuffered() const noexcept;
    /**Get log severity level.*/
    [[nodiscard]]
    Level getSeverity() const noexcept;
    /**Set log severity level.*/
    void setSeverity(Level level);
    /**Get if logging is enabled.*/
    [[nodiscard]]
    bool isEnabled() const noexcept;
    /**Enable or disable logging.*/
    void enableLog(bool enable);

    void write(Level level, std::string const& msg) const;
    void write(Level level, std::wstring const& msg) const;
    void trace(std::string const& msg) const;
    void trace(std::wstring const& msg) const;
    void debug(std::string const& msg) const;
    void debug(std::wstring const& msg) const;
    void info(std::string const& msg) const;
    void info(std::wstring const& msg) const;
    void warn(std::string const& msg) const;
    void warn(std::wstring const& msg) const;
    void error(std::string const& msg) const;
    void error(std::wstring const& msg) const;
    void fatal(std::string const& msg) const;
    void fatal(std::wstring const& msg) const;

    void testing() const;

    //! See: https://stackoverflow.com/questions/26788361/is-there-any-way-to-use-varargs-with-boostformat
    void write(Level level, boost::format& fmt) const;
    template<typename TValue, typename... TArgs>
    void write(Level level, boost::format& fmt, TValue&& arg, TArgs&&... args) const {
        fmt% std::forward<TValue>(arg);
        write(level, fmt, std::forward<TArgs>(args)...);
    }
    template<typename... TArgs>
    void write(Level level, std::string const& fmt, TArgs&&... args) const {
        boost::format f(fmt);
        write(level, f, std::forward<TArgs>(args)...);
    }
    void write(Level level, boost::wformat& fmt) const;
    template<typename TValue, typename... TArgs>
    void write(Level level, boost::wformat& fmt, TValue&& arg, TArgs&&... args) const {
        fmt% std::forward<TValue>(arg);
        write(level, fmt, std::forward<TArgs>(args)...);
    }
    template<typename... TArgs>
    void write(Level level, std::wstring const& fmt, TArgs&&... args) const {
        boost::wformat f(fmt);
        write(level, f, std::forward<TArgs>(args)...);
    }
#define DeclareMethod(MethodName, StringT, Fmt) \
    void MethodName(Fmt& fmt) const; \
    template<typename TValue, typename... TArgs> \
    void MethodName(Fmt& fmt, TValue&& arg, TArgs&&... args) const { \
        fmt % std::forward<TValue>(arg); \
        MethodName(fmt, std::forward<TArgs>(args)...); \
    } \
    template<typename... TArgs> \
    void MethodName(StringT const& fmt, TArgs&&... args) const {\
        Fmt format(fmt); \
        MethodName(format, std::forward<TArgs>(args)...); \
    }

    DeclareMethod(trace, std::string, boost::format)
        DeclareMethod(trace, std::wstring, boost::wformat)
        DeclareMethod(debug, std::string, boost::format)
        DeclareMethod(debug, std::wstring, boost::wformat)
        DeclareMethod(info, std::string, boost::format)
        DeclareMethod(info, std::wstring, boost::wformat)
        DeclareMethod(warn, std::string, boost::format)
        DeclareMethod(warn, std::wstring, boost::wformat)
        DeclareMethod(error, std::string, boost::format)
        DeclareMethod(error, std::wstring, boost::wformat)
        DeclareMethod(fatal, std::string, boost::format)
        DeclareMethod(fatal, std::wstring, boost::wformat)
#undef DeclareMethod

private:
    Logger(
        std::string name, std::string path, bool toFile, bool toConsole,
        bool fileBuffered, bool consoleBuffered, Level severity);

    struct LoggerImpl;
    std::unique_ptr<LoggerImpl> impl;
}; // ! class Logger


inline void Logger::trace(std::string const& msg) const { write(Level::Trace, msg); }
inline void Logger::trace(std::wstring const& msg) const { write(Level::Trace, msg); }
inline void Logger::debug(std::string const& msg) const { write(Level::Debug, msg); }
inline void Logger::debug(std::wstring const& msg) const { write(Level::Debug, msg); }
inline void Logger::info(std::string const& msg) const { write(Level::Info, msg); }
inline void Logger::info(std::wstring const& msg) const { write(Level::Info, msg); }
inline void Logger::warn(std::string const& msg) const { write(Level::Warn, msg); }
inline void Logger::warn(std::wstring const& msg) const { write(Level::Warn, msg); }
inline void Logger::error(std::string const& msg) const { write(Level::Error, msg); }
inline void Logger::error(std::wstring const& msg) const { write(Level::Error, msg); }
inline void Logger::fatal(std::string const& msg) const { write(Level::Fatal, msg); }
inline void Logger::fatal(std::wstring const& msg) const { write(Level::Fatal, msg); }

inline void Logger::trace(boost::format& fmt) const { write(Level::Trace, fmt); }
inline void Logger::trace(boost::wformat& fmt) const { write(Level::Trace, fmt); }
inline void Logger::debug(boost::format& fmt) const { write(Level::Debug, fmt); }
inline void Logger::debug(boost::wformat& fmt) const { write(Level::Debug, fmt); }
inline void Logger::info(boost::format& fmt) const { write(Level::Info, fmt); }
inline void Logger::info(boost::wformat& fmt) const { write(Level::Info, fmt); }
inline void Logger::warn(boost::format& fmt) const { write(Level::Warn, fmt); }
inline void Logger::warn(boost::wformat& fmt) const { write(Level::Warn, fmt); }
inline void Logger::error(boost::format& fmt) const { write(Level::Error, fmt); }
inline void Logger::error(boost::wformat& fmt) const { write(Level::Error, fmt); }
inline void Logger::fatal(boost::format& fmt) const { write(Level::Fatal, fmt); }
inline void Logger::fatal(boost::wformat& fmt) const { write(Level::Fatal, fmt); }

} // ! namespace josh::util

#endif // ! _UTIL_LOGGER_H_

