/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_widgets.h

 2026-03-16 09:00:13

*/
#ifndef UI_WIDGETS_H_20260316090013
#define UI_WIDGETS_H_20260316090013

#include "ui.h"
#include "ui_gfx.h"
#include "ui_utils.h"

#include <list>
#include <map>

namespace minimgui {

// 部件
struct uiWidget
{
    uiWidgetType type; // 类型
    uiID id;           // hash ID
    uint32_t flags;    // 标志
    uiString text;     // 文本
    uiRect bounds;     // 矩形范围
    uiWidget* parent;  // 父部件

    // 默认构造函数
    uiWidget(uiWidgetType type = UI_WIDGET_NONE) : type(type), id(), flags(), text(), bounds(), parent() { }

    uiWidget(uiWidgetType type, const uiString& text, uiID id, uiWidget* parent = nullptr) :
        type(type),
        id(id ? id : ui_hash(text)), // 如果有 ID 就用 ID 初始化，否则使用文字初始化
        flags(),
        text(text),
        bounds(),
        parent(parent)
    {
    }
};

// 编辑框
struct uiEdit : public uiWidget
{
    uiChar* buffer;               // 字符缓冲区
    int bufferSize;               // 字符缓冲区大小

    int length;                   // 字符串长度
    int offset;                   // 字符串显示偏移量
    int selectStart;              // 选择开始
    int selectEnd;                // 选择结束

    std::vector<float> sizeCache; // 缓存的字符宽度，加速处理

    uiEdit() : uiWidget(UI_WIDGET_EDIT), buffer(), bufferSize(), length(), offset(), selectStart(), selectEnd() { }
    uiEdit(uiID id, uiWidget* parent = nullptr) :
        uiWidget(UI_WIDGET_EDIT, uiString(), id, parent), buffer(), bufferSize(), length(), offset(), selectStart(), selectEnd() { }
};

// 面板
struct uiPanel : public uiWidget
{
    uint32_t panelFlags; // 面板标志
    uiBounds slot;       // 槽位
    uiVec2 scroll;       // 滚动条
    uiVec2 scrollMax;    // 滚动条范围
    uiID focusWidget;    // 输入焦点部件
    int data;            // 用户数据

    // 默认构造函数
    uiPanel(uiFlags flags = UI_PANEL_DEFAULT) :
        uiWidget(UI_WIDGET_PANEL),
        panelFlags(flags), slot(), scroll(), scrollMax(), focusWidget(), data() { }

    uiPanel(uiWidgetType type, const uiString& text, uiID id, uiWidget* parent = nullptr, uiFlags flags = UI_PANEL_DEFAULT) :
        uiWidget(type, text, id, parent),
        panelFlags(flags), slot(), scroll(), scrollMax(), focusWidget(), data() { }
};

// 部件列表
typedef std::vector<uiWidget*> uiWidgetList;

// 窗口，所有窗口必须是顶层
struct uiWindow : public uiPanel
{
    uiCommandList cmdlist; // 绘图列表
    uiWidgetList widgets;  // 部件列表，用于需要缓存属性的部件

    union
    {
        uint64_t param;
        bool* pIsOpen;
        struct
        {
            float w, h;
        } popupSize; // 最后一次统计的弹出窗口大小
    };

    // 默认构造函数
    uiWindow(uiFlags panelFlags = UI_WINDOW_DEFAULT) :
        uiPanel(UI_WIDGET_WINDOW, uiString(), 0, nullptr, panelFlags),
        cmdlist(), widgets(), param() { }

    uiWindow(uiWidgetType type, const uiString& text, uiID id, uiFlags panelFlags = UI_WINDOW_DEFAULT) :
        uiPanel(type, text, id, nullptr, panelFlags),
        cmdlist(), widgets(), param() { }
};


// 布局，描述当前容器布局信息
struct uiLayout
{
    uiPanel* container;    // 当前容器
    uiWindow* owner;       // 当前窗口
    uintptr_t data;        // 用户数据

    uiRect contentRect;    // 容器区域矩形
    uiVec2 padding;        // 边距
    uiVec2 space;          // 间距
    uiVec2 scrollMax;      // 统计滚动范围

    uiVec2 nextWidgetPos;  // 下一个部件位置
    uiVec2 nextWidgetSize; // 下一个部件大小

    bool sameline;         // 是否同一行
    float lineHeight;      // 行高，同行最高部件的高度

    uiRect lastRect;       // 最后申请的矩形区域

    uiLayout()
    {
        this->reset();
    }

    // 重置
    void reset()
    {
        container = nullptr;
        owner     = nullptr;
        data      = 0;

        contentRect = uiRect();
        padding     = uiVec2(0.0f);
        space       = uiVec2(0.0f);
        scrollMax   = uiVec2(0.0f);

        nextWidgetPos  = uiVec2(0.0f);
        nextWidgetSize = uiVec2(0.0f);

        sameline   = false;
        lineHeight = 0.0f;
    }

    // 返回剩余的宽度
    float remaining_width()
    {
        return contentRect.w - nextWidgetPos.x;
    }

    // 扩大
    void expand(float cx, float cy)
    {
        container->bounds.w += cx;
        container->bounds.h += cy;
        contentRect.w += cx;
        contentRect.h += cy;
    }
};

// 环境上下文
struct uiContext
{
    uiFlags debugFlags;     // 调试参数

    uiStyle style;          // 主题

    uiVec2 screen;          // 屏幕大小
    uiRect clipRect;        // 缓存当前剪裁矩形

    uiVec2 mouse;           // 当前鼠标坐标
    uiVec2 lastpos;         // 最后鼠标坐标
    uiButton button;        // 鼠标按键
    uiVec2 delta;           // 鼠标从按下开始积累的移动向量
    float scroll;           // 滚轴数值

    uiVec2 clickPos;        // 鼠标点击的位置，一帧之后重置
    int clicked;            // 鼠标是否点击。点击空白区域设置 -1，点击窗口、部件设置 1，没有点击设置 0。

    int key;                // 当前帧按键输入，处理过后要清零 (这两个都没有做缓存处理)
    int ch;                 // 当前帧字符输入，处理过后要清零

    uiID hoveredWindow;     // 悬停窗口
    uiID activedWindow;     // 激活窗口
    uiID focusWindow;       // 焦点窗口

    uiID hoveredWidget;     // 悬停部件
    uiID lastHoveredWidget; // 最后悬停部件，用于处理鼠标进入和离开
    uiID clickedWidget;     // 鼠标点击的窗口、部件
    uiID popupWidget;       // 当前弹出部件

    uiFlags widgetFlags;    // 当前部件的标志，用来公开查询部件状态


    uiCommandList backgroundCmdList; // 背景绘图列表
    uiCommandList foregroundCmdList; // 前景绘图列表


    uiWindow mainWindow;            // 主窗口 (桌面)
    uiLayout mainLayout;            // 主窗口布局

    std::vector<uiWindow*> windows; // 窗口列表

    // 布局堆栈。开始一个窗口的时候，先压入窗口布局；遇到子容器的时候，再把子容器压进去。
    std::vector<uiLayout> layouts;
    float indent; // 缩进

    // 缓存的状态
    std::map<uiID, uint32_t> stateCache;

    std::vector<uiID> openPath;  // 弹出窗口打开路径链
    std::vector<uiID> menuStack; // 弹出窗口打开路径链

    // 弹出窗口堆栈
    std::vector<uiWindow*> popupStack;
};

//
// 内部函数
//

uiContext* context();                               // 返回当前环境

void init_layout(uiLayout* layout, uiPanel* panel); // 初始化布局
uiLayout* push_layout(uiPanel* panel);              // 压入布局
void pop_layout();                                  // 弹出布局
uiLayout* layout();                                 // 返回当前布局

void push_clip(uiPanel* panel, bool intersect);     // 压入剪裁矩形
void pop_clip(uiPanel* panel);                      // 弹出剪裁矩形

/* 检查窗口是否产生鼠标消息
 * 这里不处理鼠标悬停、点击，无法处理窗口 Z 顺序问题
 * 鼠标悬停、点击统一在鼠标事件里面，根据窗口 Z 顺序处理。
 */
uiFlags check_window_mouse_event(uiWindow* window);

// 检查部件是否产生鼠标消息
// 这里会处理部件的悬停、点击事件，如果发生了点击行为，会激活父窗口。
uiFlags check_widget_mouse_event(uiWidget* widget, bool allowActive = true);


//
// 部件
//

// uiWindow* current_window();                         // 返回当前窗口
uiWindow* find_window(const uiString& title);              // 查找窗口
uiWindow* find_window_by_id(uiID id);                      // 根据 ID 查找窗口
size_t find_window(uiWindow* window);                      // 查找窗口指针，返回窗口索引
bool is_actived(uiWindow* window);                         // 判断窗口是否激活
void set_active(uiWindow* window);                         // 设置激活窗口
void set_active(size_t index);
void bring_to_front(uiWindow* window);                     // 把窗口放到最前面
void bring_to_back(uiWindow* window);                      // 把窗口放到最后面
uiWindow* select_window_from_position(const uiVec2& pos);  // 查找坐标位置的窗口
void on_root_window_click();                               // 主窗口点击
uiRect compute_clip_rect(uiPanel* panel);                  // 计算剪裁矩形
uiRect compute_content_rect(uiPanel* panel, bool padding); // 计算容器区域矩形 (客户区)

// 分配部件矩形，宽高是部件大小期望值
uiRect allocate_widget_rect(float width, float height, bool forceNewLine = false);

// 分配整行部件矩形，无 padding
uiRect allocate_widget_rect_full_line(float height);

// 分配槽位空间
uiRect allocate_slot_area(float size, uiFlags position);

// 调整矩形弹出坐标
uiVec2 adjust_popup_position(const uiRect& widgetRect, const uiVec2& size, uiDirection dir);

// 调整菜单弹出坐标
uiVec2 adjust_main_menu_popup_position(const uiRect& widgetRect, const uiVec2& size);
uiVec2 adjust_sub_menu_popup_position(const uiRect& widgetRect, const uiVec2& size);

//
// 部件
//

bool is_container(uiWidget* widget);         // 判断部件是否容器
bool is_window(uiWidget* widget);            // 判断部件是否窗口
uiWindow* get_owner(uiWidget* widget);       // 返回部件的所有者窗口
uiWidget* find_child(const uiString& title); // 查找子部件
uiWidget* find_child_by_id(uiID id);         // 根据 ID 查找子部件
bool is_enabled(uiWidget* widget);           // 判断部件是否激活
bool check_hover(uiWidget* widget);          // 检查部件是否鼠标悬停
bool is_hovered(uiWidget* widget);           // 判断部件是否鼠标悬停
bool is_pressed(uiWidget* widget);           // 判断部件是否鼠标按下
void set_clicked_widget(uiWidget* widget);   // 设置鼠标点击部件
bool is_capture(uiWidget* widget);           // 判断部件是否鼠标捕获
bool is_focus(uiWidget* widget);             // 判断部件是否输入焦点
void set_focus(uiWidget* widget);            // 设置焦点部件
bool is_visible_clipbox(uiWidget* widget);   // 判断部件是否在剪裁矩形内

// 返回部件类型名称
const uiChar* widget_type_to_string(uiWidgetType type);

// 获取部件缓存的状态，这个状态保存在 context 里面，是全局的
uiFlags& pick_cache_flags(uiID id, uiFlags init);

//
// 绘制
//

// 选择颜色
uiColor select_color(uiColor* colorMode, bool hovered, bool selected, bool disabled = false);

// 选择部件背景色
// uiColor select_widget_background(bool hovered, bool selected, bool disabled = false);

// 选择部件前景色
// uiColor select_widget_foreground(bool hovered, bool selected, bool disabled = false);

// 选择文本背景色
uiColor select_text_background(bool hovered, bool selected, bool disabled = false);

// 选择文本前景色
uiColor select_text_foreground(bool hovered, bool selected, bool disabled = false);

// 绘制框架
void draw_frame(uiCommandList* cmdlist, const uiRect& rect, const uiColor& border, const uiColor& background);
void draw_frame(uiCommandList* cmdlist, uiWidget* widget, const uiColor& border, const uiColor& background);

// 绘制窗口
void draw_window(uiCommandList* cmdlist, uiWindow* window);

// 绘制折叠面板
void draw_collapsible(uiCommandList* cmdlist, uiWidget* widget, bool isCollapsed);

// 绘制菜单窗口
void draw_menu_window(uiCommandList* cmdlist, uiWindow* window);

// 绘制菜单项
void draw_menu_item(uiCommandList* cmdlist, uiWidget* widget, bool hovered, bool checked, bool hasSubMenu);

// 绘制标签
void draw_label(uiCommandList* cmdlist, const uiRect& rect, const uiString& text, uiAlign align, uiColor textcolor, uiColor foreground, uiColor background);
void draw_label(uiCommandList* cmdlist, uiWidget* widget, uiAlign align, uiColor textcolor, uiColor foreground, uiColor background);

// 绘制按钮
void draw_button(uiCommandList* cmdlist, uiWidget* widget);

// 绘制标签
void draw_check_box(uiCommandList* cmdlist, uiWidget* widget, bool checked);

// 绘制组合框
void draw_combo(uiCommandList* cmdlist, uiWidget* widget, bool opened);

// 绘制滑块
void draw_slider(uiCommandList* cmdlist, uiWidget* widget, float value, float min, float max);

// 绘制横向滚动条
void draw_hscroll(uiCommandList* cmdlist, uiWidget* widget, float value, float sliderPos, float sliderSize);

// 绘制竖向滚动条
void draw_vscroll(uiCommandList* cmdlist, uiWidget* widget, float value, float sliderPos, float sliderSize);

// 绘制分隔符
void draw_separator(uiCommandList* cmdlist, const uiRect& rect, const uiString& text);

// 绘制树节点
void draw_treenode(uiCommandList* cmdlist, uiWidget* widget, bool opened, uiColor textColor);

} // namespace minimgui

#endif // UI_WIDGETS_H_20260316090013
