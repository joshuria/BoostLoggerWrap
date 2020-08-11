#include "text.h"
#include <boost/locale.hpp>
#include <cwchar>
#include <locale>

using namespace josh::util;


std::wstring Text::toWString(char const* str) {
    auto const isAlreadyUtf8 = std::use_facet<boost::locale::info>(std::locale()).utf8();
    std::locale oldLocale;
    if (!isAlreadyUtf8)
        oldLocale = std::locale::global(std::locale(".UTF-8"));

    std::wstring output;
    std::mbstate_t state;
#ifdef _MSC_VER
    std::size_t len;
    if (mbsrtowcs_s(&len, nullptr, 0, &str, _TRUNCATE, &state) != 0) {
        // Invalid string
        output = L"<Invalid string>";
    }
    else {
        output.resize(len);
        mbsrtowcs_s(nullptr, &output[0], len, &str, _TRUNCATE, &state);
        output.pop_back();
    }
#else
    auto const len = std::mbsrtowcs(nullptr, &str, 0, &state);
    if (len == static_cast<std::size_t>(-1)) {
        // Invalid string
        output = L"<Invalid string>";
    }
    else {
        output.resize(len);
        std::mbsrtowcs(&output[0], &str, len, &state);
    }
#endif

    if (!isAlreadyUtf8)
        std::locale::global(oldLocale);
    return output;
}


std::string Text::toString(wchar_t const* wstr) {
    auto const isAlreadyUtf8 = std::use_facet<boost::locale::info>(std::locale()).utf8();
    std::locale oldLocale;
    if (!isAlreadyUtf8)
        oldLocale = std::locale::global(std::locale(".UTF-8"));

    std::string output;
    std::mbstate_t state;
#ifdef _MSC_VER
    std::size_t len;
    if (wcsrtombs_s(&len, nullptr, 0, &wstr, _TRUNCATE, &state) != 0) {
        // Invalid string
        output = "<Invalid string>";
    }
    else {
        output.resize(len);
        wcsrtombs_s(nullptr, &output[0], len, &wstr, _TRUNCATE, &state);
        output.pop_back();
    }
#else
    auto const len = std::wcsrtombs(nullptr, &wstr, 0, &state);
    if (len == static_cast<std::size_t>(-1)) {
        // Invalid string
        output = "<Invalid string>";
    }
    else {
        output.resize(len);
        std::wcsrtombs(&output[0], &swtr, len, &state);
    }
#endif

    if (!isAlreadyUtf8)
        std::locale::global(oldLocale);
    return output;
}

