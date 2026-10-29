/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_win32.h

 2026-03-21 07:11:21

*/
#ifndef UI_WIN32_H_20260321071121
#define UI_WIN32_H_20260321071121

#include "../ui.h"

#include <windows.h>

namespace minimgui {

int init_win32(HWND hwnd, HDC hdc);

// 按键映射
int key_map(int key);

} // namespace minimgui

#endif // UI_WIN32_H_20260321071121
