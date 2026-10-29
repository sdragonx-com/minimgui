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

} // namespace minimgui

#endif // UI_IMPL_GL2_H_20260321071103
