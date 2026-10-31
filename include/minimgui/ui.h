/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui.h

 2026-03-16 08:59:01

 部件的宽高计算:
 width      如果 (-1.0 <= w < 0)，数值按部件的容器区域宽高计算比例大小

*/
#ifndef UI_H_20260316085901
#define UI_H_20260316085901

#include "ui_std.h"
#include "ui_gfx.h"

namespace minimgui {

//
// 环境函数
//

// version()
void init_gui(); // 初始化
void shutdown(); // 关闭

//
// 消息处理
//

int on_resize(int width, int height);             // 窗口大小改变

int on_key_push(int key);                         // 按键按下
int on_key_pop(int key);                          // 按键弹起
int on_input(int ch);                             // 字符输入

int on_mouse_push(int x, int y, uiButton button); // 鼠标按下
int on_mouse_pop(int x, int y, uiButton button);  // 鼠标弹起
int on_mouse_move(int x, int y, uiButton button); // 鼠标移动
int on_scroll(int z);                             // 滚轮滚动

//
// 功能函数
//

bool begin_context(void* instance);  // 开始界面生成
void end_context(void* instance);    // 结束界面生成
void render(void* instance);         // 绘制

uiCommandList* window_cmdlist();     // 返回当前窗口的绘图列表
uiCommandList* background_cmdlist(); // 返回背景绘图列表
uiCommandList* foreground_cmdlist(); // 返回前景绘图列表

// 压入剪裁区域，intersect：是否和之前的剪裁区域相交，false 重新设置剪裁区域
void push_clip(float x, float y, float w, float h, bool intersect);

// 弹出剪裁区域
void pop_clip();

// 判断矩形是否在剪裁矩形内
bool is_visible_clipbox(const uiRect& rect);

// 布局函数
void sameline();               // 同一行
void indent(float i = 0.0f);   // 缩进
void unindent(float i = 0.0f); // 取消缩进

uiVec2 get_position();         // 获取当前部件位置
uiVec2 get_size();             // 获取当前部件大小
uiRect get_bounds();           // 获取当前部件矩形
bool is_hovered();             // 判断当前部件是否鼠标悬停

//
// 组件
//

// 开始窗口
bool begin_window(const uiChar* text, bool* isOpen, uiVec2 pos = uiVec2(), uiVec2 size = uiVec2(400.0f, 300.0f), uiFlags panelFlags = UI_FLAGS_NONE);
bool begin_window(const uiChar* text, uiVec2 pos = uiVec2(), uiVec2 size = uiVec2(400.0f, 300.0f), uiFlags panelFlags = UI_FLAGS_NONE);

// 结束窗口
void end_window();

// 开始子窗口
bool begin_child(const uiChar* text, uiFlags panelFlags = UI_FLAGS_NONE);

// 结束子窗口
void end_child();

// 折叠面板
bool collapsible_panel(const uiChar* text, uiFlags panelFlags = UI_FLAGS_NONE);

bool begin_menu_bar(const uiChar* text);                           // 菜单栏开始
void end_menu_bar();                                               // 菜单栏结束
bool begin_menu(const uiChar* text);                               // 菜单开始
void end_menu();                                                   // 菜单结束
bool menu_item(const uiChar* text);                                // 菜单项
void cancel_popup();                                               // 取消弹出窗口

bool button(const uiChar* text, uiVec2 size = uiVec2());           // 按钮
void color_box(uiColor color, float width = -1.0f);                // 显示颜色框
bool checkbox(const uiChar* text, bool* checked);                  // 单选框，ID 生成使用变量地址
bool checkbox(const uiChar* text, uint32_t* flags, uint32_t flag); // 标志选择框
bool edit(uiChar* buffer, size_t size);                            // 编辑框，ID 生成使用变量地址

// 图片框
bool image(const uiChar* text, uiHandle id, uiVec2 imageSize, uiVec2 size, uiFlags align = UI_FLAGS_NONE);

void label(const uiChar* param, ...);                                       // 标签页 (无鼠标事件)
void label(float width, const uiChar* param, ...);                          // 固定宽度标签页
void label(float width, uiAlign align, const uiChar* param, ...);           // 固定宽度标签页

bool selectable(const uiChar* text, bool selected, uiVec2 size = uiVec2()); // 可选择按钮
void separator(const uiChar* text = nullptr);                               // 分隔符 (无鼠标事件)
bool slider(const uiChar* text, float* value, float min, float max);        // 滑块，ID 生成使用变量地址

bool hscroll(const uiChar* text, float* value, float min, float max);       // 横向滚动条
bool vscroll(const uiChar* text, float* value, float min, float max);       // 竖向滚动条
bool hscroll(const uiChar* text, const uiRect& rect, float* value, float min, float max);
bool vscroll(const uiChar* text, const uiRect& rect, float* value, float min, float max);

bool begin_list(const uiChar* text);  // 开始列表框
void end_list();                      // 结束列表框
bool begin_combo(const uiChar* text); // 开始组合框
void end_combo();                     // 结束组合框

// 树节点
bool treenode(const uiChar* text, uiFlags nodeFlags = UI_FLAGS_NONE);
bool treenode(const uiChar* text, uiColor color, uiFlags nodeFlags = UI_FLAGS_NONE);
void tree_pop();

bool drag_int(float width, int* value, int min, int max);         // 整形拖拽框，ID 生成使用变量地址
bool drag_float(float width, float* value, float min, float max); // 浮点拖拽框，ID 生成使用变量地址

bool drag_color(uint8_t* color, float width = -1.0f);             // 颜色拖拽框，ID 生成使用变量地址
bool drag_color(float* color, float width = -1.0f);               // 浮点颜色拖拽框，ID 生成使用变量地址
bool drag_color(uiColor& color, float width = -1.0f);             // 颜色拖拽框

bool custom_test(const uiChar* text, float x, float y, float w, float h);

//
// 主题
//

uiStyle* style(); // 返回当前主题

void set_alpha_style(uiStyle* style);
void set_white_style(uiStyle* style);
void set_black_style(uiStyle* style);

// 辅助函数

uiVec2 pick_delta(float scale);                   // 采集鼠标移动向量
uiVec2i pick_delta_int_acc(int min, float scale); // 采集整形移动向量。因为向量是浮点数，值最少积累到 min 才返回。

} // namespace minimgui

#endif // UI_H_20260316085901
