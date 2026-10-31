/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_impl_gdi.h

 2026-03-21 07:09:32

*/
#ifndef UI_IMPL_GDI_H_20260321070932
#define UI_IMPL_GDI_H_20260321070932

#include "../ui_std.h"

#include <windows.h>

namespace minimgui {

std::wstring to_wstring(const char* str, int length); // 转 UNICODE 字符串
std::wstring to_wstring(const std::string& str);      //

float char_width(int ch);                             // 返回字符宽度
float char_height();                                  // 返回字符高度
float text_width(const char* str, size_t length);     // 返回文字宽度
float text_width(const wchar_t* str, size_t length);  //

uiHandle create_image(int width, int height);         // 创建图片 (RGBA)
uiHandle load_image(const char* filename);            // 加载图片
uiHandle load_image(const wchar_t* filename);         //
void delete_image(uiHandle image);                    // 删除图片

// 用 AlphaBlend 填充半透明矩形
BOOL fill_rect_alpha(HDC hdc, int x, int y, int w, int h, uiColor color);

} // namespace minimgui

#endif // UI_IMPL_GDI_H_20260321070932
