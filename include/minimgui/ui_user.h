/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_user.h

 2026-10-21 17:31:39

*/
#ifndef UI_USER_H_20261021173139
#define UI_USER_H_20261021173139

#include "ui.h"

namespace minimgui {

//
// 界面 API 需要实现的接口
//

// 判断按键是否按下
bool key_state(int key);

//
// 绘图 API 需要实现的接口
//

// 初始化界面环境
void on_begin_context(void* instance);

// 结束界面环境
void on_end_context(void* instance);

// 初始化绘制
void on_begin_draw(void* instance);

// 结束绘制
void on_end_draw(void* instance);

// 绘制命令列表
void draw_cmd(const uiCommandList& cmdlist, void* instance);

// 返回字符宽度
float char_width(int ch);

// 返回字符高度
float char_height();

// 返回文字宽度
float text_width(const uiChar* text, size_t length);

} // namespace minimgui

#endif // UI_USER_H_20261021173139
