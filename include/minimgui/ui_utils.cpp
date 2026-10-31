#include "ui_utils.h"
#include "ui.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <xxh3.h>

namespace minimgui {

// 生成 hashID
uiID ui_hash(const void* data, size_t size)
{
    return XXH3_64bits_withSeed(data, size, 0);
}

uiID ui_hash(const char* str, size_t length)
{
    return XXH3_64bits_withSeed(str, length, 0);
}

uiID ui_hash(const wchar_t* str, size_t length)
{
    return XXH3_64bits_withSeed(str, length * sizeof(wchar_t), 0);
}

uiID ui_hash(const std::string& str)
{
    return ui_hash(str.c_str(), str.length());
}

uiID ui_hash(const std::wstring& str)
{
    return ui_hash(str.c_str(), str.length());
}

// bool 转字符串
const char* bool_to_string(bool value)
{
    return value ? "true" : "false";
}

// 格式化字符串 (最大 256 字节)
const char* format(const char* param, ...)
{
    static char buf[256] = { 0 };

    va_list body;
    va_start(body, param);
    std::vsnprintf(buf, countof(buf), param, body);
    va_end(body);

    return buf;
}

const wchar_t* format(const wchar_t* param, ...)
{
    static wchar_t buf[256] = { 0 };

    va_list body;
    va_start(body, param);
    _vsnwprintf(buf, countof(buf), param, body);
    va_end(body);

    return buf;
}

} // namespace minimgui
