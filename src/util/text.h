#pragma once
#ifndef _UTIL_TEXT_H_
#define _UTIL_TEXT_H_

#include <string>

namespace josh::util {


/**Text-related utility functions.*/
struct Text {
    Text() = delete;

    /**Convert std::string to std::wstring.
     @note this method does NOT handle unicode BOM. */
    static std::wstring toWString(std::string const& str);
    /**@overload */
    static std::wstring toWString(char const* str);
    /**Convert std::wstring to std::string.
     @note this method does NOT handle unicode BOM. */
    static std::string toString(std::wstring const& wstr);
    /**@overload */
    static std::string toString(wchar_t const* wstr);
}; // ! struct Text


/*************************************************************/
// Implementation
/*************************************************************/
inline std::wstring Text::toWString(std::string const& str) {
    return toWString(str.c_str());
}


inline std::string Text::toString(std::wstring const& wstr) {
    return toString(wstr.c_str());
}

} // ! namespace josh::util

#endif // ! _UTIL_TEXT_H_

