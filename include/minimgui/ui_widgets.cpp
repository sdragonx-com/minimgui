#include "ui_widgets.h"
#include "ui_user.h"

#include <algorithm>
#include <stdarg.h>

namespace minimgui {

//------------------------------------------------------------------------------
// 部件
//------------------------------------------------------------------------------

// 返回当前窗口
/*
uiWindow* current_window()
{
    // 栈底是当前窗口
    return static_cast<uiWindow*>(context()->layouts.front().window);
}
*/

// 返回部件类型名称
const uiChar* widget_type_to_string(uiWidgetType type)
{
    switch (type) {
    case UI_WIDGET_WINDOW: return _T("Window");           // 窗口 (uiWindow)
    case UI_WIDGET_PANEL: return _T("Panel");             // 面板 (uiPanel)

    case UI_WIDGET_MENUBAR: return _T("MenuBar");         // 菜单栏 (uiPanel)
    case UI_WIDGET_MENU: return _T("Menu");               // 菜单 (uiWindow)
    case UI_WIDGET_STATUSBAR: return _T("StatusBar");     // 状态栏
    case UI_WIDGET_TOOLBAR: return _T("ToolBar");         // 工具栏
    case UI_WIDGET_COLLAPSIBLE: return _T("Collapsible"); // 折叠面板

    case UI_WIDGET_BUTTON: return _T("Button");           // 按钮
    case UI_WIDGET_COLOR: return _T("Color");             // 颜色框
    case UI_WIDGET_CHECKBOX: return _T("Check");          // 选择框
    case UI_WIDGET_DRAG_NUMBER: return _T("DragNum");     // 拖拽数字
    case UI_WIDGET_EDIT: return _T("Edit");               // 单行编辑框
    case UI_WIDGET_LABEL: return _T("Label");             // 文本框
    case UI_WIDGET_MENU_ITEM: return _T("MenuItem");      // 菜单项
    case UI_WIDGET_SCROLL: return _T("Scroll");           // 滚动条
    case UI_WIDGET_SLIDER: return _T("Slider");           // 滑块
    case UI_WIDGET_TREENODE: return _T("TreeNode");       // 树节点

    case UI_WIDGET_CUSTOM: return _T("Custom");           // 自定义
    default: return _T("Unknown");
    }
}

// 获取部件缓存的状态，这个状态保存在 context 里面，是全局的
// 也就是说，部件的 id 必须是全局唯一的
uiFlags& pick_cache_flags(uiID id, uiFlags init)
{
    std::map<uiID, uint32_t>::iterator itr = context()->stateCache.find(id);
    if (itr != context()->stateCache.end()) { // 状态已经存在
        return itr->second;
    }
    else { // 插入状态
        itr = context()->stateCache.insert(std::make_pair(id, init)).first;
        return itr->second;
    }
}

//------------------------------------------------------------------------------

// 开始窗口
bool begin_window(const uiChar* text, bool* isOpen, uiVec2 pos, uiVec2 size, uiFlags panelFlags)
{
    if (isOpen && !*isOpen) {
        return false;
    }

    uiWindow* window = find_window(text);
    if (!window) {
        if (panelFlags == UI_FLAGS_NONE) {
            panelFlags = UI_WINDOW_DEFAULT;
        }
        window          = new uiWindow(UI_WIDGET_WINDOW, text, 0, panelFlags);
        window->bounds  = uiRect(pos.x, pos.y, size.x, size.y);
        window->pIsOpen = isOpen;
        context()->windows.push_back(window);
    }

    // 窗口折叠，客户区不再绘制，返回
    if (check_flags(window->panelFlags, UI_PANEL_COLLAPSE)) {
        // 绘制窗口
        draw_window(&window->cmdlist, window);
        return false;
    }

    // 压入布局
    push_layout(window);

     // 绘制窗口
    draw_window(&window->cmdlist, window);

    // 剪裁客户区矩形
    push_clip(window, false);

    return true;
}

bool begin_window(const uiChar* text, uiVec2 pos, uiVec2 size, uiFlags panelFlags)
{
    return begin_window(text, nullptr, pos, size, panelFlags);
}

// 结束窗口
void end_window()
{
    // 获取当前布局容器
    uiPanel* panel = layout()->container;

    if (panel->text == _T("design")) {
        // printf("design\n");
    }

    // 设置滚动范围
    if (check_flags(panel->panelFlags, UI_PANEL_SCROLL)) {
        const uiRect rect = layout()->contentRect;
        uiVec2 scrollMax  = layout()->scrollMax;
        if (rect.w < scrollMax.x) {
            panel->scrollMax.x = scrollMax.x;
            panel->scroll.x    = std::min(panel->scroll.x, scrollMax.x);
        }
        else {
            panel->scrollMax.x = 0.0f;
            panel->scroll.x    = 0.0f;
        }
        if (rect.h < scrollMax.y) {
            panel->scrollMax.y = scrollMax.y;
            panel->scroll.y    = std::min(panel->scroll.y, scrollMax.y);
        }
        else {
            panel->scrollMax.y = 0.0f;
            panel->scroll.y    = 0.0f;
        }
    }

    // 弹出布局
    pop_layout();

    // 弹出剪裁矩形
    pop_clip(panel);
}

//------------------------------------------------------------------------------

// 开始弹出窗口
bool begin_popup(const uiChar* text, uiVec2 pos, uiVec2 size)
{
    uiWindow* window = find_window(text);

    if (!window) {
        window          = new uiWindow(UI_WIDGET_WINDOW, text, 0, UI_PANEL_SCROLL | UI_PANEL_POPUP);
        window->bounds  = uiRect(pos.x, pos.y, size.x, size.y);
        window->pIsOpen = nullptr;
        context()->windows.push_back(window);
    }
    bring_to_front(window);
    context()->popupStack.push_back(window);

    window->bounds.x = pos.x;
    window->bounds.y = pos.y;

    // 绘制窗口
    draw_window(&window->cmdlist, window);

    // 压入布局
    push_layout(window);

    // 剪裁客户区矩形
    push_clip(window, false);

    return true;
}

// 结束弹出窗口
void end_popup()
{
    if (!context()->popupStack.empty()) {
        context()->popupStack.pop_back();

        end_window();
    }
}

//------------------------------------------------------------------------------
// 子窗口，面板对象

// 开始子窗口
bool begin_child(const uiChar* text, uiFlags panelFlags)
{
    uiPanel* parent = layout()->container;

    // 计算子窗口矩形
    uiRect rect = allocate_widget_rect(-1.0f, 100.0f);

    // 判断是否在窗口可视范围之内
    if (!parent->bounds.collision(rect)) {
        return false;
    }

    // 申请子窗口
    uiPanel* panel = static_cast<uiPanel*>(find_child(text));
    if (!panel) { // 这里只申请了对象，释放还没琢磨好
        panel = new uiPanel(UI_WIDGET_PANEL, text, 0, parent, panelFlags ? panelFlags : (uiFlags) UI_PANEL_DEFAULT);
        layout()->owner->widgets.push_back(panel);
    }
    panel->bounds = rect;

    // uiFlags eventFlags = check_widget_mouse_event(panel);
    // printf("events: %u\n", eventFlags);

    // 压入布局
    push_layout(panel);

    // 绘制窗口
    draw_frame(window_cmdlist(), panel, style()->widgetForeground[UI_COLOR_NORMAL], style()->background[UI_COLOR_NORMAL]);

    // 垂直滚动条
    if (panel->scrollMax.y > 0.0f) {
        uiRect rect = uiRect(
            panel->bounds.x + panel->bounds.w - style()->scrollWidth,
            panel->bounds.y,
            style()->scrollWidth, // 宽度
            panel->bounds.h       // 高度
        );

        const float padding   = ((panel->flags & UI_PANEL_PADDING) ? style()->framePadding.y * 2.0f : 0.0f);
        const float scrollMax = panel->scrollMax.y - panel->bounds.h + padding;

        // 滚轴滚动
        if (is_hovered(parent) && check_hover(panel)) {
            panel->scroll.y   = clamp(panel->scroll.y - context()->scroll, 0.0f, scrollMax);
            context()->scroll = 0.0f;
        }

        // 绘制滚动条
        vscroll(format(_T("%p.vscroll"), panel), rect, &panel->scroll.y, 0.0f, scrollMax);
    }

    // 剪裁客户区矩形
    push_clip(panel, true);

    return true;
}

// 结束子窗口
void end_child()
{
    end_window();
}

//------------------------------------------------------------------------------
// 菜单
//------------------------------------------------------------------------------

// 菜单最小宽度
const float MENU_ITEM_MIN_WIDTH = 160.0f;

// hash
const uiID MENU_HASH_MAGIC = 0x4D454E554D454E55ull;

// 开始菜单窗口
/*
bool begin_menu_window(const uiChar* text, uiID id, uiVec2 pos, uiVec2 size, uiFlags flags = UI_PANEL_SCROLL | UI_PANEL_POPUP)
{
    uiWindow* window = find_window_by_id(id);

    if (!window) {
        window              = new uiWindow(UI_WIDGET_MENU, text, id, flags ? flags : UI_PANEL_SCROLL | UI_PANEL_POPUP);
        window->bounds      = uiRect(pos.x, pos.y, size.x, size.y);
        window->popupSize.w = 0.0f;
        window->popupSize.h = 0.0f;
        context()->windows.push_back(window);
    }
    window->bounds = uiRect(pos.x, pos.y, size.x, size.y);
    bring_to_front(window);

    // 压入布局
    uiLayout* layout = push_layout(window);

    // 绘制窗口
    layout->data = draw_menu_window(window_cmdlist(), window);

    // 剪裁客户区矩形
    push_clip(nullptr, false);

    return true;
}
*/

bool begin_menu_window(const uiChar* text, uiID id, uiWidget* widget, bool isMenubar)
{
    uiWindow* window = find_window_by_id(id);

    if (!window) {
        window              = new uiWindow(UI_WIDGET_MENU, text, id, UI_PANEL_SCROLL | UI_PANEL_PADDING | UI_PANEL_POPUP);
        window->bounds      = uiRect(0.0f, 0.0f, MENU_ITEM_MIN_WIDTH, style()->menuPadding.y * 2.0f);
        window->popupSize.w = 0.0f;
        window->popupSize.h = 0.0f;
        context()->windows.push_back(window);
    }

    set_flags_enabled(window->flags, UI_FLAGS_DISABLED, false);
    bring_to_front(window);

    //*
    uiRect rect    = widget->bounds;
    uiVec2 pos     = isMenubar ? uiVec2(rect.x, rect.y + rect.h) : uiVec2(rect.x + rect.w - 4.0f, rect.y - 4.0f);
    uiVec2 size    = uiVec2(window->popupSize.w, window->popupSize.h);
    window->bounds = uiRect(pos, size);

    window->popupSize.w = MENU_ITEM_MIN_WIDTH;
    window->popupSize.h = style()->menuPadding.y * 2.0f;

    /*/
    // 计算弹出位置
    uiVec2 size(window->popupSize.w, window->popupSize.h);
    uiVec2 pos     = isMenubar
                         ? adjust_main_menu_popup_position(widget->bounds, size)
                         : adjust_sub_menu_popup_position(widget->bounds, size);
    window->bounds = uiRect(pos.x, pos.y, size.x, size.y);

    ++window->data;          // 增加弹出计数
    if (window->data == 1) { // 初始化弹出大小
        window->popupSize.w = MENU_ITEM_MIN_WIDTH;
        window->popupSize.h = style()->menuPadding.y * 2.0f;
    }
    else if (window->data == 2) { // 第二次弹出，窗口还没有计算好
        return false;
    }
    //*/

    // 压入布局
    uiLayout* layout = push_layout(window);

    // 绘制窗口
    layout->data = draw_menu_window(&window->cmdlist, window);

    // 剪裁客户区矩形
    push_clip(nullptr, false);

    return true;
}

// 结束菜单窗口
void end_menu_window()
{
    uiLayout* layout = minimgui::layout();

    // 获取当前布局容器
    uiPanel* panel = layout->container;

    // 设置滚动范围
    if (check_flags(panel->panelFlags, UI_PANEL_SCROLL)) {
        const uiRect rect = layout->contentRect;
        uiVec2 scrollMax  = layout->scrollMax;
        if (rect.w < scrollMax.x) {
            panel->scrollMax.x = scrollMax.x;
            panel->scroll.x    = std::min(panel->scroll.x, scrollMax.x);
        }
        else {
            panel->scrollMax.x = 0.0f;
            panel->scroll.x    = 0.0f;
        }
        if (rect.h < scrollMax.y) {
            panel->scrollMax.y = scrollMax.y;
            panel->scroll.y    = std::min(panel->scroll.y, scrollMax.y);
        }
        else {
            panel->scrollMax.y = 0.0f;
            panel->scroll.y    = 0.0f;
        }
    }

    // 修正菜单窗口弹出坐标、顶点大小
    // 这里无法解决菜单弹出问题
    /*
    size_t id              = layout->data;
    uiCommandList* cmdlist = window_cmdlist();

    cmdlist->points(id + 1).y = static_cast<uiWindow*>(layout->container)->popupSize.h;
    cmdlist->points(id + 3).y = static_cast<uiWindow*>(layout->container)->popupSize.h;
    //*/
    // static_cast<uiWindow*>(layout->container)->popupSize.w = layout->container->bounds.w;
    // static_cast<uiWindow*>(layout->container)->popupSize.h = layout->container->bounds.h;

    // 弹出布局
    pop_layout();

    // 弹出剪裁矩形
    pop_clip(panel);
}

// 开始菜单栏
bool begin_menu_bar(const uiChar* text)
{
    uiContext* ctx = context();

    uiPanel* parent = layout()->container;

    // 设置启用菜单栏
    set_flags_enabled(parent->panelFlags, UI_PANEL_MENU, true);

    // 申请子窗口
    uiPanel* panel = static_cast<uiPanel*>(find_child(text));
    if (!panel) { // 这里只申请了对象，释放还没琢磨好
        panel = new uiPanel(UI_WIDGET_MENUBAR, text, 0, parent, UI_PANEL_PADDING | UI_PANEL_SPACE);
        layout()->owner->widgets.push_back(panel);
    }

    // 计算菜单栏矩形
    panel->bounds = parent->bounds;
    if (check_flags(parent->panelFlags, UI_PANEL_TITLE)) {
        panel->bounds.y += style()->titleHeight;
    }
    panel->bounds.h = style()->menuHeight;

    // uiFlags eventFlags = check_widget_mouse_event(panel);
    // printf("events: %u\n", eventFlags);
    // printf("menubar\n");

    // 弹出剪裁矩形
    pop_clip(parent);

    // 压入布局
    push_layout(panel);

    // 绘制窗口
    draw_frame(window_cmdlist(), panel, uiColor(), style()->widgetBackground[UI_COLOR_NORMAL]);

    // 剪裁客户区矩形
    push_clip(panel, true);

    // 初始化弹出参数
    ctx->menuStack.clear();

    return true;
}

// 结束菜单栏
void end_menu_bar()
{
    end_child();

    // 重新压入父窗口剪裁矩形
    uiPanel* parent = layout()->container;
    push_clip(parent, true);
}

// 菜单栏主菜单项
static bool begin_main_menu(const uiChar* text)
{
    uiContext* ctx = context();

    uiString title = layout()->owner->text + (layout()->owner->type == UI_WIDGET_MENU ? _T(".") : _T(".menu.")) + text;
    uiID id        = ui_hash(title) ^ MENU_HASH_MAGIC;

    // 计算布局大小
    float cx    = text_width(text, _tcslen(text)) + style()->framePadding.x * 4.0f;
    float cy    = style()->menuHeight;
    uiRect rect = allocate_widget_rect(cx, cy);

    // 创建部件
    uiWidget widget(UI_WIDGET_MENU_ITEM, text, id, layout()->container);
    widget.bounds = rect;

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget, true);
    bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);
    bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

    if (clicked) {
        if (ctx->openPath.empty()) { // 添加到路径列表
            ctx->openPath.push_back(id);
        }
        else { // 关闭菜单
            ctx->openPath.clear();
        }
    }
    else if (check_flags(eventFlags, UI_FLAGS_HOVERED) && !ctx->openPath.empty()) {
        ctx->openPath.clear(); // 重新打开菜单
        ctx->openPath.push_back(id);
    }

    // 绘制
    draw_label(
        window_cmdlist(), &widget, UI_CENTER,
        select_text_foreground(hovered, false), uiColor(), select_color(style()->widgetBackground, hovered, false)
    );

    // 判断是否弹出
    bool isOpen = false;
    if (!ctx->openPath.empty() && ctx->openPath[0] == id) {
        isOpen = true;
    }

    // 弹出菜单
    if (isOpen) {
        ctx->menuStack.clear();
        ctx->menuStack.push_back(id);
        // printf("menu push\n");

        // begin_menu_window(title.c_str(), id, uiVec2(rect.x, rect.y + rect.h), uiVec2(MENU_ITEM_MIN_WIDTH, style()->framePadding.y * 2.0f));
        begin_menu_window(title.c_str(), id, &widget, true);
    }

    return isOpen;
}

static void end_main_menu()
{
    uiContext* ctx = context();

    end_menu_window();

    ctx->menuStack.pop_back();
    // printf("menu pop\n");

    assert(ctx->menuStack.empty()); // 最后必须是空
}

// 子菜单菜单项
static bool begin_sub_menu(const uiChar* text)
{
    uiContext* ctx = context();

    uiString title = layout()->owner->text + (layout()->owner->type == UI_WIDGET_MENU ? _T(".") : _T(".menu.")) + text;
    const uiID id  = ui_hash(title) ^ MENU_HASH_MAGIC;

    // 计算布局大小
    uiRect rect = allocate_widget_rect(-1.0f, style()->menuHeight);

    // 创建部件
    uiWidget widget(UI_WIDGET_MENU_ITEM, text, 0, layout()->container);
    widget.bounds = rect;

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget, false);
    // bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);
    bool hovered = check_flags(eventFlags, UI_FLAGS_HOVERED);

    if (hovered) {
        // printf("path: %u -> stack: %u  submenu\n", ctx->openPath.size(), ctx->menuStack.size());
        ctx->openPath.resize(ctx->menuStack.size() + 1);
        ctx->openPath[ctx->menuStack.size()] = id;
    }

    // 绘制
    draw_menu_item(window_cmdlist(), &widget, hovered, false, true);

    // 判断打开路径里面是否有菜单 ID
    bool isOpen = false;
    if (ctx->openPath.size() > ctx->menuStack.size() && ctx->openPath[ctx->menuStack.size()] == id) {
        isOpen = true;
    }

    // 打开菜单
    if (isOpen) {
        ctx->menuStack.push_back(id);
        // printf("submenu push\n");

        // begin_menu_window(title.c_str(), id, uiVec2(rect.x + rect.w - 4.0f, rect.y - 4.0f), uiVec2(MENU_ITEM_MIN_WIDTH, style()->framePadding.y * 2.0f));
        begin_menu_window(title.c_str(), id, &widget, false);
    }

    return isOpen;
}

static void end_sub_menu()
{
    uiContext* ctx = context();

    end_menu_window();
    ctx->menuStack.pop_back();
    // printf("submenu pop\n");
}

// 菜单开始
bool begin_menu(const uiChar* text)
{
    uiContext* ctx = context();

    // bool isMainMenu = (layout()->container->type == UI_WIDGET_MENU_BAR);
    if (ctx->menuStack.empty()) {
        return begin_main_menu(text);
    }
    else {
        return begin_sub_menu(text);
    }
}

// 菜单结束
void end_menu()
{
    /*
    uiContext* ctx = context();

    printf("%d: [", ctx->openLevels);
    for (auto id : ctx->openPath) {
        printf("%llu, ", id);
    }
    printf("]\n");
    */
    uiContext* ctx = context();
    // bool isMainMenu = (layout()->container->type == UI_WIDGET_MENU_BAR);
    if (ctx->menuStack.size() == 1) {
        end_main_menu();
    }
    else {
        end_sub_menu();
    }
}

// 菜单项
bool menu_item(const uiChar* text)
{
    // 分隔符
    if (_tcscmp(text, _T("-")) == 0) {
        separator(uiString());
        return false;
    }

    uiContext* ctx = context();

    uiWidget widget(UI_WIDGET_MENU_ITEM, text, 0, layout()->container);

    // 计算布局大小
    float cy      = char_height() + style()->framePadding.y * 2.0f;
    widget.bounds = allocate_widget_rect(-1.0f, cy);

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget, false);
    bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);
    bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

    if (hovered) {
        // printf("path: %u -> stack: %u\n", ctx->openPath.size(), ctx->menuStack.size());
        ctx->openPath.resize(ctx->menuStack.size()); // 关闭下层打开的菜单
    }
    if (clicked) {
        cancel_popup();
        bring_to_back(layout()->owner);
    }

    // 绘制
    draw_menu_item(window_cmdlist(), &widget, hovered, false, false);

    return clicked;
}

//------------------------------------------------------------------------------

// 折叠面板
bool collapsible_panel(const uiChar* text, uiFlags panelFlags)
{
    uiWidget widget(UI_WIDGET_COLLAPSIBLE, text, 0, layout()->container);

    float cy      = char_height() + style()->framePadding.y * 2.0f;
    widget.bounds = allocate_widget_rect_full_line(cy);

    // 获取缓存的状态
    uiFlags& state = pick_cache_flags(widget.id, check_flags(panelFlags, UI_FLAGS_OPEN) ? UI_FLAGS_OPEN : 0);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return check_flags(state, UI_FLAGS_OPEN);
    }

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);

    // printf("eventFlags: %08x, state: %08x\n", eventFlags, state);

    if (clicked) {
        set_flags_enabled(state, UI_FLAGS_OPEN, !check_flags(state, UI_FLAGS_OPEN));
    }

    // 绘制
    bool expanded = check_flags(state, UI_FLAGS_OPEN);
    draw_collapsible(window_cmdlist(), &widget, !expanded);

    return expanded;
}

//------------------------------------------------------------------------------
// 标签页

static void label_widget(const uiChar* text, float width = -10.0f)
{
    uiWidget widget(UI_WIDGET_LABEL, text, 0);

    float cx = width;
    if (cx < -1.0f) {
        cx = text_width(text, _tcslen(text)) + style()->framePadding.x * 2.0f;
    }

    float cy      = char_height() + style()->framePadding.y * 2.0f;
    widget.bounds = allocate_widget_rect(cx, cy);

    if (is_visible_clipbox(&widget)) {
        draw_label(window_cmdlist(), &widget, UI_LEFT, style()->foreground[UI_COLOR_NORMAL], uiColor(), uiColor());
    }
}

// 标签页
void label_va(float width, const uiChar* param, va_list body)
{
    static uiChar buf[256] = { 0 };

    _vsntprintf(buf, countof(buf), param, body);

    label_widget(buf, width);
}

void label(const uiChar* param, ...)
{
    va_list body;
    va_start(body, param);
    label_va(-2.0f, param, body);
    va_end(body);
}

// 固定宽度标签页
void label(float width, const uiChar* param, ...)
{
    va_list body;
    va_start(body, param);
    label_va(width, param, body);
    va_end(body);
}

//------------------------------------------------------------------------------

// 按钮
bool button(const uiChar* text, uiVec2 size)
{
    uiWidget widget(UI_WIDGET_BUTTON, text, 0, layout()->container);

    float cx      = is_zero(size.x) ? text_width(text, _tcslen(text)) + style()->framePadding.x * 4.0f : size.x;
    float cy      = is_zero(size.y) ? char_height() + style()->framePadding.y * 2.0f : size.y;
    widget.bounds = allocate_widget_rect(cx, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    uiFlags eventFlags = check_widget_mouse_event(&widget);

    draw_button(window_cmdlist(), &widget);

    return check_flags(eventFlags, UI_FLAGS_CLICKED);
}

// 显示颜色框
void color_box(uiColor color, float width)
{
    uiWidget widget(UI_WIDGET_COLOR);

    float cy      = char_height();
    widget.bounds = allocate_widget_rect(width, cy);

    if (is_visible_clipbox(&widget)) {
        draw_frame(window_cmdlist(), widget.bounds, select_color(style()->widgetForeground, false, false), color);
    }
}

// 单选框，ID 生成使用变量地址
bool checkbox(const uiChar* text, bool* checked)
{
    uiWidget widget(UI_WIDGET_CHECKBOX, text, ui_hash(&checked, sizeof(void*)), layout()->container);

    float cx      = text_width(text, _tcslen(text)) + style()->framePadding.x * 2.0f;
    float cy      = char_height() + style()->framePadding.y * 2.0f;
    widget.bounds = allocate_widget_rect(cy + 4.0f + cx, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);

    if (clicked) {
        *checked = !(*checked);
    }

    // 绘制
    draw_check_box(window_cmdlist(), &widget, *checked);

    return clicked;
}

// 可选择按钮
bool selectable(const uiChar* text, bool selected, uiVec2 size)
{
    uiWidget widget(UI_WIDGET_BUTTON, text, 0, layout()->container);

    float cx      = is_zero(size.x) ? text_width(text, _tcslen(text)) + style()->framePadding.x : size.x;
    float cy      = is_zero(size.y) ? char_height() + style()->framePadding.y : size.y;
    widget.bounds = allocate_widget_rect(cx, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

    // 绘制
    uiColor textcolor  = select_text_foreground(hovered, selected);
    uiColor background = select_text_background(hovered, selected);
    draw_label(window_cmdlist(), &widget, UI_LEFT, textcolor, uiColor(), background);

    return check_flags(eventFlags, UI_FLAGS_CLICKED);
}

//------------------------------------------------------------------------------

// 分隔符
void separator(const uiString& text)
{
    // 分隔符高度
    const float cy    = text.empty() ? char_height() * 0.5f : char_height();
    const uiRect rect = allocate_widget_rect(-1.0f, cy);

    // 绘制
    if (is_visible_clipbox(rect)) {
        draw_separator(window_cmdlist(), rect, text);
    }
}
//------------------------------------------------------------------------------

// 滑块，ID 生成使用变量地址
bool slider(const uiChar* text, float* value, float min, float max)
{
    uiWidget widget(UI_WIDGET_SLIDER, text, ui_hash(&value, sizeof(void*)), layout()->container);

    float cy      = char_height() + style()->framePadding.y * 2.0f;
    widget.bounds = allocate_widget_rect(-1.0f, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    // 事件处理
    check_widget_mouse_event(&widget);
    bool changed = false;

    // 拖拽
    if (is_capture(&widget)) {
        const float sliderSize  = style()->sliderSize;
        const float sliderRange = widget.bounds.w - sliderSize;
        const float sliderPos   = clamp(context()->mouse.x - widget.bounds.x - sliderSize * 0.5f, 0.0f, sliderRange);
        *value                  = min + (sliderPos / sliderRange) * (max - min);
        changed                 = true;
    }

    // 绘制
    draw_slider(window_cmdlist(), &widget, *value, min, max);

    return changed;
}

//------------------------------------------------------------------------------

// 计算滚动条滑块大小 slider = size * (size / (max - min + size));
static float compute_scroll_slider_size(
    float min, // 最小值
    float max, // 最大值
    float size // 滚动条长度
)
{
    const float range = max - min; // 范围
    if (range <= 0) {
        return size;
    }

    const float minSize = style()->scrollSliderMin; // 大尺度滑块最小长度
    float ratio         = size / (range + size);
    float slider        = size * ratio;

    return slider < minSize ? minSize : slider;
}

// 横向滚动条
bool hscroll(const uiChar* text, const uiRect& rect, float* value, float min, float max)
{
    uiWidget widget(UI_WIDGET_SCROLL, text, 0, layout()->container);
    widget.bounds = rect;

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    check_widget_mouse_event(&widget);

    // 计算当前滑块位置
    const float sliderSize  = compute_scroll_slider_size(min, max, widget.bounds.w);
    const float sliderRange = widget.bounds.w - sliderSize;
    float sliderPos         = (*value - min) / (max - min) * sliderRange;
    bool changed            = false;

    // 拖拽
    if (is_capture(&widget)) {
        float mouse = context()->mouse.x; // - context()->delta.x;

        // 判断鼠标是否在滑块上
        if (mouse > widget.bounds.x + sliderPos && mouse < widget.bounds.x + sliderPos + sliderSize) {
            // 拖动滑块
            sliderPos        = clamp(sliderPos + context()->delta.x, 0.0f, sliderRange);
            context()->delta = uiVec2(); // 清理鼠标移动向量
        }
        else {
            // 跳过去
            sliderPos = clamp(context()->mouse.x - widget.bounds.x - sliderSize * 0.5f, 0.0f, sliderRange);
        }
        *value  = (sliderPos / sliderRange) * (max - min) + min;
        changed = true;
    }

    // 绘制
    draw_hscroll(window_cmdlist(), &widget, *value, sliderPos, sliderSize);

    return changed;
}

// 竖向滚动条
bool vscroll(const uiChar* text, const uiRect& rect, float* value, float min, float max)
{
    uiWidget widget(UI_WIDGET_SCROLL, text, 0, layout()->container);
    widget.bounds = rect;

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        // return false;
    }

    check_widget_mouse_event(&widget);

    if (is_hovered(&widget)) {
        wprintf(_T("container: %ls\n"), layout()->container->text.c_str());
    }

    // 计算当前滑块位置
    const float sliderSize  = compute_scroll_slider_size(min, max, widget.bounds.h);
    const float sliderRange = widget.bounds.h - sliderSize;
    float sliderPos         = (*value - min) / (max - min) * sliderRange;
    bool changed            = false;

    // 拖拽
    if (is_capture(&widget)) {
        float mouse = context()->mouse.y; // - context()->delta.y;

        // 判断鼠标是否在滑块上
        if (mouse > widget.bounds.y + sliderPos && mouse < widget.bounds.y + sliderPos + sliderSize) {
            // 拖动滑块
            sliderPos        = clamp(sliderPos + context()->delta.y, 0.0f, sliderRange);
            context()->delta = uiVec2(); // 清理鼠标移动向量
        }
        else {
            // 跳过去
            sliderPos = clamp(context()->mouse.y - widget.bounds.y - sliderSize * 0.5f, 0.0f, sliderRange);
        }
        *value  = (sliderPos / sliderRange) * (max - min) + min;
        changed = true;
    }
    else {
        // wprintf(L"err: %ls\n", layout()->container->text.c_str());
    }

    // 绘制
    draw_vscroll(window_cmdlist(), &widget, *value, sliderPos, sliderSize);
    // draw_vscroll(foreground_cmdlist(), &widget, *value, sliderPos, sliderSize);

    return changed;
}

bool hscroll(const uiChar* text, float* value, float min, float max)
{
    return hscroll(text, allocate_widget_rect(-1.0f, style()->scrollWidth), value, min, max);
}

bool vscroll(const uiChar* text, float* value, float min, float max)
{
    return vscroll(text, allocate_widget_rect(style()->scrollWidth, -1.0f), value, min, max);
}

//------------------------------------------------------------------------------

// 开始列表框
bool begin_list(const uiChar* text)
{
    return begin_child(text, UI_PANEL_SCROLL);
}

// 结束列表框
void end_list()
{
    end_child();
}

//------------------------------------------------------------------------------

// 开始组合框
bool begin_combo(const uiChar* text)
{
    uiWidget widget(UI_WIDGET_BUTTON, text, 0);

    float width = -1.0f;
    float cx    = width;
    if (cx < -1.0f) {
        cx = text_width(text, _tcslen(text)) + style()->framePadding.x * 2.0f;
    }

    float cy      = char_height() + style()->framePadding.y * 2.0f;
    widget.bounds = allocate_widget_rect(cx, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);
    bool opened        = context()->popupWidget == widget.id;

    // 如果点击，打开列表
    if (clicked) {
        opened = !opened;
    }

    // 绘制
    draw_combo(window_cmdlist(), &widget, opened);

    // 打开列表
    // printf("combo: %u\n", state);
    if (opened) {
        context()->popupWidget = widget.id;
        begin_popup(
            L"combo.listbox",
            uiVec2(widget.bounds.x, widget.bounds.y + widget.bounds.h + 2.0f),
            uiVec2(widget.bounds.w, 200)
        );
    }
    else {
        context()->popupWidget = 0;
    }

    return opened;
}

// 结束组合框
void end_combo()
{
    end_popup();
}

//------------------------------------------------------------------------------

// 树节点
bool treenode(const uiChar* text, uiFlags nodeFlags)
{
    uiWidget widget(UI_WIDGET_TREENODE, text, 0, layout()->container);

    float cy      = char_height() + style()->framePadding.y * 2.0f;
    float cx      = -1.0f; // text_width(text, _tcslen(text)) + style()->framePadding.x * 2.0f + cy + 4.0f;
    widget.bounds = allocate_widget_rect(cx, cy);

    // 获取缓存的状态
    uiFlags& state = pick_cache_flags(widget.id, nodeFlags);

    // 判断是否在剪裁范围内
    if (is_visible_clipbox(&widget)) {
        // 事件处理
        uiFlags eventFlags = check_widget_mouse_event(&widget);
        bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);
        bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

        if (clicked) {
            set_flags_enabled(state, UI_FLAGS_OPEN, !check_flags(state, UI_FLAGS_OPEN));
        }

        // 绘制
        draw_treenode(window_cmdlist(), &widget, check_flags(state, UI_FLAGS_OPEN), select_text_foreground(hovered, false));
    }

    if (check_flags(state, UI_FLAGS_OPEN)) {
        indent(24.0f); // 增加缩进
        return true;
    }

    return false;
}

// 自定义颜色的树节点
bool treenode(const uiChar* text, uiColor color, uiFlags nodeFlags)
{
    uiWidget widget(UI_WIDGET_TREENODE, text, 0, layout()->container);

    float cy      = char_height() + style()->framePadding.y * 2.0f;
    float cx      = -1.0f; // text_width(text, _tcslen(text)) + style()->framePadding.x * 2.0f + cy + 4.0f;
    widget.bounds = allocate_widget_rect(cx, cy);

    // 获取缓存的状态
    uiFlags& state = pick_cache_flags(widget.id, nodeFlags);

    // 判断是否在剪裁范围内
    if (is_visible_clipbox(&widget)) {
        // 事件处理
        uiFlags eventFlags = check_widget_mouse_event(&widget);
        bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);
        // bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

        if (clicked) {
            set_flags_enabled(state, UI_FLAGS_OPEN, !check_flags(state, UI_FLAGS_OPEN));
        }

        // 绘制
        draw_treenode(window_cmdlist(), &widget, check_flags(state, UI_FLAGS_OPEN), color);
    }

    if (check_flags(state, UI_FLAGS_OPEN)) {
        indent(24.0f); // 增加缩进
        return true;
    }
    return false;
}

void tree_pop()
{
    unindent(24.0f); // 减少缩进
}

//------------------------------------------------------------------------------

// 整形拖拽框，ID 生成使用变量地址
template<typename T>
bool drag_int(float width, T* value, int min, int max)
{
    uiWidget widget(UI_WIDGET_DRAG_NUMBER, uiString(), ui_hash(&value, sizeof(void*)), layout()->container);

    float cy      = char_height();
    widget.bounds = allocate_widget_rect(width, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    // 事件处理
    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

    bool changed = false; // 是否更改

    // 拖拽
    if (is_capture(&widget)) {
        uiVec2i delta = pick_delta_int_acc(1, 0.25f);
        *value        = clamp(*value + delta.x, min, max);
        changed       = true;
    }

    // 绘制
    widget.text = format(_T("%d"), (int) *value);
    draw_label(
        window_cmdlist(), &widget, UI_CENTER,
        select_text_foreground(hovered, false),
        select_color(style()->widgetForeground, hovered, false),
        style()->background[UI_COLOR_NORMAL]
    );

    return changed;
}

// 整形拖拽框，ID 生成使用变量地址
bool drag_int(float width, int* value, int min, int max)
{
    return drag_int<int>(width, value, min, max);
}

// 浮点拖拽框，ID 生成使用变量地址
bool drag_float(float width, float* value, float min, float max)
{
    uiWidget widget(UI_WIDGET_DRAG_NUMBER, uiString(), ui_hash(&value, sizeof(void*)), layout()->container);

    float cy      = char_height();
    widget.bounds = allocate_widget_rect(width, cy);

    // 判断是否在剪裁范围内
    if (!is_visible_clipbox(&widget)) {
        return false;
    }

    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool hovered       = check_flags(eventFlags, UI_FLAGS_HOVERED);

    bool changed = false; // 是否更改

    // 拖拽
    if (is_capture(&widget)) {
        uiVec2 delta = pick_delta(1.0f);
        *value       = clamp(*value + delta.x, min, max);
        changed      = true;
    }

    // 绘制
    widget.text = format(_T("%0.2f"), *value);
    draw_label(
        window_cmdlist(), &widget, UI_CENTER,
        select_text_foreground(hovered, false),
        select_color(style()->widgetForeground, hovered, false),
        style()->background[UI_COLOR_NORMAL]
    );

    return changed;
}

// 颜色拖拽框，ID 生成使用变量地址
bool drag_color(uint8_t* color, float width)
{
    (void) width;

    float w = layout()->remaining_width() - layout()->space.x * 4.0f;

    bool changed = false;
    changed |= drag_int<uint8_t>(w * 0.2f, &color[0], 0, 255);
    sameline();
    changed |= drag_int<uint8_t>(w * 0.2f, &color[1], 0, 255);
    sameline();
    changed |= drag_int<uint8_t>(w * 0.2f, &color[2], 0, 255);
    sameline();
    changed |= drag_int<uint8_t>(w * 0.2f, &color[3], 0, 255);
    sameline();
    color_box(*reinterpret_cast<uiColor*>(color), w * 0.2f);

    return changed;
}

bool drag_color(float* color, float width)
{
    (void) width;

    float w = layout()->remaining_width() - layout()->space.x * 4.0f;

    bool changed = false;
    changed |= drag_float(w * 0.2f, &color[0], 0, 255);
    sameline();
    changed |= drag_float(w * 0.2f, &color[1], 0, 255);
    sameline();
    changed |= drag_float(w * 0.2f, &color[2], 0, 255);
    sameline();
    changed |= drag_float(w * 0.2f, &color[3], 0, 255);
    sameline();
    color_box(*reinterpret_cast<uiColor*>(color), w * 0.2f);

    return changed;
}

bool drag_color(uiColor& color, float width)
{
    return drag_color(&color.r, width);
}

// 测试无窗口部件
bool custom_test(const uiChar* text, float x, float y, float w, float h)
{
    uiWidget widget(UI_WIDGET_SLIDER, text, 0);

    widget.bounds = uiRect(x, y, w, h);

    uiFlags eventFlags = check_widget_mouse_event(&widget);
    bool clicked       = check_flags(eventFlags, UI_FLAGS_CLICKED);

    // 拖拽
    if (is_capture(&widget)) {
    }

    // 绘制
    if (is_visible_clipbox(&widget)) {
        draw_button(foreground_cmdlist(), &widget);
        // draw_frame(window_cmdlist(), &widget, style()->widgetForeground[UI_COLOR_NORMAL], style()->background[UI_COLOR_NORMAL]);
    }

    return clicked;
}

//------------------------------------------------------------------------------

} // namespace minimgui
