#include "ui_string.h"

#if defined(MINIMGUI_PLATFORM_WINDOWS)
#include <windows.h>
#else
#endif

namespace minimgui {

//------------------------------------------------------------------------------
// UNICODE
//------------------------------------------------------------------------------

// clang-format off

/*
// 向后移动一个 Unicode 字符
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

// 向前移动一个 Unicode 字符
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
*/

namespace detail {

// 返回 utf8 编码长度
inline size_t utf8_encoded_len(uint32_t cp)
{
    if (!is_valid_scalar(cp)) return 3; // 非法码点（代理区/超范围）替换输出 U+FFFD，返回长度 3
    if (cp < 0x80)    return 1;
    if (cp < 0x800)   return 2;
    if (cp < 0x10000) return 3;
    return 4;
}

// 写入一个码点，返回写入字节数
inline size_t put_utf8(unsigned char* dst, uint32_t cp)
{
    if (cp < 0x80) {
        dst[0] = (unsigned char)cp;
        return 1;
    }
    else if (cp < 0x800) {
        dst[0] = (unsigned char)(0xC0 | (cp >> 6));
        dst[1] = (unsigned char)(0x80 | (cp & 0x3F));
        return 2;
    }
    else if (cp < 0x10000) {
        dst[0] = (unsigned char)(0xE0 | ( cp >> 12));
        dst[1] = (unsigned char)(0x80 | ((cp >>  6) & 0x3F));
        dst[2] = (unsigned char)(0x80 | ( cp        & 0x3F));
        return 3;
    }
    else {
        dst[0] = (unsigned char)(0xF0 | ( cp >> 18));
        dst[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
        dst[2] = (unsigned char)(0x80 | ((cp >>  6) & 0x3F));
        dst[3] = (unsigned char)(0x80 | ( cp        & 0x3F));
        return 4;
    }
}

} // end namespace detail

// 将单个码点编码为 UTF-8 写入 dst（至少 4 字节空间），返回写入字节数。
size_t utf8_encode(uint32_t cp, unsigned char dst[4], int* outErr)
{
    // 判断是否有效码点
    if (!is_valid_scalar(cp)) {
        if (outErr) ++*outErr;
        cp = UTF_REPLACEMENT_CHAR;
    }

    if (cp < 0x80) {
        dst[0] = (unsigned char)cp;
        return 1;
    }
    else if (cp < 0x800) {
        dst[0] = (unsigned char)(0xC0 | (cp >> 6));
        dst[1] = (unsigned char)(0x80 | (cp & 0x3F));
        return 2;
    }
    else if (cp < 0x10000) {
        dst[0] = (unsigned char)(0xE0 | (cp >> 12));
        dst[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        dst[2] = (unsigned char)(0x80 | (cp & 0x3F));
        return 3;
    }
    else {
        dst[0] = (unsigned char)(0xF0 | (cp >> 18));
        dst[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
        dst[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        dst[3] = (unsigned char)(0x80 | (cp & 0x3F));
        return 4;
    }
}

// 从 UTF-8 字符串解出首个码点，成功返回转换字节数 (1~4)。
size_t utf8_decode(const unsigned char* src, size_t srcLen, uint32_t* codePoint, int* outErr)
{
    uint32_t cp;
    unsigned char unit = src[0];
    size_t u8len;
    size_t k;

    if      ( unit < 0x80)          { cp = unit;        u8len = 1; }
    else if ((unit & 0xE0) == 0xC0) { cp = unit & 0x1F; u8len = 2; }
    else if ((unit & 0xF0) == 0xE0) { cp = unit & 0x0F; u8len = 3; }
    else if ((unit & 0xF8) == 0xF0) { cp = unit & 0x07; u8len = 4; }
    else {
        // 非法起始字节（孤立 continuation byte、0xC0/0xC1、0xF5..0xFF）
        if (outErr) ++*outErr;
        *codePoint = UTF_REPLACEMENT_CHAR;
        return 1;
    }

    // 边界检查：后续字节必须存在
    if (u8len > srcLen) {
        if (outErr) ++*outErr; // 输入被截断，剩余部分无法构成完整序列
        *codePoint = UTF_REPLACEMENT_CHAR;
        return srcLen;
    }

    // 校验并拼接后续字节
    for (k = 1; k < u8len; ++k) {
        unit = src[k];
        if ((unit & 0xC0) != 0x80) { break; }
        cp = (cp << 6) | (unit & 0x3F);
    }
    if (k < u8len) { // k == u8len: 校验通过
        if (outErr) ++*outErr;
        *codePoint = UTF_REPLACEMENT_CHAR;
        return u8len;
    }

    // 拒绝过长编码
    if (u8len >= 2 && cp < kMinCp[u8len]) {
        if (outErr) ++*outErr;
        *codePoint = UTF_REPLACEMENT_CHAR;
        return u8len;
    }

    // 拒绝代理区、超范围
    if (!is_valid_scalar(cp)) {
        if (outErr) ++*outErr;
        cp = UTF_REPLACEMENT_CHAR;
    }

    *codePoint = cp;

    return u8len;
}

//------------------------------------------------------------------------------

// 将单个码点编码为 UTF-16 写入 dst，返回写入字节数。
size_t utf16_encode(uint32_t cp, uint16_t dst[2], int* outErr)
{
    if (!is_valid_scalar(cp)) { // 判断是否有效码点
        if (outErr) ++*outErr;
        cp = UTF_REPLACEMENT_CHAR;
    }

    if (cp <= 0xFFFF) { // BMP 字符：直接写入一个单元
        dst[0] = static_cast<uint16_t>(cp);

        return 1;
    }
    else { // 辅助平面：拆成代理对
        uint32_t v = cp - 0x10000;
        dst[0] = static_cast<uint16_t>(0xD800 + (v >> 10));
        dst[1] = static_cast<uint16_t>(0xDC00 + (v & 0x3FF));

        return 2;
    }
}

// 从 UTF-16 字符串解出首个码点，成功返回转换字节数 (1~2)。
size_t utf16_decode(const uint16_t* src, size_t srcLen, uint32_t* cp, int* outErr)
{
    uint32_t unit = src[0];

    if (unit < 0xD800 || unit > 0xDFFF) { // BMP 字符（非代理区）
        *cp = unit;
        return 1;
    }
    else if (is_high_surrogate(unit)) { // 判断是否高代理
        if (srcLen >= 2) { // 判断是否越界
            if (is_low_surrogate(src[1])) { // 判断是否低代理
                *cp = 0x10000 + ((unit - 0xD800) << 10) + (src[1] - 0xDC00);
                return 2; // 消费低代理
            }
            // else { } // 非法：高代理后不是低代理
        }
        // else { } // 截断：孤立高代理
    }
    // else { } // 孤立低代理

    if (outErr) ++*outErr;
    *cp = UTF_REPLACEMENT_CHAR;

    return 1;
}

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
size_t utf8_to_utf32(const unsigned char* src, size_t srcLen, uint32_t* dst, size_t dstLen, int* outErr)
{
    if (outErr) *outErr = 0;
    if (src == nullptr || srcLen == 0) { // 空输入
        return 0;
    }
    if (dst && dstLen == 0) { // 输出长度是零
        return 0;
    }

    int err        = 0;  // 错误计数
    size_t written = 0;  // 输出统计

    uint32_t cp;

    for (size_t i = 0; i < srcLen; ) {
        i += utf8_decode(src + i, srcLen - i, &cp, outErr);

        // 写入，dst 不为空则输出，否则只统计
        if (dst) {
            // 输出空间检查
            if (written >= dstLen) { // 截断：空间不足
                if (outErr) *outErr = err;
                return written;
            }

            dst[written++] = cp;
        }
        else {
            ++written;
        }
    }

    if (outErr) *outErr = err;

    return written;
}

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
size_t utf32_to_utf8(const uint32_t* src, size_t srcLen, unsigned char* dst, size_t dstLen, int* outErr)
{
    if (outErr) *outErr = 0;
    if (src == nullptr || srcLen == 0) { // 空输入
        return 0;
    }
    if (dst && dstLen == 0) { // 输出长度是零
        return 0;
    }

    int err        = 0;  // 错误计数
    size_t written = 0;  // 输出统计

    uint32_t cp;
    size_t need;

    for (size_t i = 0; i < srcLen; ++i) {
        cp = src[i];

        // 非法码点
        if (!is_valid_scalar(cp)) {
            cp = UTF_REPLACEMENT_CHAR;
            ++err;
        }

        // 写入，dst 不为空则输出，否则只统计
        need = detail::utf8_encoded_len(cp);
        if (dst) {
            // 输出空间检查
            if (written + need > dstLen) { // 截断：空间不足
                if (outErr) *outErr = err;
                return written;
            }

            written += detail::put_utf8(dst + written, cp);
        }
        else {
            written += need;
        }
    }

    if (outErr) *outErr = err;

    return written;
}

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
size_t utf16_to_utf32(const uint16_t* src, size_t srcLen, uint32_t* dst, size_t dstLen, int* outErr)
{
    if (outErr) *outErr = 0;
    if (src == nullptr || srcLen == 0) { // 空输入
        return 0;
    }
    if (dst && dstLen == 0) { // 输出长度是零
        return 0;
    }

    int err        = 0;
    size_t written = 0;

    uint32_t cp;
    uint32_t unit;

    for (size_t i = 0; i < srcLen; ++i) {
        cp = 0;
        unit = src[i];
        if (unit < 0xD800 || unit > 0xDFFF) { // BMP 字符（非代理区）
            cp = unit;
        }
        else if (is_high_surrogate(unit)) { // 判断是否高代理
            if (srcLen >= srcLen - i) { // 判断是否越界
                if (is_low_surrogate(src[i + 1])) { // 判断是否低代理
                    cp = 0x10000 + ((unit - 0xD800) << 10) + (src[i + 1] - 0xDC00);
                    ++i; // 消费低代理
                }
                // else { } // 非法：高代理后不是低代理
            }
            // else { } // 截断：孤立高代理
        }
        // else { } // 孤立低代理

        if (cp == 0) {
            ++err;
            cp = UTF_REPLACEMENT_CHAR;
        }

        // 写入，dst 不为空则输出，否则只统计
        if (dst) {
            if (written >= dstLen) { // 截断：空间不足
                break;
            }
            dst[written++] = cp;
        }
        else {
            ++written;
        }
    }

    if (outErr) *outErr = err;

    return written;
}

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
size_t utf32_to_utf16(const uint32_t* src, size_t srcLen, uint16_t* dst, size_t dstLen, int* outErr)
{
    if (outErr) *outErr = 0;
    if (src == nullptr || srcLen == 0) { // 空输入
        return 0;
    }
    if (dst && dstLen == 0) { // 输出长度是零
        return 0;
    }

    int err        = 0;
    size_t written = 0;

    uint32_t cp;
    size_t need;

    for (size_t i = 0; i < srcLen; ++i) {
        cp = src[i];

        if (!is_valid_scalar(cp)) { // 判断是否有效码点
            cp = UTF_REPLACEMENT_CHAR;
            ++err;
        }

        // 写入
        need = (cp <= 0xFFFF) ? 1 : 2;
        if (dst) {
            if (written + need > dstLen) { // 截断：空间不足
                break;
            }

            if (need == 1) { // BMP 字符：直接写入一个单元
                dst[written++] = static_cast<uint16_t>(cp);
            }
            else { // 辅助平面：拆成代理对
                uint32_t v     = cp - 0x10000;
                dst[written++] = static_cast<uint16_t>(0xD800 + (v >> 10));
                dst[written++] = static_cast<uint16_t>(0xDC00 + (v & 0x3FF));
            }
        }
        else {
            written += need;
        }
    }

    if (outErr) *outErr = err;

    return written;
}

// UTF16 -> UTF-8
size_t utf16_to_utf8(const uint16_t* src, size_t srcLen, uint8_t* dst, size_t dstLen, int* outErr)
{
    if (outErr) *outErr = 0;
    if (src == nullptr || srcLen == 0) { // 空输入
        return 0;
    }
    if (dst && dstLen == 0) { // 输出长度是零
        return 0;
    }

    int err        = 0;
    size_t written = 0;

    uint32_t cp;
    uint32_t unit;
    size_t need;

    for (size_t i = 0; i < srcLen; ++i) {
        cp = 0;
        unit = src[i];
        if (unit < 0xD800 || unit > 0xDFFF) { // BMP 字符（非代理区）
            cp = unit;
        }
        else if (is_high_surrogate(unit)) { // 判断是否高代理
            if (srcLen >= srcLen - i) { // 判断是否越界
                if (is_low_surrogate(src[i + 1])) { // 判断是否低代理
                    cp = 0x10000 + ((unit - 0xD800) << 10) + (src[i + 1] - 0xDC00);
                    ++i; // 消费低代理
                }
                // else { } // 非法：高代理后不是低代理
            }
            // else { } // 截断：孤立高代理
        }
        // else { } // 孤立低代理

        if (cp == 0) {
            ++err;
            cp = UTF_REPLACEMENT_CHAR;
        }

        // 写入，dst 不为空则输出，否则只统计
        need = detail::utf8_encoded_len(cp);
        if (dst) {
            // 输出空间检查
            if (written + need > dstLen) { // 截断：空间不足
                if (outErr) *outErr = err;
                return written;
            }

            written += detail::put_utf8(dst + written, cp);
        }
        else {
            written += need;
        }
    }

    if (outErr) *outErr = err;

    return written;
}

// UTF8 -> UTF-16
size_t utf8_to_utf16(const uint8_t* src, size_t srcLen, uint16_t* dst, size_t dstLen, int* outErr)
{
    if (outErr) *outErr = 0;
    if (src == nullptr || srcLen == 0) { // 空输入
        return 0;
    }
    if (dst && dstLen == 0) { // 输出长度是零
        return 0;
    }

    int err        = 0;
    size_t written = 0;
    uint32_t cp;
    size_t need;

    for (size_t i = 0; i < srcLen; ) {
        i += utf8_decode(src + i, srcLen - i, &cp, outErr);

        // 写入，dst 不为空则输出，否则只统计
        need = (cp <= 0xFFFF) ? 1 : 2;
        if (dst) {
            // 输出空间检查
            if (written + need > dstLen) { // 截断：空间不足
                if (outErr) *outErr = err;
                return written;
            }

            if (need == 1) { // BMP 字符：直接写入一个单元
                dst[written++] = static_cast<uint16_t>(cp);
            }
            else { // 辅助平面：拆成代理对
                uint32_t v     = cp - 0x10000;
                dst[written++] = static_cast<uint16_t>(0xD800 + (v >> 10));
                dst[written++] = static_cast<uint16_t>(0xDC00 + (v & 0x3FF));
            }
        }
        else {
            written += need;
        }
    }

    if (outErr) *outErr = err;

    return written;
}

//------------------------------------------------------------------------------
// 平台字符串相关函数
//------------------------------------------------------------------------------

#if defined(MINIMGUI_PLATFORM_WINDOWS)

// ANSI to UNICODE
size_t ansi_to_unicode(std::wstring& wstr, const char* astr, size_t length)
{
    int wlen = MultiByteToWideChar(CP_ACP, 0, astr, (int) length, NULL, 0);
    if (wlen == 0) {
        wstr.clear();
        return 0;
    }

    wstr.resize(wlen);
    MultiByteToWideChar(CP_ACP, 0, astr, (int) length, &wstr[0], wlen);

    return static_cast<size_t>(wlen);
}

// UNICODE to ANSI
size_t unicode_to_ansi(std::string& astr, const wchar_t* wstr, size_t length)
{
    int len = WideCharToMultiByte(CP_ACP, 0, wstr, (int) length, nullptr, 0, nullptr, nullptr);
    if (len == 0) {
        astr.clear();
        return 0;
    }

    astr.resize(len);
    WideCharToMultiByte(CP_ACP, 0, wstr, (int) length, (char*) &astr[0], len, nullptr, nullptr);

    return static_cast<size_t>(len);
}

// UTF-8 to UNICODE
size_t utf8_to_unicode(std::wstring& wstr, const char* u8str, size_t length)
{
    int wlen = MultiByteToWideChar(CP_UTF8, 0, (const char*) u8str, (int) length, NULL, 0);
    if (wlen == 0) {
        wstr.clear();
        return 0;
    }

    wstr.resize(wlen);
    MultiByteToWideChar(CP_UTF8, 0, (const char*) u8str, (int) length, &wstr[0], wlen);

    return static_cast<size_t>(wlen);
}

// UNICODE to UTF-8
size_t unicode_to_utf8(std::string& u8str, const wchar_t* wstr, size_t length)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, (int) length, nullptr, 0, nullptr, nullptr);
    if (len == 0) {
        u8str.clear();
        return 0;
    }

    u8str.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, wstr, (int) length, (char*) &u8str[0], len, nullptr, nullptr);

    return static_cast<size_t>(len);
}

// ANSI to UTF-8
size_t ansi_to_utf8(std::string& u8str, const char* astr, size_t length)
{
    std::wstring wstr;
    size_t len = ansi_to_unicode(wstr, astr, length);
    if (len == 0) {
        u8str.clear();
        return 0;
    }

    return unicode_to_utf8(u8str, wstr.c_str(), wstr.length());
}

// UTF-8 to ANSI
size_t utf8_to_ansi(std::string& astr, const char* u8str, size_t length)
{
    std::wstring wstr;
    size_t len = utf8_to_unicode(wstr, u8str, length);
    if (len == 0) {
        astr.clear();
        return 0;
    }

    return unicode_to_ansi(astr, wstr.c_str(), wstr.length());
}

#else // MINIMGUI_PLATFORM_WINDOWS

// UTF-8 to UNICODE
size_t to_unicode(std::wstring& wstr, const unsigned char* u8str, size_t length)
{
    size_t wlen = utf8_to_utf32(u8str, length, nullptr, 0, nullptr);
    if (wlen == 0) {
        return 0;
    }
    wstr.resize(wlen);
    utf8_to_utf32(u8str, length, (uint32_t*) &wstr[0], wlen, nullptr);

    return wlen;
}

// UNICODE to UTF-8
size_t to_utf8(u8string& u8str, const wchar_t* wstr, size_t length)
{
    int u8len = utf32_to_utf8((const uint32_t*) wstr, length, nullptr, 0, nullptr);
    if (u8len == 0) {
        return 0;
    }

    u8str.resize(u8len);
    utf32_to_utf8((const uint32_t*) wstr, length, &u8str[0], u8len, nullptr);

    return u8len;
}

#endif // MINIMGUI_PLATFORM_WINDOWS

//------------------------------------------------------------------------------
// clang-format on

} // namespace minimgui
