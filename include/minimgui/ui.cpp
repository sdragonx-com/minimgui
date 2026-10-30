#include "ui.h"
#include "ui_widgets.h"
#include "ui_user.h"

#include <ctype.h>

namespace minimgui {

//------------------------------------------------------------------------------
// 变量
//------------------------------------------------------------------------------

uiContext m_ui_context;

static int _m_push_count = 0;
static int _m_pop_count  = 0;

//------------------------------------------------------------------------------
// 框架
//------------------------------------------------------------------------------

// 初始化
void init_gui()
{
    m_ui_context.debugFlags = UI_FLAGS_NONE;

    m_ui_context.screen = uiVec2(1280.0f, 720.0f);

    m_ui_context.mouse   = uiVec2();
    m_ui_context.lastpos = uiVec2();
    m_ui_context.button  = UI_BUTTON_NONE;
    m_ui_context.delta   = uiVec2();
    m_ui_context.scroll  = 0.0f;

    m_ui_context.clickPos = UI_INVALID_POS;
    m_ui_context.clicked  = 0;

    m_ui_context.key = 0;
    m_ui_context.ch  = 0;

    m_ui_context.hoveredWindow = 0;
    m_ui_context.activedWindow = 0;
    m_ui_context.focusWindow   = 0;

    m_ui_context.hoveredWidget = 0;
    m_ui_context.clickedWidget = 0;
    m_ui_context.popupWidget   = 0;

    // 动态设置标题栏高度
    // m_ui_context.style.titleHeight = char_height("A") + m_ui_context.style.framePadding.y * 2.0f;

    m_ui_context.mainWindow.panelFlags = UI_PANEL_MENU;
    m_ui_context.mainWindow.text       = _T("Root");

    // 菜单堆栈
    m_ui_context.openPath.reserve(64);
    m_ui_context.menuStack.reserve(64);
}

// 关闭
void shutdown()
{
}

//------------------------------------------------------------------------------
// 消息处理
//------------------------------------------------------------------------------

// 窗口大小改变
int on_resize(int width, int height)
{
    m_ui_context.screen              = uiVec2(width, height);
    m_ui_context.mainWindow.bounds.w = static_cast<float>(width);
    m_ui_context.mainWindow.bounds.h = static_cast<float>(height);
    return 0;
}

// 按键按下
int on_key_push(int key)
{
    m_ui_context.key = key;
    return 0;
}

// 按键弹起
int on_key_pop(int key)
{
    (void) key;

    // m_ui_context.key = 0;
    return 0;
}

// 字符输入
int on_input(int ch)
{
    if (ch > 255 || isspace(ch) || isgraph(ch)) {
        m_ui_context.ch = ch;
        return 1;
    }
    return 0;
}

// 鼠标按下
int on_mouse_push(int x, int y, uiButton button)
{
    (void) x;
    (void) y;

    m_ui_context.lastpos = m_ui_context.mouse;
    m_ui_context.button  = button;

    // 2026-03-20 18:20:36 鼠标按下，设置偏移量为 0
    m_ui_context.delta = uiVec2();

    // 判断是否没有悬停窗口，取消激活当前窗口
    if (m_ui_context.hoveredWindow == 0) {
        on_root_window_click();
    }

    return 0;
}

// 鼠标弹起
int on_mouse_pop(int x, int y, uiButton button)
{
    (void) x;
    (void) y;
    (void) button;

    m_ui_context.button        = UI_BUTTON_NONE;
    m_ui_context.clicked       = 0;
    m_ui_context.clickedWidget = 0;

    return 0;
}

// 鼠标移动
int on_mouse_move(int x, int y, uiButton button)
{
    m_ui_context.mouse = uiVec2(x, y);

    if (button != UI_BUTTON_NONE) {
        // 计算移动偏移量
        // 2026-03-20 18:20:36 积累移动
        m_ui_context.delta += m_ui_context.mouse - m_ui_context.lastpos;
        m_ui_context.lastpos = m_ui_context.mouse;
        // printf("delta: %8.3f, %8.3f\n", m_ui_context.delta.x, m_ui_context.delta.y);
    }
    else {
        m_ui_context.delta = uiVec2();
    }

    // 设置悬停窗口
    uiWindow* window = select_window_from_position(m_ui_context.mouse);
    if (window) {
        m_ui_context.hoveredWindow = window->id;
    }
    else {
        m_ui_context.hoveredWindow = 0;
    }

    return 0;
}

// 滚轮滚动
int on_scroll(int z)
{
    // printf("scroll: %d\n", z);
    m_ui_context.scroll += static_cast<float>(z) * 8.0f;

    return 0;
}

//------------------------------------------------------------------------------

// 返回当前环境
uiContext* context()
{
    return &m_ui_context;
}

// 开始界面生成
bool begin_context(void* instance)
{
    // 初始化界面环境
    on_begin_context(instance);

    // 需要重置的状态

    // 动态设置标题栏高度 (因为字体问题可能开始未初始化)
    m_ui_context.style.titleHeight = char_height() + m_ui_context.style.framePadding.y * 2.0f;
    m_ui_context.style.menuHeight  = char_height() + m_ui_context.style.framePadding.y * 2.0f;

    // 重置缓存的剪裁矩形
    m_ui_context.clipRect = uiRect(0.0f, 0.0f, m_ui_context.screen.x, m_ui_context.screen.y);

    // 悬停部件切换
    if (m_ui_context.lastHoveredWidget != m_ui_context.lastHoveredWidget) {
        m_ui_context.lastHoveredWidget = m_ui_context.lastHoveredWidget;
        // m_ui_context.scrollY = 0.0f; // 重置滚轴数值
    }

    // 重置悬停部件，每帧只处理一个部件作为悬停部件
    m_ui_context.hoveredWidget = 0;

    // 释放所有组件
    /*
    for (auto widget : m_ui_context.widgets) {
        ui_delete(widget);
    }
    m_ui_context.widgets.clear();
    */

    // 清空绘图列表
    m_ui_context.mainWindow.cmdlist.dispose();
    for (size_t i = 0; i < m_ui_context.windows.size(); ++i) {
        m_ui_context.windows[i]->cmdlist.dispose();
    }
    m_ui_context.backgroundCmdList.dispose();
    m_ui_context.foregroundCmdList.dispose();

    // 清空布局参数
    m_ui_context.mainWindow.bounds = uiRect(uiVec2(), m_ui_context.screen);
    init_layout(&m_ui_context.mainLayout, &m_ui_context.mainWindow);
    m_ui_context.layouts.clear();
    m_ui_context.indent = 0.0f;

    return true;
}

// 结束界面生成
void end_context(void* instance)
{
    uiWindow* window;
    uiFlags eventFlags;

    // 处理窗口鼠标消息、移动 (反向遍历)
    // 窗口消息要晚于部件消息处理，不然会抢夺部件消息，而且无法处理窗口 Z 值问题。
    for (size_t i = m_ui_context.windows.size() - 1; i < m_ui_context.windows.size(); --i) {
        window     = m_ui_context.windows[i];
        eventFlags = check_window_mouse_event(window);

        // 设置悬停窗口，无法处理多个窗口重叠问题
        // if (check_flags(eventFlags, UI_FLAGS_HOVERED)) { }

        // 鼠标是否点击
        if (check_flags(eventFlags, UI_FLAGS_CLICKED)) {
            // 如果当前悬停部件不是窗口的焦点部件，取消部件焦点
            if (window->focusWidget != m_ui_context.hoveredWidget) {
                window->focusWidget = 0;
            }

            if (is_actived(window) == false) {
                // 激活窗口
                set_active(i);
                // printf("set active: %lld\n", window->id);
            }
            break; // 窗口顺序改变，停止循环
        }

        // 判断是否捕获的窗口
        if (is_capture(window)) {
            if (window->panelFlags & UI_PANEL_SIZING) {
                // 缩放窗口
                if (window->panelFlags & UI_PANEL_SIZING_X) {
                    float w          = text_width(window->text.c_str(), window->text.length()) + (style()->titleHeight + style()->framePadding.y) * 2.0f;
                    window->bounds.w = std::max(window->bounds.w + m_ui_context.delta.x, w);
                }
                if (window->panelFlags & UI_PANEL_SIZING_Y) {
                    window->bounds.h = std::max(window->bounds.h + m_ui_context.delta.y, style()->titleHeight);
                }
                m_ui_context.delta = uiVec2(); // 设置 delta 为空
            }
            else {
                // 拖动窗口
                // printf("move %16s: %8.3f, %8.3f\n", window->text.c_str(), m_ui_context.delta.x, m_ui_context.delta.y);
                window->bounds.x += m_ui_context.delta.x;
                window->bounds.y += m_ui_context.delta.y;
                m_ui_context.delta = uiVec2(); // 设置 delta 为空
            }
            break;
        }

        /*
        if (check_hover(window)) {
            // printf("hover %16s: %8.3f, %8.3f\n", window->text.c_str(), m_ui_context.delta.x, m_ui_context.delta.y);
            wprintf(L"hover %16ls: %8.3f, %8.3f\n", window->text.c_str(), m_ui_context.delta.x, m_ui_context.delta.y);
        }
        */
        eventFlags = UI_FLAGS_NONE;
    }

    if (eventFlags == UI_FLAGS_NONE) {
        window     = &m_ui_context.mainWindow;
        eventFlags = check_window_mouse_event(window);

        // 鼠标是否点击
        if (check_flags(eventFlags, UI_FLAGS_CLICKED)) {
            // 如果当前悬停部件不是窗口的焦点部件，取消部件焦点
            if (window->focusWidget != m_ui_context.hoveredWidget) {
                window->focusWidget = 0;
            }

            on_root_window_click();
        }
    }

    if (_m_push_count != _m_pop_count) {
        assert(false);
    }

    // 结束界面环境
    on_end_context(instance);
}

// 绘制
void render(void* instance)
{
    // 开始绘制
    on_begin_draw(instance);

    // 绘制背景
    // m_ui_context.cmdlist.clear();
    // ui_draw_widget(&m_ui_context);

    // 绘制所有窗口
    if (!m_ui_context.backgroundCmdList.empty()) { // 背景
        draw_cmd(m_ui_context.backgroundCmdList, instance);
    }
    if (!m_ui_context.mainWindow.cmdlist.empty()) { // 桌面窗口
        draw_cmd(m_ui_context.mainWindow.cmdlist, instance);
    }
    for (size_t i = 0; i < m_ui_context.windows.size(); ++i) { // 窗口列表
        uiWindow* window = m_ui_context.windows[i];
        /*
        if (window->type == UI_WIDGET_MENU && window->data == 1) { // 菜单延迟一帧绘制
            printf("%d\n", window->data);
            continue;
        }
        */
        draw_cmd(window->cmdlist, instance);
    }
    if (!m_ui_context.foregroundCmdList.empty()) { // 前景
        draw_cmd(m_ui_context.foregroundCmdList, instance);
    }

    // 结束绘制
    on_end_draw(instance);
}

// 初始化布局
void init_layout(uiLayout* layout, uiPanel* panel)
{
    layout->container = panel;

    if (is_window(panel)) { // 窗口
        layout->owner = static_cast<uiWindow*>(panel);
        if (panel->panelFlags & UI_PANEL_PADDING) {
            if (panel->type == UI_WIDGET_MENU) { // 弹出菜单
                layout->padding = style()->menuPadding;
            }
            else { // 窗口
                layout->padding = style()->windowPadding;
            }
        }
        else {
            layout->padding = uiVec2(0.0f);
        }
        layout->space = (panel->panelFlags & UI_PANEL_SPACE) ? style()->itemSpace : uiVec2(0.0f);
    }
    /*
    else if (panel->type == UI_WIDGET_POPUP) { // 弹出窗口
        layout->owner = static_cast<uiWindow*>(panel);

        layout->padding = (panel->panelFlags & UI_PANEL_PADDING) ? style()->windowPadding : uiVec2(0.0f);
        layout->space   = (panel->panelFlags & UI_PANEL_SPACE) ? style()->itemSpace : uiVec2(0.0f);
    }
    */
    else { // 子窗口
        layout->owner =
            m_ui_context.layouts.empty()
                ? &m_ui_context.mainWindow
                : m_ui_context.layouts.back().owner;
        layout->padding = (panel->panelFlags & UI_PANEL_PADDING) ? style()->framePadding : uiVec2(0.0f);
        layout->space   = (panel->panelFlags & UI_PANEL_SPACE) ? style()->itemSpace : uiVec2(0.0f);
    }

    layout->contentRect = compute_content_rect(panel, true);

    layout->scrollMax = uiVec2();

    layout->nextWidgetPos  = uiVec2(m_ui_context.indent, 0.0f);
    layout->nextWidgetSize = uiVec2();

    layout->sameline   = false;
    layout->lineHeight = 0.0f;
}

// 压入布局
uiLayout* push_layout(uiPanel* panel)
{
    uiLayout layout;
    init_layout(&layout, panel);

    m_ui_context.layouts.push_back(layout);

    ++_m_push_count;
    // wprintf(_T("push layout: %d/%d  %ls\n"), _m_push_count, _m_pop_count, layout.container->text.c_str());

    return &m_ui_context.layouts.back();
}

// 弹出布局
void pop_layout()
{
    if (m_ui_context.layouts.empty()) {
        return;
    }

    ++_m_pop_count;
    // wprintf(_T(" pop layout: %d/%d  %ls\n"), _m_push_count, _m_pop_count, layout()->container->text.c_str());

    m_ui_context.layouts.back().owner->cmdlist.pop_clip(); // 弹出剪裁矩形
    m_ui_context.layouts.pop_back();
}

// 返回当前窗口布局
uiLayout* layout()
{
    if (m_ui_context.layouts.empty()) {
        return &m_ui_context.mainLayout; // 返回桌面布局
    }
    else {
        return &m_ui_context.layouts.back(); // 返回当前窗口布局
    }
}

//------------------------------------------------------------------------------

// 返回当前窗口的绘图列表
uiCommandList* window_cmdlist()
{
    /*
    if (m_ui_context.popups.empty()) {
        return &current_window()->cmdlist;
    }
    else {
        // uiWindow* window = find_window_by_id(m_ui_context.popups.back());
        // return &window->cmdlist;
        return &m_ui_context.popupCmdlist;
    }
    */
    return &layout()->owner->cmdlist;
}

// 返回背景绘图列表
uiCommandList* background_cmdlist()
{
    return &m_ui_context.backgroundCmdList;
}

// 返回前景绘图列表
uiCommandList* foreground_cmdlist()
{
    return &m_ui_context.foregroundCmdList;
}

// 压入剪裁区域
void push_clip(float x, float y, float w, float h, bool intersect)
{
    uiCommandList* cmdlist = window_cmdlist();
    if (!cmdlist) {
        return;
    }

    // cmdlist->set_stroke(colors::red);
    // cmdlist->draw_rect(x, y, w, h);

    cmdlist->push_clip(x, y, w, h, intersect);
}

// 弹出剪裁区域
void pop_clip()
{
    uiCommandList* cmdlist = window_cmdlist();
    if (!cmdlist) {
        return;
    }
    cmdlist->pop_clip();
}

// 压入剪裁矩形
void push_clip(uiPanel* panel, bool intersect)
{
    uiRect rect;

    if (panel) {
        rect = compute_clip_rect(panel);
    }
    else {
        rect = uiRect(uiVec2(), m_ui_context.screen);
    }

    /*
    if (check_hover(panel)) {
        uiCommandList* cmdlist = command_list();
        if (cmdlist) {
            cmdlist->set_stroke(colors::red);
            cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);
        }
    }
    */

    push_clip(rect.x, rect.y, rect.w, rect.h, intersect);

    // 记录当前剪裁矩形
    m_ui_context.clipRect = rect;
}

// 弹出剪裁矩形
void pop_clip(uiPanel* panel)
{
    (void) panel;

    pop_clip();

    // 调试剪裁矩形
    /*
    if (is_actived(get_owner(panel))) {
        command_list()->set_stroke(colors::magenta);
        command_list()->draw_rect(m_ui_context.clipRect.shrink(2.0f, 2.0f));
    }
    */

    // 还原缓存的剪裁矩形
    if (m_ui_context.layouts.empty()) {
        m_ui_context.clipRect = uiRect(0, 0, m_ui_context.screen.x, m_ui_context.screen.y);
    }
    else {
        m_ui_context.clipRect = compute_clip_rect(m_ui_context.layouts.back().container);
    }
}

// 判断矩形是否在剪裁矩形内
bool is_visible_clipbox(const uiRect& rect)
{
    return m_ui_context.clipRect.collision(rect);
}

// 判断控件是否在剪裁矩形内
bool is_visible_clipbox(uiWidget* widget)
{
    return is_visible_clipbox(widget->bounds);
}

//------------------------------------------------------------------------------

// 检查窗口是否产生鼠标消息
uiFlags check_window_mouse_event(uiWindow* window)
{
    uiFlags flags = 0;

    const float titleHeight = m_ui_context.style.titleHeight;                     // 标题栏高度
    const bool hasTitle     = check_flags(window->panelFlags, UI_PANEL_TITLE);    // 是否拥有标题栏
    const bool isCollapsed  = check_flags(window->panelFlags, UI_PANEL_COLLAPSE); // 是否折叠

    // 窗口矩形
    uiRect rect = window->bounds;
    if (isCollapsed) {
        rect.h = titleHeight;
    }

    /*
    // 绘制鼠标位置窗口矩形
    if (check_hover(window)) {
        uiCommandList* cmdlist = foreground_cmdlist();
        if (cmdlist) {
            // cmdlist->set_stroke(colors::magenta, 4.0f);
            // cmdlist->draw_rect(m_ui_context.clipRect);
            cmdlist->set_stroke(colors::yellow, 2.0f);
            cmdlist->draw_rect(window->bounds);
        }
    }
    //*/

    // check mouse hovering
    // if (rect.contains(m_ui_context.mouse) == false) { return flags; }
    if (m_ui_context.hoveredWindow != window->id) {
        return flags;
    }
    flags |= UI_FLAGS_HOVERED;
    // wprintf(L"hot: %ls\n", window->text.c_str());

    // check mouse buttons
    if (m_ui_context.button == UI_BUTTON_NONE) {
        return flags;
    }
    // flags |= UI_FLAGS_PRESSED;

    // check click
    if (m_ui_context.clicked == 0) {                  // 判断鼠标是否已经点击过
        m_ui_context.delta    = uiVec2();             // 清理鼠标移动向量
        m_ui_context.clickPos = m_ui_context.lastpos; // 记录点击坐标
        m_ui_context.clicked  = 1;                    // 标记已经触发点击
        set_clicked_widget(window);                   // 记录点击的部件
        // if (m_ui_context.clickedWidget == 0) {        // 防止和部件抢事件
        //     m_ui_context.clickedWidget = window->id;  // 记录点击的部件
        // }

        flags |= UI_FLAGS_CLICKED;

        // 标题栏检测
        if (hasTitle) {
            // 判断是否点击了折叠
            if (uiRect(rect.x, rect.y, titleHeight, titleHeight).contains(m_ui_context.mouse)) {
                set_flags_enabled(window->panelFlags, UI_PANEL_COLLAPSE, !check_flags(window->panelFlags, UI_PANEL_COLLAPSE));
            }

            // 判断是否点击了关闭
            if (uiRect(rect.x + rect.w - titleHeight, rect.y, titleHeight, titleHeight).contains(m_ui_context.mouse)) {
                if (window->pIsOpen) {
                    *window->pIsOpen = false;
                }
            }
        }

        // 如果是折叠状态
        if (isCollapsed) {
            set_flags_masked(window->panelFlags, 0, UI_PANEL_SIZING); // 禁用改变大小
        }
        else {
            // 检查是否点击了小三角
            if (check_flags(window->panelFlags, UI_PANEL_RESIZABLE)) {
                // float left   = rect.x;
                // float top    = rect.y;
                const float right  = rect.x + rect.w;
                const float bottom = rect.y + rect.h;

                const float triangleSize = style()->triangleSize;
                const uiVec2 triangle[]  = {
                    uiVec2(right, bottom - triangleSize),
                    uiVec2(right - triangleSize, bottom),
                    uiVec2(right, bottom),
                };

                // 判断是否点击边框或者缩放小三角 (带滚动条的窗口判断有问题)
                if (point_in_triangle(m_ui_context.mouse, triangle[0], triangle[1], triangle[2])) {
                    set_flags_masked(window->panelFlags, UI_PANEL_SIZING, UI_PANEL_SIZING);
                }
                else if (m_ui_context.mouse.y >= bottom - 4 && m_ui_context.mouse.y < bottom) {
                    set_flags_masked(window->panelFlags, UI_PANEL_SIZING_Y, UI_PANEL_SIZING);
                }
                else {
                    set_flags_masked(window->panelFlags, 0, UI_PANEL_SIZING);
                }
            }
        }
    }

    return flags;
}

// 检查部件是否产生鼠标消息
uiFlags check_widget_mouse_event(uiWidget* widget, bool allowActive)
{
    uiFlags& flags = m_ui_context.widgetFlags = UI_FLAGS_NONE;

    /*
    printf(
        "check click: %16s [%8.3f, %8.3f, %8.3f, %8.3f] [%8.3f, %8.3f]\n",
        widget->text.c_str(),
        widget->bounds.x, widget->bounds.y, widget->bounds.w, widget->bounds.h,
        m_ui_context.mouse.x, m_ui_context.mouse.y
    );
    */

    if (is_window(widget)) {
        printf("error> widget is window\n");
        return flags;
    }

    //*
    if (check_hover(widget)) {
        uiCommandList* cmdlist = foreground_cmdlist();
        if (cmdlist) {
            cmdlist->set_stroke(colors::magenta, 1.0f); // 剪裁矩形
            draw_check_rect(cmdlist, m_ui_context.clipRect.shrink(2.0f, 2.0f));
            cmdlist->set_stroke(colors::blue, 1.0f);    // 部件矩形
            cmdlist->draw_rect(widget->bounds.expand(2.0f, 2.0f));
            // cmdlist->set_text(colors::blue);
            // cmdlist->draw_string(widget->bounds.x, widget->bounds.y, widget->text);
        }
    }
    //*/

    // 检查鼠标是否在当前剪裁区域 (error: 窗口的滚动条绘制的时候，剪裁区域还在上个窗口)
    // if (m_ui_context.clipRect.contains(m_ui_context.mouse) == false) { return flags; }

    if (widget->parent) { // 测试无窗口任意位置部件
        // 用以解决窗口 Z 值判断问题
        // 当前悬停窗口不是部件的所有者窗口，返回
        // if (is_hovered(layout()->owner) == false) { return flags; }
        // 2026-10-27 21:36:10  获取所有者窗口，判断所有者是否悬停窗口。
        //                      如果在 Android 设备上判定需要修正，判断所有者窗口是否被正确点击。
        uiWindow* window = get_owner(widget);
        /*
        if (is_enabled(window) == false) { // ???
            return flags;
        }
        */

        if (is_hovered(window) == false) {
            return flags;
        }

        if (check_hover(widget)) {
            // wprintf(L"owner: %ls\n", window->text.c_str());
        }

        // 父窗口剪裁，用于处理部件越界检查
        uiPanel* panel = layout()->container;
        if (panel->type == UI_WIDGET_PANEL) {
            if (check_hover(panel) == false) {
                return flags;
            }
        }
    }

    // check mouse hovering
    if (check_hover(widget) == false) {
        return flags;
    }
    flags |= UI_FLAGS_HOVERED;

    // 设置悬停部件
    // _tprintf(_T("hovered: %s  uiID: %llu\n"), widget->text.c_str(), widget->id);
    // wprintf(L"hot: %ls\n", widget->text.c_str());
    if (m_ui_context.hoveredWidget == 0) { // 处理多个部件重叠，每帧只设置一个悬停部件
        m_ui_context.hoveredWidget = widget->id;
    }

    // check mouse buttons
    if (m_ui_context.button == UI_BUTTON_NONE) {
        return flags;
    }
    // flags |= UI_FLAGS_PRESSED; // 不准确

    // printf("button: %d, clicked: %d\n", m_ui_context.button, m_ui_context.clicked);

    // check click
    if (widget->parent) {
        if (m_ui_context.clicked == 0) {                  // 判断鼠标是否已经点击过
            m_ui_context.delta    = uiVec2();             // 清理鼠标移动向量
            m_ui_context.clickPos = m_ui_context.lastpos; // 记录点击坐标
            m_ui_context.clicked  = 1;                    // 标记已经触发点击
            set_clicked_widget(widget);                   // 记录点击的部件

            flags |= UI_FLAGS_CLICKED;

            // 激活父窗口
            if (allowActive && is_actived(layout()->owner) == false) {
                set_active(layout()->owner);
                // todo...
            }
        }
    }
    else {                                                // 无窗口组件
        if (m_ui_context.clicked <= 0) {                  // 判断鼠标是否已经点击过
            m_ui_context.delta    = uiVec2();             // 清理鼠标移动向量
            m_ui_context.clickPos = m_ui_context.lastpos; // 记录点击坐标
            m_ui_context.clicked  = 1;                    // 标记已经触发点击
            set_clicked_widget(widget);                   // 记录点击的部件

            flags |= UI_FLAGS_CLICKED;
        }
    }

    return flags;
}

//------------------------------------------------------------------------------
// 窗口
//------------------------------------------------------------------------------

// 查找窗口
uiWindow* find_window(const uiString& title)
{
    for (size_t i = 0; i < m_ui_context.windows.size(); ++i) {
        uiWindow* window = m_ui_context.windows[i];
        if (window->text == title) {
            return window;
        }
    }
    return nullptr;
}

// 根据 ID 查找窗口
uiWindow* find_window_by_id(uiID id)
{
    for (size_t i = 0; i < m_ui_context.windows.size(); ++i) {
        uiWindow* window = m_ui_context.windows[i];
        if (window->id == id) {
            return window;
        }
    }
    return nullptr;
}

// 查找窗口指针，返回窗口索引
size_t find_window(uiWindow* window)
{
    for (size_t i = 0; i < m_ui_context.windows.size(); ++i) {
        if (m_ui_context.windows[i] == window) {
            return i;
        }
    }
    return SIZE_MAX;
}

// 判断窗口是否激活
bool is_actived(uiWindow* window)
{
    if (window) {
        return m_ui_context.activedWindow == window->id;
    }
    return false;
}

// 设置激活窗口
void set_active(size_t index)
{
    if (index != SIZE_MAX) {
        uiWindow* window = m_ui_context.windows[index];

        // 激活窗口，并设置焦点
        m_ui_context.activedWindow = window->id;
        m_ui_context.focusWindow   = window->id;
        std::rotate(m_ui_context.windows.begin() + index, m_ui_context.windows.begin() + index + 1, m_ui_context.windows.end());

        // _tprintf(_T("window active: %u. %ls\n"), index, window->text.c_str());
    }
}

void set_active(uiWindow* window)
{
    set_active(find_window(window));
}

// 把窗口放到最前面
void bring_to_front(uiWindow* window)
{
    size_t index = find_window(window);
    if (index != SIZE_MAX) {
        std::rotate(m_ui_context.windows.begin() + index, m_ui_context.windows.begin() + index + 1, m_ui_context.windows.end());
    }
}

// 把窗口放到最后面
void bring_to_back(uiWindow* window)
{
    size_t index = find_window(window);
    if (index != SIZE_MAX) {
        std::rotate(m_ui_context.windows.begin(), m_ui_context.windows.begin() + index, m_ui_context.windows.begin() + index + 1);
    }
}

// 查找坐标位置的窗口
uiWindow* select_window_from_position(const uiVec2& pos)
{
    if (m_ui_context.windows.empty()) {
        return nullptr;
    }

    for (size_t i = m_ui_context.windows.size() - 1; i < m_ui_context.windows.size(); --i) {
        uiWindow* window = m_ui_context.windows[i];
        if (is_enabled(window) && window->bounds.contains(pos)) {
            return window;
        }
    }

    if (m_ui_context.mainWindow.bounds.contains(pos)) {
        return &m_ui_context.mainWindow;
    }

    return nullptr;
}

// 主窗口点击
void on_root_window_click()
{
    m_ui_context.activedWindow = 0;
    m_ui_context.focusWindow   = 0;

    cancel_popup();            // 取消弹出部件
    m_ui_context.clicked = -1; // 标记鼠标按下
}

// 计算剪裁矩形
uiRect compute_clip_rect(uiPanel* panel)
{
    const uiStyle& style = m_ui_context.style;

    // 如果有标题栏
    uiRect rect = panel->bounds;
    if (check_flags(panel->panelFlags, UI_PANEL_TITLE)) {
        rect.y += style.titleHeight;
        rect.h -= style.titleHeight;
    }

    // 如果有菜单栏
    if (panel->panelFlags & UI_PANEL_MENU) {
        rect.y += style.titleHeight;
        rect.h -= style.titleHeight;
    }

    return rect;
}

// 计算容器区域矩形 (客户区)
uiRect compute_content_rect(uiPanel* panel, bool padding)
{
    const uiStyle& style = m_ui_context.style;

    uiRect rect = panel->bounds;

    // 如果有标题栏
    if (panel->panelFlags & UI_PANEL_TITLE) {
        rect.y += style.titleHeight;
        rect.h -= style.titleHeight;
    }

    // 如果有菜单栏
    if (panel->panelFlags & UI_PANEL_MENU) {
        rect.y += style.titleHeight;
        rect.h -= style.titleHeight;
    }

    // 如果有边界空白
    if (padding && (panel->panelFlags & UI_PANEL_PADDING)) {
        if (panel->type == UI_WIDGET_WINDOW) { // 如果是窗口，不包括菜单窗口
            rect.x += style.windowPadding.x;
            rect.y += style.windowPadding.y;
            rect.w -= style.windowPadding.x * 2.0f;
            rect.h -= style.windowPadding.y * 2.0f;
        }
        else if (panel->type == UI_WIDGET_MENU) {
            rect.x += style.menuPadding.x;
            rect.y += style.menuPadding.y;
            rect.w -= style.menuPadding.x * 2.0f;
            rect.h -= style.menuPadding.y * 2.0f;
        }
        else { // 如果是面板
            rect.x += style.framePadding.x;
            rect.y += style.framePadding.y;
            rect.w -= style.framePadding.x * 2.0f;
            rect.h -= style.framePadding.y * 2.0f;
        }
    }

    // 槽位
    rect.x += panel->slot.left;
    rect.y += panel->slot.top;
    rect.w -= panel->slot.left + panel->slot.right;
    rect.h -= panel->slot.top + panel->slot.bottom;

    // 如果有滚动条
    if (panel->panelFlags & UI_PANEL_SCROLL) {
        if (panel->scrollMax.x > 0.0f) { // 水平滚动条
            rect.h -= style.scrollWidth;
        }
        if (panel->scrollMax.y > 0.0f) { // 垂直滚动条
            rect.w -= style.scrollWidth;
        }
    }

    return rect;
}

// 分配菜单栏项
uiRect allocate_menubar_item(uiLayout* layout, float width, float height)
{
    const float locationX = layout->contentRect.x - layout->container->scroll.x;
    const float locationY = layout->contentRect.y - layout->container->scroll.y;

    float x, y, w, h;

    x = locationX + layout->nextWidgetPos.x;
    y = locationY + layout->nextWidgetPos.y;
    w = width;
    if (w < 0.0f) {
        w = layout->contentRect.w * clamp(-width, 0.0f, 1.0f); // [0, -1] 按比例分配
    }
    h = height;

    // 计算下一个部件坐标
    layout->nextWidgetPos.x += w + layout->space.x;

    return layout->lastRect = uiRect(x, y, w, h);
}

// 分配部件矩形，宽高是部件大小期望值
uiRect allocate_widget_rect(float width, float height, bool forceNewLine)
{
    uiLayout* layout = minimgui::layout();

    if (layout->container->type == UI_WIDGET_MENUBAR) { // 菜单栏只分配横向空间
        return allocate_menubar_item(layout, width, height);
    }

    // 计算偏移的原点，加上滚动条偏移量
    const float locationX = layout->contentRect.x - layout->container->scroll.x;
    const float locationY = layout->contentRect.y - layout->container->scroll.y;
    const float indent    = m_ui_context.indent;

    float x, y, w, h;

    // 强制新行
    if (forceNewLine) {
        layout->sameline = false;
    }

    // 同行，坐标右移
    if (layout->sameline) {
        x = locationX + layout->nextWidgetPos.x;
        y = locationY + layout->nextWidgetPos.y;
        w = width;
        if (w < 0.0f) {
            // w = (layout->contentRect.x + layout->contentRect.w - x) * clamp(-width, 0.0f, 1.0f); // [0, -1] 按比例分配
            w = (layout->contentRect.w - indent) * clamp(-width, 0.0f, 1.0f); // [0, -1] 按比例分配
        }
        h = height;
    }
    else {
        // 新行坐标
        layout->nextWidgetPos.x = indent;
        layout->nextWidgetPos.y += layout->lineHeight + layout->space.y;
        layout->lineHeight = 0.0f;

        x = locationX + layout->nextWidgetPos.x;
        y = locationY + layout->nextWidgetPos.y;
        w = width;
        if (w < 0.0f) {
            w = (layout->contentRect.w - indent) * clamp(-width, 0.0f, 1.0f); // [0, -1] 按比例分配
        }
        h = height;
    }

    // 统计行最高部件
    layout->lineHeight = std::max(layout->lineHeight, h);

    // 计算下一个部件坐标 (只计算同行)
    layout->nextWidgetPos.x += w + layout->space.x;

    // 拓宽滚动区域
    layout->scrollMax.x = std::max(layout->scrollMax.x, x + w - locationX);
    layout->scrollMax.y = std::max(layout->scrollMax.y, y + h - locationY);

    // 重置 sameline
    layout->sameline = false;

    // 如果是菜单窗口，扩大窗口大小
    if (layout->container->type == UI_WIDGET_MENU) {
        // if (layout->container->type == UI_WIDGET_MENU && layout->container->data == 1) {
        float cx = std::max(layout->container->bounds.w - w, 0.0f);
        // layout->expand(cx, h);
        uiWindow* window = static_cast<uiWindow*>(layout->container);
        window->popupSize.w += cx;
        window->popupSize.h += h;
    }

    return layout->lastRect = uiRect(x, y, w, h);
}

// 分配整行部件矩形，无 padding
uiRect allocate_widget_rect_full_line(float height)
{
    uiLayout* layout = minimgui::layout();

    // 判断是否需要新开一行
    if (layout->nextWidgetPos.x > m_ui_context.indent) {
        layout->nextWidgetPos.y += layout->lineHeight + layout->space.y;
    }

    const float locationY = layout->contentRect.y - layout->container->scroll.y;
    const float indent    = m_ui_context.indent;

    float x, y, w, h;

    x = layout->container->bounds.x + 1.0f;
    y = locationY + layout->nextWidgetPos.y;
    w = layout->container->bounds.w - 2.0f;
    h = height;

    if (layout->container->scrollMax.y > 0.0f) { // 垂直滚动条
        w -= style()->scrollWidth;
    }

    // 新行坐标
    layout->nextWidgetPos.x = indent;
    layout->nextWidgetPos.y += h + layout->space.y;
    layout->lineHeight = 0.0f;

    return layout->lastRect = uiRect(x, y, w, h);
}

// 分配槽位空间
uiRect allocate_slot_area(float size, uiFlags position)
{
    uiLayout* layout = minimgui::layout();
    uiPanel* panel   = layout->container;
    uiRect rect      = compute_content_rect(panel, false);

    switch (position) {
    case UI_LEFT:
        panel->slot.left += size;
        rect.w = size;
        break;
    case UI_TOP:
        panel->slot.top += size;
        rect.h = size;
        break;
    case UI_RIGHT:
        panel->slot.right += size;
        rect.x += rect.w - size;
        rect.w = size;
        break;
    case UI_BOTTOM:
        panel->slot.bottom += size;
        rect.y += rect.h - size;
        rect.h = size;
        break;
    default:
        break;
    }

    return layout->lastRect = rect;
}

/*
// 不太完善...
static uiVec2 adjust_popup_rect(const uiRect& widgetRect, const uiVec2& size, uiDirection dir)
{
    uiVec2 pos;
    switch (dir) {
    case UI_LEFT:
        pos.x = widgetRect.x - size.x;
        pos.y = widgetRect.y;
        break;
    case UI_RIGHT:
        pos.x = widgetRect.x + widgetRect.w;
        pos.y = widgetRect.y;
        break;
    case UI_UP:
        pos.x = widgetRect.x;
        pos.y = widgetRect.y - size.y;
        break;
    case UI_DOWN:
    default:
        pos.x = widgetRect.x;
        pos.y = widgetRect.y + size.y;
        break;
    }
    return pos;
}

// 计算矩形弹出坐标
uiVec2 adjust_popup_position(const uiRect& widgetRect, const uiVec2& size, uiDirection dir)
{
    // todo...
}
*/

// 调整菜单弹出坐标
uiVec2 adjust_main_menu_popup_position(const uiRect& widgetRect, const uiVec2& size)
{
    const uiRect screen = uiRect(uiVec2(), m_ui_context.screen);

    // 计算弹出坐标
    const float rightX  = widgetRect.x;
    const float leftX   = widgetRect.x + widgetRect.w - size.x;
    const float bottomY = widgetRect.y + widgetRect.h;
    const float topY    = widgetRect.y - size.y;

    const uiVec2 candidates[] = {
        uiVec2(rightX, bottomY), // 底-右 (default)
        uiVec2(leftX, bottomY),  // 底-左
        uiVec2(rightX, topY),    // 顶-右
        uiVec2(leftX, topY),     // 顶-左
    };

    for (size_t i = 0; i < 4; ++i) {
        if (screen.contains(candidates[i], size)) {
            return candidates[i];
        }
    }

    // 垂直：检查底部是否放下；否则选上下空间大的一侧 (可能仍然放不下，但必须显示)
    const float upSize   = widgetRect.y;
    const float downSize = screen.h - (widgetRect.y + widgetRect.h);

    float y;
    if (size.y <= downSize || upSize < downSize) {
        y = bottomY;
    }
    else {
        y = topY;
    }

    // 水平：选左右空间大的一侧对齐，越界则压进屏幕
    const float leftSize  = widgetRect.x;
    const float rightSize = screen.w - (widgetRect.x + widgetRect.w);

    float x;
    if (size.x <= rightSize || leftSize <= rightSize) {
        x = rightX;
    }
    else {
        x = leftX;
    }
    x = clamp(x, 0.0f, screen.w - size.x);

    return uiVec2(x, y);
}

uiVec2 adjust_sub_menu_popup_position(const uiRect& widgetRect, const uiVec2& size)
{
    const uiRect screen = uiRect(uiVec2(), m_ui_context.screen);

    // 计算弹出坐标
    const float rightX  = widgetRect.x + widgetRect.w;
    const float leftX   = widgetRect.x - size.x;
    const float bottomY = widgetRect.y;
    const float topY    = widgetRect.y + widgetRect.h - size.y;

    const uiVec2 candidates[] = {
        uiVec2(rightX, bottomY), // 右-底 (default)
        uiVec2(rightX, topY),    // 右-顶
        uiVec2(leftX, bottomY),  // 左-底
        uiVec2(leftX, topY),     // 左-顶
    };

    for (size_t i = 0; i < 4; ++i) {
        if (screen.contains(candidates[i], size)) {
            return candidates[i];
        }
    }

    // 水平：检查右侧是否放下；否则选左右空间大的一侧 (可能仍然放不下，但必须显示)
    const float leftSize  = widgetRect.x;
    const float rightSize = screen.w - (widgetRect.x + widgetRect.w);

    float x;
    if (size.x <= rightSize || leftSize <= rightSize) {
        x = rightX;
    }
    else {
        x = leftX;
    }

    // 垂直：选上下空间大的一侧对齐，越界压进屏幕
    const float upSize   = widgetRect.y;
    const float downSize = screen.h - (widgetRect.y + widgetRect.h);

    float y;
    if (size.y <= downSize || upSize <= downSize) {
        y = bottomY;
    }
    else {
        y = topY;
    }
    y = clamp(y, 0.0f, screen.h - size.y);

    return uiVec2(x, y);
}

//------------------------------------------------------------------------------
// 部件
//------------------------------------------------------------------------------

// 判断部件是否容器
bool is_container(uiWidget* widget)
{
    return widget->type & UI_WIDGET_MASK_CONTAINER;
}

// 判断部件是否窗口
bool is_window(uiWidget* widget)
{
    return widget->type & UI_WIDGET_MASK_WINDOW;
}

// 返回部件的所有者窗口
uiWindow* get_owner(uiWidget* widget)
{
    if (is_window(widget)) { // 窗口
        return static_cast<uiWindow*>(widget);
    }
    else { // 部件
        uiWidget* parent = widget->parent;
        while (parent->parent) {
            parent = parent->parent;
        }
        return static_cast<uiWindow*>(parent);
    }
}

// 查找子部件
uiWidget* find_child(const uiString& title)
{
    uiWidgetList& widgets = layout()->owner->widgets;
    for (size_t i = 0; i < widgets.size(); ++i) {
        if (widgets[i]->text == title) {
            return widgets[i];
        }
    }
    return nullptr;
}

// 根据 ID 查找子部件
uiWidget* find_child_by_id(uiID id)
{
    uiWidgetList& widgets = layout()->owner->widgets;
    for (size_t i = 0; i < widgets.size(); ++i) {
        if (widgets[i]->id == id) {
            return widgets[i];
        }
    }
    return nullptr;
}

// 判断部件是否激活
bool is_enabled(uiWidget* widget)
{
    return check_flags(widget->flags, UI_FLAGS_DISABLED) == false;
}

// 检查部件是否鼠标悬停
bool check_hover(uiWidget* widget)
{
    return widget->bounds.contains(m_ui_context.mouse);
}

// 判断部件是否鼠标悬停
bool is_hovered(uiWidget* widget)
{
    if (widget == nullptr) {
        printf("is_hovered(): widget is null.\n");
        return false;
    }

    if (is_window(widget)) {
        return m_ui_context.hoveredWindow == widget->id; // 悬停窗口
    }
    else {
        return m_ui_context.hoveredWidget == widget->id; // 悬停部件
    }
}

// 判断部件是否鼠标按下
bool is_pressed(uiWidget* widget)
{
    // return check_flags(widget->flags, UI_FLAGS_DOWN);
    // 距离判断？如果没有拖动
    return m_ui_context.clickedWidget == widget->id;
}

// 设置鼠标点击部件
void set_clicked_widget(uiWidget* widget)
{
    m_ui_context.clickedWidget = widget->id;

    bool isMenuItem = false;

    wprintf(_T("click [%12ls: %12ls]"), widget->text.c_str(), widget_type_to_string(widget->type));
    if (widget->parent) {
        wprintf(_T("  parent: [%12ls: %12ls]"), widget->parent->text.c_str(), widget_type_to_string(widget->parent->type));
        isMenuItem = widget->parent->type == UI_WIDGET_MENU;
    }
    wprintf(_T("\n"));

    // 取消弹出部件
    if (isMenuItem == false && m_ui_context.popupWidget != widget->id) {
        cancel_popup();
    }
}

// 判断部件是否鼠标捕获
bool is_capture(uiWidget* widget)
{
    // 当前窗口必须是激活的，不然跨窗口同 ID 的部件会出乱
    if (is_container(widget)) {
        return m_ui_context.clickedWidget == widget->id;
    }
    else {
        if (widget->parent) {
            return is_actived(layout()->owner) && m_ui_context.clickedWidget == widget->id;
        }
        else {
            return m_ui_context.clickedWidget == widget->id;
        }
    }
}

// 判断部件是否输入焦点
bool is_focus(uiWidget* widget)
{
    if (is_window(widget)) { // 窗口
        return m_ui_context.activedWindow == widget->id;
    }
    else {
        if (widget->parent == nullptr) { // 如果没有父对象，返回 false
            return false;
        }

        if (widget->type & UI_WIDGET_MASK_CONTAINER) { // 容器
            // todo...
        }
        else {
        }

        uiPanel* panel = static_cast<uiPanel*>(widget->parent);
        return is_focus(panel) && (panel->focusWidget == widget->id);
    }
    return false;
}

// 设置焦点部件
void set_focus(uiWidget* widget)
{
    if (is_window(widget)) {
        if (widget->type == UI_WIDGET_WINDOW) { // 可以获得焦点的窗口
            m_ui_context.focusWindow = widget->id;
        }
    }
    else {
        if (widget->parent == nullptr) { // 如果没有父对象，返回
            return;
        }

        uiPanel* panel = static_cast<uiPanel*>(widget->parent);
        // 设置容器焦点
        set_focus(panel);
        // 设置部件焦点
        panel->focusWidget = widget->id;

        // 清空按键
        m_ui_context.key = m_ui_context.ch = 0;
    }
}

// 取消弹出窗口
void cancel_popup()
{
    // 获取缓存的状态
    // uiFlags& state = pick_cache_flags(0, UI_FLAGS_NONE);
    // state          = 0;
    m_ui_context.openPath.clear();

    for (size_t i = 0; i < m_ui_context.windows.size(); ++i) {
        uiWindow* window = m_ui_context.windows[i];
        if (window->type == UI_WIDGET_MENU) {
            set_flags_enabled(window->flags, UI_FLAGS_DISABLED, true);
            window->data = 0; // 重置菜单弹出计数
        }
    }

    m_ui_context.popupWidget = 0;
}

//------------------------------------------------------------------------------

// 返回当前主题
uiStyle* style()
{
    return &m_ui_context.style;
}

// 同一行
void sameline()
{
    layout()->sameline = true;
}

// 缩进
void indent(float i)
{
    m_ui_context.indent += i;
}

// 取消缩进
void unindent(float i)
{
    m_ui_context.indent -= i;
    if (m_ui_context.indent < 0.0f) {
        m_ui_context.indent = 0.0f;
    }
}

// 获取当前部件位置
uiVec2 get_position()
{
    return layout()->lastRect.position();
}

// 获取当前部件大小
uiVec2 get_size()
{
    return layout()->lastRect.size();
}

// 获取当前部件矩形
uiRect get_bounds()
{
    return layout()->lastRect;
}

// 判断当前部件是否鼠标悬停
bool is_hovered()
{
    return check_flags(m_ui_context.widgetFlags, UI_FLAGS_HOVERED);
}

//------------------------------------------------------------------------------

// 采集鼠标移动向量
uiVec2 pick_delta(float scale)
{
    uiVec2 v           = m_ui_context.delta * scale;
    m_ui_context.delta = uiVec2();
    return v;
}

// 采集整形移动向量。因为向量是浮点数，值最少积累到 min 才返回。
uiVec2i pick_delta_int_acc(int min, float scale)
{
    float n      = static_cast<float>(min);
    uiVec2 delta = m_ui_context.delta * scale;
    uiVec2i v    = uiVec2i();
    if (n < fabs(delta.x)) {
        v.x                  = static_cast<int>(delta.x);
        m_ui_context.delta.x = (delta.x - static_cast<float>(v.x)) / scale;
    }
    if (n < fabs(delta.y)) {
        v.y                  = static_cast<int>(delta.y);
        m_ui_context.delta.y = (delta.y - static_cast<float>(v.y)) / scale;
    }
    return v;
}

//------------------------------------------------------------------------------

} // namespace minimgui
