/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_font.h

 2026-10-31 18:02:20

*/
#ifndef UI_FONT_H_20261031180220
#define UI_FONT_H_20261031180220

#include "ui_std.h"

#include <glad/gl4.6.h>

namespace minimgui {

struct uiFont;

// 加载字体
uiFont* load_font(const char* fontFile, float size);

// 从内存加载字体
uiFont* load_font(const char* fontName, const void* fontData, float size);

// 获取当前字体
uiFont* get_font();

// 设置当前字体
void set_font(uiFont* font);

// 获得字符宽度
float char_width1(wchar_t ch);

// 获取字符高度
float char_height1();

// 获取字符串宽度 (UTF-8)
float text_width1(const unsigned char* text, size_t length);

// 输出单行文字
void textout1(float x, float y, const char* str, size_t length);

} // namespace minimgui

#endif // UI_FONT_H_20261031180220
