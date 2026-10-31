/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_std.h

 2026-03-16 08:58:40

*/
#ifndef UI_STD_H_20260316085840
#define UI_STD_H_20260316085840

#ifndef UNICODE
    #define UNICODE
    #define _UNICODE
#endif

#if defined(min) || defined(max)
    #pragma message "if you include <windows.h>, please define NOMINMAX first."
#endif

#include <float.h>
#include <math.h>
#include <tchar.h>

// std
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "ui_math.h"

namespace minimgui {

// 字符
#if defined(UNICODE) || defined(_UNICODE)
typedef wchar_t uiChar;
#else
// #define UI_USE_UTF8
typedef char uiChar;
#endif

typedef std::basic_string<uiChar> uiString;

// 句柄
typedef uint64_t uiHandle;

// hashID
typedef uint64_t uiID;

// 标志 DI
typedef uint32_t uiFlags;

// 颜色
typedef glm::u8vec4 uiColor;

// clang-format off

namespace colors {
    static const uiColor black   = uiColor(  0,   0,   0, 255); // #808080
    static const uiColor gray    = uiColor(128, 128, 128, 255); // #808080
    static const uiColor red     = uiColor(255,   0,   0, 255); // #FF0000
    static const uiColor yellow  = uiColor(255, 255,   0, 255); // #FFFF00
    static const uiColor lime    = uiColor(  0, 255,   0, 255); // #00FF00
    static const uiColor cyan    = uiColor(  0, 255, 255, 255); // #00FFFF
    static const uiColor blue    = uiColor(  0,   0, 255, 255); // #00FFFF
    static const uiColor magenta = uiColor(255,   0, 255, 255); // #FF00FF
    static const uiColor maroon  = uiColor(128,   0,   0, 255); // #800000
    static const uiColor olive   = uiColor(128, 128,   0, 255); // #808000
    static const uiColor green   = uiColor(  0, 128,   0, 255); // #008000
    static const uiColor teal    = uiColor(  0, 128, 128, 255); // #008080
    static const uiColor navy    = uiColor(  0,   0, 128, 255); // #000080
    static const uiColor purple  = uiColor(128,   0, 128, 255); // #800080
    static const uiColor silver  = uiColor(192, 192, 192, 255); // #C0C0C0
    static const uiColor white   = uiColor(255, 255, 255, 255); // #FFFFFF
} // namespace colors

// RGBA 颜色
inline uint32_t RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return uint32_t((uint8_t(r) | (uint16_t(uint8_t(g)) << 8)) | (uint32_t(uint8_t(b)) << 16) | (uint32_t(uint8_t(a)) << 24));
}
inline uint8_t RGBA_RED   (uint32_t color) { return static_cast<uint8_t>(color      ); }
inline uint8_t RGBA_GREEN (uint32_t color) { return static_cast<uint8_t>(color >> 8 ); }
inline uint8_t RGBA_BLUE  (uint32_t color) { return static_cast<uint8_t>(color >> 16); }
inline uint8_t RGBA_ALPHA (uint32_t color) { return static_cast<uint8_t>(color >> 24); }

// 交换 R 和 B 通道
inline uint32_t RedBlueSwap(uint32_t color) {
    return (color & 0xFF00FF00) | ((color >> 16) & 0x000000FF) | ((color << 16) & 0x00FF0000);
}

// clang-format on

// 无效坐标
const uiVec2 UI_INVALID_POS = uiVec2(-FLT_MAX, -FLT_MAX);

// 方位，对齐
typedef enum uiDirection
{
    UI_LEFT  = 1,                            // 左
    UI_RIGHT = 2,                            // 右
    UI_UP    = 4,                            // 上
    UI_DOWN  = 8,                            // 下

    UI_TOP    = UI_UP,                       // 顶部
    UI_BOTTOM = UI_DOWN,                     // 底部

    UI_CENTER_H = UI_LEFT | UI_RIGHT,        // 水平居中
    UI_CENTER_V = UI_UP | UI_DOWN,           // 垂直居中
    UI_CENTER   = UI_CENTER_H | UI_CENTER_V, // 居中
} uiAlign, uiDirection;

// clang-format off
// 按键
enum uiKey
{
    UI_KEY_NONE            = 0,

    UI_KEY_BACK            = 8,    // '\b' back
    UI_KEY_TAB             = 9,    // '\t' tabulator
    UI_KEY_ENTER           = 13,   // '\r' enter, return
    UI_KEY_SHIFT           = 14,   // shift
    UI_KEY_CONTROL         = 15,   // ctrl
    UI_KEY_ALT             = 16,   // alt
    UI_KEY_META            = 17,   // meta [win(windows), command(mac)]
    UI_KEY_MENU            = 18,   // menu

    UI_KEY_PRINT_SCREEN    = 19,   // print screen, snapshot
    UI_KEY_PAUSE           = 20,   // pause, break

    UI_KEY_HOME            = 21,   // home
    UI_KEY_END             = 22,   // end
    UI_KEY_PAGE_UP         = 23,   // previous
    UI_KEY_PAGE_DOWN       = 24,   // next
    UI_KEY_INSERT          = 25,   // insert
    UI_KEY_DELETE          = 26,   // delete

    UI_KEY_ESCAPE          = 27,   // escape

    UI_KEY_LEFT            = 28,   // left
    UI_KEY_UP              = 29,   // up
    UI_KEY_RIGHT           = 30,   // right
    UI_KEY_DOWN            = 31,   // down

    UI_KEY_SPACE           = 32,   // ' '  space
    UI_KEY_EXCLAM          = 33,   // !    exclamation mark
    UI_KEY_DOUBLE_QUOTE    = 34,   // "
    UI_KEY_NUMBER_SIGN     = 35,   // #    number sign
    UI_KEY_DOLLAR          = 36,   // $    dollar sign
    UI_KEY_PERCENT_SIGN    = 37,   // %
    UI_KEY_AMPERSAND       = 38,   // &
    UI_KEY_APOSTROPHE      = 39,   // '    quote 单引号
    UI_KEY_LEFT_PAREN      = 40,   // (    round brackets
    UI_KEY_RIGHT_PAREN     = 41,   // )
    UI_KEY_ASTERISK        = 42,   // *

    UI_KEY_PLUS            = 43,   // +
    UI_KEY_COMMA           = 44,   // ,
    UI_KEY_MINUS           = 45,   // -
    UI_KEY_PERIOD          = 46,   // .
    UI_KEY_SLASH           = 47,   // /

    // 0 ~ 9 [48 ~ 57]
    UI_KEY_0 = 48, UI_KEY_1, UI_KEY_2, UI_KEY_3, UI_KEY_4,
    UI_KEY_5, UI_KEY_6, UI_KEY_7, UI_KEY_8, UI_KEY_9,

    UI_KEY_COLON           = 58,   // :
    UI_KEY_SEMICOLON       = 59,   // ;
    UI_KEY_LESS            = 60,   // <    angle brackets
    UI_KEY_EQUAL           = 61,   // =
    UI_KEY_GREATER         = 62,   // >
    UI_KEY_QUESTION        = 63,   // ?
    UI_KEY_AT              = 64,   // @

    // A ~ Z [65 ~ 90]
    UI_KEY_A = 65, UI_KEY_B, UI_KEY_C, UI_KEY_D, UI_KEY_E, UI_KEY_F, UI_KEY_G,
    UI_KEY_H, UI_KEY_I, UI_KEY_J, UI_KEY_K, UI_KEY_L, UI_KEY_M, UI_KEY_N,
    UI_KEY_O, UI_KEY_P, UI_KEY_Q, UI_KEY_R, UI_KEY_S, UI_KEY_T,
    UI_KEY_U, UI_KEY_V, UI_KEY_W, UI_KEY_X, UI_KEY_Y, UI_KEY_Z,

    UI_KEY_LEFT_BRACKET    = 91,   // [    square brackets
    UI_KEY_BACKSLASH       = 92,   // '\'  backslash
    UI_KEY_RIGHT_BRACKET   = 93,   // ]
    UI_KEY_CARET           = 94,   // ^
    UI_KEY_UNDERSCORE      = 95,   // _
    UI_KEY_GRAVE_ACCENT    = 96,   // `    重音符号

    // a ~ z [97 ~ 122]

    UI_KEY_LEFT_BRACE      = 123,  // {    french brackets
    UI_KEY_BAR             = 124,  // |    vertical bar
    UI_KEY_RIGHT_BRACE     = 125,  // }
    UI_KEY_TILDE           = 126,  // ~    波浪号
    UI_KEY_BACKSPACE       = 127,  // backspace

    // 扩展功能键
    UI_KEY_FUNCTION        = 128,

    UI_KEY_LEFT_SHIFT      = UI_KEY_FUNCTION + 16, // 144,
    UI_KEY_RIGHT_SHIFT,
    UI_KEY_LEFT_CONTROL,
    UI_KEY_RIGHT_CONTROL,
    UI_KEY_LEFT_ALT,
    UI_KEY_RIGHT_ALT,
    UI_KEY_LEFT_META,
    UI_KEY_RIGHT_META,

    UI_KEY_CAPS_LOCK,      // capslock
    UI_KEY_SCROLL_LOCK,    // scroll lock
    UI_KEY_NUM_LOCK,       // numlock

    // Fn 键
    UI_KEY_F1 = UI_KEY_FUNCTION + 64, UI_KEY_F2, UI_KEY_F3, UI_KEY_F4, UI_KEY_F5, UI_KEY_F6,
    UI_KEY_F7, UI_KEY_F8, UI_KEY_F9, UI_KEY_F10, UI_KEY_F11, UI_KEY_F12,
    UI_KEY_F13, UI_KEY_F14, UI_KEY_F15, UI_KEY_F16, UI_KEY_F17, UI_KEY_F18,
    UI_KEY_F19, UI_KEY_F20, UI_KEY_F21, UI_KEY_F22, UI_KEY_F23, UI_KEY_F24,

    // 数字小键盘 [224+]
    UI_KEY_KP_0 = UI_KEY_FUNCTION + 96, UI_KEY_KP_1, UI_KEY_KP_2, UI_KEY_KP_3, UI_KEY_KP_4,
    UI_KEY_KP_5, UI_KEY_KP_6, UI_KEY_KP_7, UI_KEY_KP_8, UI_KEY_KP_9,
    UI_KEY_KP_DECIMAL,     // .
    UI_KEY_KP_DIVIDE,      // /
    UI_KEY_KP_MULTIPLY,    // *
    UI_KEY_KP_SUBTRACT,    // -
    UI_KEY_KP_ADD,         // +
    UI_KEY_KP_ENTER,       // enter
    UI_KEY_KP_EQUAL,       // =

    UI_KEY_LAST            = 0xFF,
};
// clang-format on

enum uiButton
{
    UI_BUTTON_NONE   = -1,
    UI_BUTTON_LEFT   = 0,
    UI_BUTTON_RIGHT  = 1,
    UI_BUTTON_MIDDLE = 2,
};

// 部件类型
enum uiWidgetType
{
    UI_WIDGET_NONE,                    // 无
    UI_WIDGET_MASK_CONTAINER = 0x8000, // 容器
    UI_WIDGET_MASK_WINDOW    = 0x4000, // 容器

    // 窗口 (uiWindow)
    UI_WIDGET_WINDOW = 0 | UI_WIDGET_MASK_CONTAINER | UI_WIDGET_MASK_WINDOW,

    // 面板 (uiPanel)
    UI_WIDGET_PANEL = 1 | UI_WIDGET_MASK_CONTAINER,

    // 容器
    UI_WIDGET_MENUBAR     = 10 | UI_WIDGET_MASK_CONTAINER, // 菜单栏 (uiPanel)
    UI_WIDGET_MENU        = 11 | UI_WIDGET_WINDOW,         // 菜单 (uiWindow)
    UI_WIDGET_STATUSBAR   = 12 | UI_WIDGET_MASK_CONTAINER, // 状态栏
    UI_WIDGET_TOOLBAR     = 13 | UI_WIDGET_MASK_CONTAINER, // 工具栏
    UI_WIDGET_COLLAPSIBLE = 14 | UI_WIDGET_MASK_CONTAINER, // 折叠面板

    // 部件
    UI_WIDGET_BUTTON = 1,  // 按钮
    UI_WIDGET_COLOR,       // 颜色框
    UI_WIDGET_CHECKBOX,    // 选择框
    UI_WIDGET_DRAG_NUMBER, // 拖拽数字
    UI_WIDGET_EDIT,        // 单行编辑框
    UI_WIDGET_IMAGE,       // 图片框
    UI_WIDGET_LABEL,       // 文本框
    UI_WIDGET_MENU_ITEM,   // 菜单项
    UI_WIDGET_SCROLL,      // 滚动条
    UI_WIDGET_SLIDER,      // 滑块
    UI_WIDGET_TREENODE,    // 树节点

    UI_WIDGET_CUSTOM,      // 自定义
};

// 部件标志
enum uiWidgetFlags
{
    UI_FLAGS_NONE     = 0x0000, // 无
    UI_FLAGS_SELECTED = 0x0002, // 选择
    UI_FLAGS_DISABLED = 0x0004, // 禁用
    UI_FLAGS_CHECKED  = 0x0008, // 选定
    UI_FLAGS_DOWN     = 0x0010, // 按下
    UI_FLAGS_OPEN     = 0x0020, // 打开

    // 即时标志
    UI_FLAGS_HOVERED = 0x00010000, // 悬停
    UI_FLAGS_CLICKED = 0x00020000, // 点击
    // UI_FLAGS_PRESSED = 0x00040000, // 鼠标按下
};

// 容器状态
enum uiPanelFlags
{
    // 样式状态
    UI_PANEL_TITLE     = 0x0001, // 启用标题栏
    UI_PANEL_MENU      = 0x0002, // 启用菜单栏
    UI_PANEL_SCROLL    = 0x0004, // 启用滚动条
    UI_PANEL_BORDER    = 0x0008, // 启用边框
    UI_PANEL_PADDING   = 0x0010, // 启用部件边距
    UI_PANEL_SPACE     = 0x0020, // 启用部件间距
    UI_PANEL_RESIZABLE = 0x0040, // 是否可以缩放大小
    UI_PANEL_COLLAPSE  = 0x0080, // 是否折叠
    UI_PANEL_POPUP     = 0x0100, // 弹出部件
    // UI_PANEL_VISIBLE   = 0x0200, // 可见 (针对事件处理)

    // 面板默认值
    UI_PANEL_DEFAULT = UI_PANEL_BORDER | UI_PANEL_PADDING | UI_PANEL_SPACE | UI_PANEL_SCROLL,

    // 窗口默认值
    UI_WINDOW_DEFAULT = UI_PANEL_TITLE | UI_PANEL_BORDER | UI_PANEL_PADDING | UI_PANEL_SPACE | UI_PANEL_SCROLL | UI_PANEL_RESIZABLE,

    // 即时状态
    UI_PANEL_SIZING_X = 0x00010000, // 宽度缩放
    UI_PANEL_SIZING_Y = 0x00020000, // 高度缩放
    UI_PANEL_SIZING   = UI_PANEL_SIZING_X | UI_PANEL_SIZING_Y,
};

// 树节点状态
enum uiTreeNodeFlags
{
    UI_FLAGS_FULL_ROW_CLICK = 0x0100, // 整行点击
    UI_FLAGS_TRACKING       = 0x0200, // 鼠标追踪
};

//
// 主题
//

// 颜色模式
enum uiColorMode
{
    UI_COLOR_NORMAL    = 0, // 普通
    UI_COLOR_HOTLIGHT  = 1, // 热点，鼠标悬停
    UI_COLOR_HIGHLIGHT = 2, // 高亮，选中、激活、拥有输入焦点，或者按钮按下
    UI_COLOR_DISABLE   = 3, // 禁用，灰色
    UI_COLOR_MODES          // 颜色模式数量
};

// 样式
struct uiStyle
{
    float titleHeight;                        // 标题栏高度
    float borderWidth;                        // 边框宽度
    float triangleSize;                       // 窗口右下角三角形大小

    uiVec2 windowPadding;                     // 窗口边界空白
    uiVec2 framePadding;                      // 部件边框空白
    uiVec2 itemSpace;                         // 部件间距

    float menuHeight;                         // 菜单高度
    uiVec2 menuPadding;                       // 菜单边框空白

    float sliderSize;                         // 滑块大小
    float scrollWidth;                        // 滚动条宽度
    float scrollSliderMin;                    // 滚动条滑块最小大小

    uiColor titleBackground[UI_COLOR_MODES];  // 标题栏背景色
    uiColor titleForeground[UI_COLOR_MODES];  // 标题栏前景色

    uiColor windowBackground;                 // 窗口背景色
    uiColor windowForeground[UI_COLOR_MODES]; // 窗口前景色

    uiColor widgetBackground[UI_COLOR_MODES]; // 部件背景色
    uiColor widgetForeground[UI_COLOR_MODES]; // 部件前景色

    // uiColor buttonBackground[UI_COLOR_MODES]; // 按钮背景色

    uiColor trackBackground;                 // 轨道，滚动条、滑块背景
    uiColor thumbForeground[UI_COLOR_MODES]; // 滑块

    uiColor background[UI_COLOR_MODES];      // 背景色 (文字、项...)
    uiColor foreground[UI_COLOR_MODES];      // 前景色 (文字、项...)

    uiStyle();
};

// load file to buffer
template<typename T>
int load_file(std::vector<T>& buf, const std::string& filename)
{
    std::ifstream in;
    in.open(filename.c_str(), std::ios::binary);
    if (in.is_open()) {
        in.seekg(0, std::ios::end);
        size_t size = static_cast<size_t>(in.tellg());
        buf.resize(size / sizeof(T));
        in.seekg(0, std::ios::beg);
        in.read(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(T));
        return static_cast<int>(in.gcount());
    }
    else {
        std::cout << "load file \"" << filename << "\" failed!" << std::endl;
        return 0;
    }
}

} // namespace minimgui

#endif // UI_STD_H_20260316085840
