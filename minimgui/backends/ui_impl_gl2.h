/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_impl_gl2.h

 2026-03-21 07:11:03

*/
#ifndef UI_IMPL_GL2_H_20260321071103
#define UI_IMPL_GL2_H_20260321071103

#include "../ui_std.h"
#include "../ui_user.h"

namespace minimgui {

// 初始化 OpenGL 实例环境
void init_gl2(void* instance);

// 释放 OpenGL 实例环境
void dispose_gl2(void* instance);

float char_width(int ch);                            // 返回字符宽度
float char_height();                                 // 返回字符高度
float text_width(const char* str, size_t length);    // 返回文字宽度
float text_width(const wchar_t* str, size_t length); //

uiHandle create_image(int width, int height);        // 创建图片 (RGBA)
uiHandle load_image(const char* filename);           // 加载图片
uiHandle load_image(const wchar_t* filename);        //
void delete_image(uiHandle image);                   // 删除图片

} // namespace minimgui

#endif // UI_IMPL_GL2_H_20260321071103
