#pragma once
#ifndef _JOSH_UTIL_LOGGER_LEVEL_H_
#define _JOSH_UTIL_LOGGER_LEVEL_H_

namespace josh::util::logger {

/**Pre-defined log severity level.*/
enum class Level {
    //Trace = 600,
    //Debug = 500,
    //Info = 400,
    //Warn = 300,
    //Error = 200,
    //Fatal = 100
    Trace = 6,
    Debug = 5,
    Info = 4,
    Warn = 3,
    Error = 2,
    Fatal = 1
};


/**Shrink level to 1 char and output to stream.*/
template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& operator<< (
    std::basic_ostream<CharT, Traits>& stream, Level lv
    ) {
    static constexpr char const* LevelMapping[] = { "A", "F", "E", "W", "I", "D", "T" };
    return stream << LevelMapping[static_cast<int>(lv)];
}

} // ! namespace josh::util::logger

#endif // ! _JOSH_UTIL_LOGGER_LEVEL_H_

