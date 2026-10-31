#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// 返回窗口句柄
HWND window();

// 初始化窗口
HWND init_window(LPCTSTR title, int width, int height, bool scalable = true);

// 判断程序是否运行
bool running();

// 执行消息循环
void do_events();

// 运行程序
int run_app();

//
// 窗口事件
//

// 窗口缩放事件
void reshape(int width, int height);

// 按键按下
void key_push(int key);

// 按键弹起
void key_pop(int key);

// 字符输入
void input(int ch);

// 鼠标按下
void mouse_push(int x, int y, int button);

// 鼠标弹起
void mouse_pop(int x, int y, int button);

// 鼠标移动
void mouse_move(int x, int y);

// 滚轮滚动
void scroll(int x, int y, int z);

// 计时器更新事件
void update(float delay);

// 绘图事件
void display(HDC hdc);

//
// 绘图函数
//

// 清屏
void clear(COLORREF color);

//
// 辅助函数
//

// 显示消息对话框
void msgbox(PCTSTR param, ...);

#endif // WIN32_WINDOW_H
