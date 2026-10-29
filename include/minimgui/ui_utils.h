/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_utils.h

 2026-03-16 09:00:37

*/
#ifndef UI_UTILS_H_20260316090037
#define UI_UTILS_H_20260316090037

#include "ui_std.h"

namespace minimgui {

//
// 辅助函数
//

// 生成 hashID
uiID ui_hash(const void* data, size_t size);
uiID ui_hash(const char* str, size_t length);
uiID ui_hash(const wchar_t* str, size_t length);
uiID ui_hash(const std::string& str);
uiID ui_hash(const std::wstring& str);

// bool 转字符串
const char* bool_to_string(bool value);

// 格式化字符串 (最大 256 字节)
const char* format(const char* param, ...);
const wchar_t* format(const wchar_t* param, ...);

// 统计数组元素数量
#if !defined(countof)
    #define countof(a) (sizeof(a) / sizeof(a[0]))
#endif

// 判断浮点数是否零
inline bool is_zero(float n)
{
    return fabs(n) < FLT_EPSILON;
}

// 判断点 p 是否在三角形 abc 内
bool point_in_triangle(uiVec2 p, uiVec2 a, uiVec2 b, uiVec2 c);

// 判断是否支持标志位
inline bool check_flags(uint32_t flags, uint32_t value)
{
    return (flags & value) == value;
}

// 通过掩码设置标志位
inline void set_flags_masked(uint32_t& flags, uint32_t value, uint32_t mask)
{
    flags = (flags & (~mask)) | (value & mask);
}

// 设置标志位是否启用
inline void set_flags_enabled(uint32_t& flags, uint32_t value, bool enable)
{
    if (enable) {
        flags |= value;
    }
    else {
        flags &= ~value;
    }
}

// 向后移动一个 Unicode 字符 (按码点计)。
inline const char* utf8_next(const char* p, const char* end)
{
    if (p >= end)
        return end;
    ++p; // 跳过当前字符
    // 跳过所有后续字节
    while (p < end && (*p & 0xC0) == 0x80) { // 判断是否 utf-8 后续字节 (10xxxxxx)
        ++p;
    }
    return p;
}

// 向前移动一个 Unicode 字符 (按码点计)。
inline const char* utf8_prev(const char* p, const char* begin)
{
    if (p <= begin)
        return begin;
    --p; // 先退一格
    // 向前跳过所有后续字节 (10xxxxxx)，直到起始字节
    while (p > begin && (*p & 0xC0) == 0x80) {
        --p;
    }
    return p;
}

} // namespace minimgui

#endif // UI_UTILS_H_20260316090037
