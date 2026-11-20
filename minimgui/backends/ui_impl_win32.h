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

int init_win32(HWND hwnd, bool installWndProc);

// 消息处理函数
LRESULT CALLBACK win32_wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 按键映射 VKey -> UI Key
int key_map(int key);

// 按键映射 UI Keys -> VKey
int key_unmap(int key);

} // namespace minimgui

#endif // UI_WIN32_H_20260321071121
