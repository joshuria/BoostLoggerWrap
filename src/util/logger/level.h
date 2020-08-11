#pragma once
#ifndef _JOSH_UTIL_LOGGER_LEVEL_H_
#define _JOSH_UTIL_LOGGER_LEVEL_H_

namespace josh::util::logger {
    /**Pre-defined log severity level.*/
    enum Level {
        Trace = 600,
        Debug = 500,
        Info = 400,
        Warn = 300,
        Error = 200,
        Fatal = 100
    };
} // ! namespace josh::util::logger

#endif // ! _JOSH_UTIL_LOGGER_LEVEL_H_

