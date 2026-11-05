/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_string.h

 2026-11-02 22:52:24

*/
#ifndef UI_STRING_H_20261102225224
#define UI_STRING_H_20261102225224

#include <string.h>
#include <string>

#define MINIMGUI_UTF8_ENCODING

namespace minimgui {

typedef std::string uiString;

//------------------------------------------------------------------------------
// UNICODE
//------------------------------------------------------------------------------

// clang-format off

// UTF-32: 无效码点
static const uint32_t UTF_REPLACEMENT_CHAR = 0xFFFD;

// UTF-32: 各长度对应的最小合法码点（用于拒绝过长编码）
static const uint32_t kMinCp[5] = { 0, 0, 0x80, 0x800, 0x10000 };

// UTF-32: 判断码点是否为合法的 Unicode 标量值
// 合法标量值：0..D7FF 或 E000..10FFFF（不含代理区，不含 0 之外的合法）
inline bool is_valid_scalar(uint32_t cp)
{
    return cp <= 0x10FFFF && !(cp >= 0xD800 && cp <= 0xDFFF);
}

// UTF-16: 判断是否高代理
inline bool is_high_surrogate(uint16_t u) { return u >= 0xD800 && u <= 0xDBFF; }

// UTF-16: 判断是否低代理
inline bool is_low_surrogate (uint16_t u) { return u >= 0xDC00 && u <= 0xDFFF; }

// UTF-8: 返回 UTF-8 字符串长度
inline int utf8_strlen(const unsigned char* str)
{
    return static_cast<int>(strlen(reinterpret_cast<const char*>(str)));
}

// UTF-8: 根据首字节返回 UTF-8 序列长度；非法首字节返回 1
inline int utf8_seqlen(unsigned char ch)
{
    if ( ch < 0x80)          return 1;
    if ((ch & 0xE0) == 0xC0) return 2;
    if ((ch & 0xF0) == 0xE0) return 3;
    if ((ch & 0xF8) == 0xF0) return 4;
    return 1; // 非法/续字节
}

// UTF-8: 判断 pos 是否是码点边界
inline bool utf8_is_boundary(const unsigned char* str, int pos, int len)
{
    if (pos <= 0 || pos >= len)
        return true;
    return (str[pos] & 0xC0) != 0x80;
}

// UTF-8: 从 pos 往前退一个码点，返回新的字节偏移
inline int utf8_prev(const unsigned char* str, int pos)
{
    if (pos <= 0)
        return 0;
    --pos;
    while (pos > 0 && (str[pos] & 0xC0) == 0x80)
        --pos;
    return pos;
}

// UTF-8: 从 pos 前进一个码点，返回新的字节偏移；不超过 len
inline int utf8_next(const unsigned char* buf, int pos, int len)
{
    if (pos >= len)
        return len;
    int n = utf8_seqlen(buf[pos]);
    int p = pos + n;
    return p > len ? len : p;
}

// UTF-8: 统计字节缓冲中的码点数量
inline int utf8_cplen(const unsigned char* str, int len)
{
    int n = 0;
    for (int i = 0; i < len; i = utf8_next(str, i, len))
        ++n;
    return n;
}

//------------------------------------------------------------------------------

// 将单个码点编码为 UTF-8 写入 dst（至少 4 字节空间），返回写入字节数。
size_t utf8_encode(uint32_t cp, unsigned char dst[4], int* outErr);

// 从 UTF-8 字符串解出首个码点，成功返回转换字节数 (1~4)。
size_t utf8_decode(const unsigned char* src, size_t srcLen, uint32_t* codePoint, int* outErr);

// 将单个码点编码为 UTF-16 写入 dst，返回写入字节数。
size_t utf16_encode(uint32_t cp, uint16_t dst[2], int* outErr);

// 从 UTF-16 字符串解出首个码点，成功返回转换字节数 (1~2)。
size_t utf16_decode(const uint16_t* src, size_t srcLen, uint32_t* cp, int* outErr);

//------------------------------------------------------------------------------

/* 将 UTF-8 字节序列解码为 UTF-32 码点序列。
 *
 * 参数：
 * src      UTF-8 字符串
 * srcLen   UTF-8 字符串长度
 * dst      输出 UTF-32 码点缓冲区
 * dstLen   输出缓冲区容量（不含终止符）
 * outErr   错误计数
 *
 * 返回值： 成功转换的码点数量
 *
 * 说明：
 *   - 代理区、超范围码点替换成 0xFFFD，并累加 outErr。
 *   - 结构非法（起始字节/continuation/过长编码）跳过 1 个字节继续扫描，累加 outErr。
 *   - 结构合法但码点非法（代理区/超范围）输出 U+FFFD，累加 outErr。
 *   - 如果 dst 为空，则只统计转换需要空间大小；如果 dst 不为空，dstLen == 0 视为错误参数返回 0。
 *   - 若目标空间不足，截断返回已转换部分。
 *   - 不会在 dst 中写入终止符
 */
size_t utf8_to_utf32(const unsigned char* src, size_t srcLen, uint32_t* dst, size_t dstLen, int* outErr);

/* 将 UTF-32 码点序列编码为 UTF-8 字节序列。
 *
 * src      输入 UTF-32 码点
 * srcLen   输入码点数（U+0000 视为普通码点，不会被当作终止符）
 * dst      输出 UTF-8 字节缓冲区
 * dstLen   输出缓冲区容量
 * outErr   被替换为 U+FFFD 的非法码点数（可为 nullptr）
 *
 * 返回值： 成功转换的 UTF-8 字符串长度
 *
 * 说明：
 *   - 代理区、超范围码点替换成 0xFFFD，并累加 outErr。
 *   - 如果 dst 为空，则只统计转换需要空间大小；如果 dst 不为空，dstLen == 0 视为错误参数返回 0。
 *   - 若目标空间不足，截断返回已转换部分。
 *   - 不会在 dst 中写入终止符
 */
size_t utf32_to_utf8(const uint32_t* src, size_t srcLen, unsigned char* dst, size_t dstLen, int* outErr);

/* 将 UTF-16 字符串解码为 UTF-32 码点序列。
 *
 * 参数：
 * src      UTF-16 字符串
 * srcLen   UTF-16 字符串长度
 * dst      输出 UTF-32 码点缓冲区
 * dstLen   输出缓冲区容量
 * outErr   错误计数
 *
 * 返回值： 成功转换的码点数量
 */
size_t utf16_to_utf32(const uint16_t* src, size_t srcLen, uint32_t* dst, size_t dstLen, int* outErr);

/* 将 UTF-32 码点序列编码为 UTF-16 字符串。
 *
 * src      输入 UTF-32 码点
 * srcLen   输入码点数（U+0000 视为普通码点，不会被当作终止符）
 * dst      输出 UTF-16 编码缓冲区
 * dstLen   输出缓冲区容量
 * outErr   被替换为 U+FFFD 的非法码点数（可为 nullptr）
 *
 * 返回值： 成功转换的 UTF-16 字符串长度
 */
size_t utf32_to_utf16(const uint32_t* src, size_t srcLen, uint16_t* dst, size_t dstLen, int* outErr);

// UTF16 -> UTF-8
size_t utf16_to_utf8(const uint16_t* src, size_t srcLen, uint8_t* dst, size_t dstLen, int* outErr);

// UTF8 -> UTF-16
size_t utf8_to_utf16(const uint8_t* src, size_t srcLen, uint16_t* dst, size_t dstLen, int* outErr);

// clang-format on

//------------------------------------------------------------------------------
// 平台字符串相关函数
//------------------------------------------------------------------------------

#if defined(_WIN32)
    #define MINIMGUI_PLATFORM_WINDOWS
#endif

#if defined(MINIMGUI_PLATFORM_WINDOWS)

// ANSI to UNICODE
size_t ansi_to_unicode(std::wstring& wstr, const char* astr, size_t length);

// UNICODE to ANSI
size_t unicode_to_ansi(std::string& astr, const wchar_t* wstr, size_t length);

// UTF-8 to UNICODE
size_t utf8_to_unicode(std::wstring& wstr, const char* u8str, size_t length);

// UNICODE to UTF-8
size_t unicode_to_utf8(std::string& u8str, const wchar_t* wstr, size_t length);

// ANSI to UTF-8
size_t ansi_to_utf8(std::string& u8str, const char* astr, size_t length);

// UTF-8 to ANSI
size_t utf8_to_ansi(std::string& astr, const char* u8str, size_t length);

#else  // MINIMGUI_PLATFORM_WINDOWS

// UTF-8 to UNICODE
size_t to_unicode(std::wstring& wstr, const unsigned char* u8str, size_t length);

// UNICODE to UTF-8
size_t to_utf8(u8string& u8str, const wchar_t* wstr, size_t length);

#endif // MINIMGUI_PLATFORM_WINDOWS

} // namespace minimgui

#endif // UI_STRING_H_20261102225224
