#include "ui_widgets.h"
#include "ui_user.h"

namespace minimgui {

// 选择颜色
uiColor select_color(uiColor* colorMode, bool hovered, bool selected, bool disabled)
{
    if (disabled) {
        return colorMode[UI_COLOR_DISABLE];
    }
    else if (selected) {
        return colorMode[UI_COLOR_HIGHLIGHT];
    }
    else if (hovered) {
        return colorMode[UI_COLOR_HOTLIGHT];
    }
    else {
        return colorMode[UI_COLOR_NORMAL];
    }
}

// 选择部件背景色
/*
uiColor select_widget_background(bool hovered, bool selected, bool disabled)
{
    if (disabled) {
        return style()->widgetBackground[UI_COLOR_DISABLE];
    }
    else if (selected) {
        return style()->widgetBackground[UI_COLOR_HIGHLIGHT];
    }
    else if (hovered) {
        return style()->widgetBackground[UI_COLOR_HOTLIGHT];
    }
    else {
        return style()->widgetBackground[UI_COLOR_NORMAL];
    }
}

// 选择部件前景色
uiColor select_widget_foreground(bool hovered, bool selected, bool disabled)
{
    if (disabled) {
        return style()->widgetForeground[UI_COLOR_DISABLE];
    }
    else if (selected) {
        return style()->widgetForeground[UI_COLOR_HIGHLIGHT];
    }
    else if (hovered) {
        return style()->widgetForeground[UI_COLOR_HOTLIGHT];
    }
    else {
        return style()->widgetForeground[UI_COLOR_NORMAL];
    }
}
*/

// 选择文本背景色
uiColor select_text_background(bool hovered, bool selected, bool disabled)
{
    if (disabled) {
        return style()->background[UI_COLOR_DISABLE];
    }
    else if (selected) {
        return style()->background[UI_COLOR_HIGHLIGHT];
    }
    else if (hovered) {
        return style()->background[UI_COLOR_HOTLIGHT];
    }
    else {
        return style()->background[UI_COLOR_NORMAL];
    }
}

// 选择文本前景色
uiColor select_text_foreground(bool hovered, bool selected, bool disabled)
{
    if (disabled) {
        return style()->foreground[UI_COLOR_DISABLE];
    }
    else if (selected) {
        return style()->foreground[UI_COLOR_HIGHLIGHT];
    }
    else if (hovered) {
        return style()->foreground[UI_COLOR_HOTLIGHT];
    }
    else {
        return style()->foreground[UI_COLOR_NORMAL];
    }
}

//------------------------------------------------------------------------------

// 填充背景
// static void fill_background(uiCommandList* cmdlist, const uiRect& rect, const uiColor& color)
//{
//    if (color.a) {
//        cmdlist->set_fill(color);
//        cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);
//    }
//}

//------------------------------------------------------------------------------

// 绘制边框和背景
void draw_frame(uiCommandList* cmdlist, const uiRect& rect, const uiColor& border, const uiColor& background)
{
    // 背景
    if (background.a) {
        cmdlist->set_fill(background);
        cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);
    }

    // 边框
    if (border.a) {
        cmdlist->set_stroke(border);
        cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);
    }
}

void draw_frame(uiCommandList* cmdlist, uiWidget* widget, const uiColor& border, const uiColor& background)
{
    draw_frame(cmdlist, widget->bounds, border, background);
}

// 通用颜色边框
// void draw_widget_frame(uiCommandList* cmdlist, const uiRect& rect, bool hovered, bool selected)
// {
//    draw_frame(cmdlist, rect, select_widget_foreground(hovered, selected), select_text_background(hovered, selected));
// }

// 绘制滑块
static inline void draw_thumb(uiCommandList* cmdlist, float x, float y, float w, float h, bool hovered, bool down)
{
    cmdlist->set_fill(select_color(style()->thumbForeground, hovered, down));
    cmdlist->fill_rect(x, y, w, h);
}

// 绘制标题
void draw_title(
    uiCommandList* cmdlist,
    float x, float y, float w, float h,
    const uiString& str,
    uiAlign align,
    uiColor color
)
{
    const float cx = text_width(str.c_str(), str.length());
    const float cy = char_height();

    cmdlist->set_text(color);

    if (align == UI_RIGHT) {
        cmdlist->draw_string(x + w - cx - context()->style.framePadding.x, y + (h - cy) * 0.5f, str);
    }
    else if (align == UI_CENTER) {
        cmdlist->draw_string(x + (w - cx) * 0.5f, y + (h - cy) * 0.5f, str);
    }
    else {
        cmdlist->draw_string(x + context()->style.framePadding.x, y + (h - cy) * 0.5f, str);
    }
}

//------------------------------------------------------------------------------

// 绘制窗口
void draw_window(uiCommandList* cmdlist, uiWindow* window)
{
    // const float titleHeight = context()->style.titleHeight;                       // 标题栏高度
    const float titleHeight = ((window->panelFlags & UI_PANEL_TITLE) ? style()->titleHeight : 0.0f);
    const bool hasTitle     = check_flags(window->panelFlags, UI_PANEL_TITLE);    // 是否拥有标题
    const bool isActived    = is_actived(window);                                 // 是否激活
    const bool isCollapsed  = check_flags(window->panelFlags, UI_PANEL_COLLAPSE); // 是否折叠

    // 窗口矩形
    uiRect rect = window->bounds;

    bool show = true;

    // 如果有标题栏
    if (hasTitle) {
        // 标题栏背景
        cmdlist->set_fill(select_color(style()->titleBackground, false, isActived));
        cmdlist->fill_rect(rect.x, rect.y, rect.w, titleHeight);

        // 标题
        draw_title(
            cmdlist,
            rect.x, rect.y, rect.w, titleHeight,
            format(_T("%s : %llu"), window->text.c_str(), window->id),
            // window->text,
            UI_CENTER,
            select_color(style()->titleForeground, false, isActived)
        );

        // 折叠标记
        draw_updown_arrow(cmdlist, rect.x, rect.y, titleHeight, titleHeight, 0.8f, isCollapsed);

        // 关闭
        if (window->pIsOpen) {
            draw_close_icon(cmdlist, rect.x + rect.w - titleHeight, rect.y, titleHeight, titleHeight);
        }

        if (isCollapsed) {
            rect.h = titleHeight;
            show   = false;
        }
        else if (window->bounds.h <= titleHeight) {
            show = false;
        }
    }

    // 如果没有折叠
    // if (!isCollapsed && (hasTitle && window->bounds.h > titleHeight)) {
    if (show) {
        // 背景
        cmdlist->set_fill(style()->windowBackground);
        cmdlist->fill_rect(rect.x, rect.y + titleHeight, rect.w, rect.h - titleHeight);

        // 三角形绘制大小略比鼠标判定大小小一些
        const float TRIANGLE_SIZE = style()->triangleSize * 0.75f;

        // 垂直滚动条
        if (window->scrollMax.y > 0.0f) {
            // const float titleHeight = ((window->panelFlags & UI_PANEL_TITLE) ? style()->titleHeight : 0.0f);
            const float scrollWidth = style()->scrollWidth;
            const float height =
                window->bounds.h - titleHeight -
                (check_flags(window->panelFlags, UI_PANEL_RESIZABLE) ? TRIANGLE_SIZE - scrollWidth : 0.0f); // 三角大小

            // 如果小于滑块最小大小，则不显示
            if (height > style()->scrollSliderMin) {
                uiRect rect = uiRect(
                    window->bounds.x + window->bounds.w - style()->scrollWidth,
                    window->bounds.y + titleHeight,
                    style()->scrollWidth, // 宽度
                    height
                );

                const float padding   = check_flags(window->panelFlags, UI_PANEL_PADDING) ? style()->windowPadding.y * 2.0f : 0.0f;
                const float scrollMax = window->scrollMax.y - height + padding;

                // 滚轴滚动
                if (is_hovered(window) && context()->hoveredWidget == 0) {
                    window->scroll.y  = clamp(window->scroll.y - context()->scroll, 0.0f, scrollMax);
                    context()->scroll = 0.0f;
                }

                // 绘制滚动条
                vscroll(format(_T("%ls.vscroll"), window->text.c_str()), rect, &window->scroll.y, 0.0f, scrollMax);
                // vscroll(format(_T("%p.vscroll"), window), rect, &window->scroll.y, 0.0f, scrollMax);
            }
        }

        // 缩放三角形
        if (check_flags(window->panelFlags, UI_PANEL_RESIZABLE)) {
            const uiVec2 triangle[] = {
                uiVec2(rect.x + rect.w, rect.y + rect.h - TRIANGLE_SIZE),
                uiVec2(rect.x + rect.w - TRIANGLE_SIZE, rect.y + rect.h),
                uiVec2(rect.x + rect.w, rect.y + rect.h),
            };
            cmdlist->set_fill(select_color(style()->windowForeground, false, isActived));
            cmdlist->fill_triangle(triangle);
        }
    }

    // 边框
    cmdlist->set_fill(select_color(style()->windowForeground, false, isActived));
    cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);
}

// 绘制折叠面板
void draw_collapsible(uiCommandList* cmdlist, uiWidget* widget, bool isCollapsed)
{
    const bool isHovered = is_hovered(widget);
    uiRect rect          = widget->bounds;

    // 标题栏背景
    cmdlist->set_fill(select_color(style()->widgetBackground, isHovered, false));
    cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);

    // 折叠标记
    draw_updown_arrow(cmdlist, rect.x, rect.y, rect.h, rect.h, 0.5f, isCollapsed);

    // 标题
    draw_title(
        cmdlist,
        rect.x, rect.y, rect.w, rect.h,
        widget->text,
        UI_CENTER,
        select_text_foreground(isHovered, false)
    );

    // 边框
    cmdlist->set_fill(style()->widgetForeground[UI_COLOR_NORMAL]);
    cmdlist->draw_line(rect.x, rect.y, rect.x + rect.w, rect.y);
    cmdlist->draw_line(rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
}

// 绘制菜单窗口
size_t draw_menu_window(uiCommandList* cmdlist, uiWindow* window)
{
    // 窗口矩形
    uiRect rect = window->bounds;

    // 背景
    cmdlist->set_fill(style()->widgetBackground[UI_COLOR_NORMAL]);
    size_t pos = cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);

    // 边框
    cmdlist->set_stroke(style()->windowForeground[UI_COLOR_NORMAL]);
    cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);

    return pos;
}

// 绘制菜单项
void draw_menu_item(uiCommandList* cmdlist, uiWidget* widget, bool hovered, bool checked, bool hasSubMenu)
{
    uiRect rect = widget->bounds;

    // 背景
    if (hovered) {
        cmdlist->set_fill(style()->widgetBackground[UI_COLOR_HIGHLIGHT]);
        cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);
    }

    // 选择
    if (checked) {
        // todo...
    }

    // 文字
    float x = rect.x + style()->framePadding.x + style()->menuHeight; // 边距 + 图标
    cmdlist->set_text(select_text_foreground(false, false));
    cmdlist->draw_string(x, rect.y + style()->framePadding.y, widget->text);

    // 右侧箭头（子菜单指示）
    if (hasSubMenu) {
        float ax = rect.x + rect.w - 16.0f;
        float ay = rect.y + rect.h * 0.5f;
        // 小三角
        uiVec2 points[] = {
            uiVec2(ax, ay - 4),
            uiVec2(ax + 6, ay),
            uiVec2(ax, ay + 4)
        };
        cmdlist->set_fill(colors::black);
        cmdlist->fill_triangle(points);
    }
}

// 绘制标签
void draw_label(uiCommandList* cmdlist, const uiRect& rect, const uiString& text, uiAlign align, uiColor textcolor, uiColor foreground, uiColor background)
{
    // 背景
    if (background.a) {
        cmdlist->set_fill(background);
        cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);
    }

    // 边框
    if (foreground.a) {
        cmdlist->set_stroke(foreground);
        cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);
    }

    // 标题
    draw_title(
        cmdlist,
        rect.x, rect.y, rect.w, rect.h,
        text,
        align,
        textcolor
    );
}

void draw_label(uiCommandList* cmdlist, uiWidget* widget, uiAlign align, uiColor textcolor, uiColor foreground, uiColor background)
{
    draw_label(cmdlist, widget->bounds, widget->text, align, textcolor, foreground, background);
}

// 绘制按钮
void draw_button(uiCommandList* cmdlist, uiWidget* widget)
{
    const bool isHovered = is_hovered(widget);
    const bool isDown    = is_pressed(widget);
    uiRect rect          = widget->bounds;

    uiColor background;
    if (isDown) { // 按下
        background = style()->widgetBackground[UI_COLOR_HIGHLIGHT];
    }
    else if (isHovered) {
        background = style()->widgetBackground[UI_COLOR_HOTLIGHT];
    }
    else {
        background = style()->widgetBackground[UI_COLOR_NORMAL];
    }

    // 背景
    cmdlist->set_fill(background);
    cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);

    // 边框
    cmdlist->set_stroke(style()->widgetForeground[UI_COLOR_NORMAL]);
    cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);

    if (isDown) {
        rect.x += 1.0f;
        rect.y += 1.0f;
    }

    // 标题
    draw_title(
        cmdlist,
        rect.x, rect.y,
        rect.w, rect.h,
        widget->text,
        UI_CENTER,
        select_text_foreground(isHovered, false)
    );
}

// 绘制选择框
void draw_check_box(uiCommandList* cmdlist, uiWidget* widget, bool checked)
{
    const bool isHovered = is_hovered(widget);
    uiRect rect          = widget->bounds;
    float boxPos         = rect.h * 0.1f; // 单选框大小
    float boxSize        = rect.h * 0.8f; // 单选框大小

    // 边框
    cmdlist->set_stroke(style()->widgetForeground[UI_COLOR_NORMAL]);
    cmdlist->draw_rect(rect.x + boxPos, rect.y + boxPos, boxSize, boxSize);

    // 选择
    if (checked) {
        cmdlist->set_fill(style()->widgetForeground[UI_COLOR_NORMAL]);
        /*
        float x = rect.x + boxSize * 0.5f;
        float y = rect.y + boxSize * 0.5f;
        fill_circle(x, y, boxSize * 0.5f);
        */
        boxPos += boxSize * 0.2f;
        boxSize *= 0.6f;
        cmdlist->fill_rect(rect.x + boxPos, rect.y + boxPos, boxSize, boxSize);
    }

    // 标题
    draw_title(
        cmdlist,
        rect.x + rect.h, rect.y, rect.w - rect.h, rect.h,
        widget->text,
        UI_LEFT,
        select_text_foreground(isHovered, false)
    );
}

// 绘制组合框
void draw_combo(uiCommandList* cmdlist, uiWidget* widget, bool opened)
{
    const bool isHovered = is_hovered(widget);
    uiRect rect          = widget->bounds;
    float btnSize        = 24.0f; // 按钮大小

    // 背景
    cmdlist->set_fill(style()->background[UI_COLOR_NORMAL]);
    cmdlist->fill_rect(rect.x, rect.y, rect.w - btnSize, rect.h);
    cmdlist->set_fill(select_color(style()->widgetBackground, isHovered, false));
    cmdlist->fill_rect(rect.x + rect.w - btnSize, rect.y, btnSize, rect.h);

    // 边框
    cmdlist->set_stroke(style()->widgetForeground[UI_COLOR_NORMAL]);
    cmdlist->draw_rect(rect.x, rect.y, rect.w, rect.h);

    // 是否打开
    float x = rect.x + rect.w - btnSize * 0.5f;
    float y = rect.y + rect.h * 0.5f;
    float n = btnSize * 0.25f;
    cmdlist->set_fill(style()->widgetForeground[UI_COLOR_NORMAL]);
    if (opened) {
        const uiVec2 points[] = {
            uiVec2(x - n, y - n),
            uiVec2(x, y + n),
            uiVec2(x + n, y - n),
        };
        cmdlist->fill_triangle(points);
    }
    else {
        const uiVec2 points[] = {
            uiVec2(x, y - n),
            uiVec2(x - n, y + n),
            uiVec2(x + n, y + n),
        };
        cmdlist->fill_triangle(points);
    }

    // 标题
    draw_title(
        cmdlist,
        rect.x + 2.0f, rect.y, rect.w - rect.h - 2.0f, rect.h,
        widget->text,
        UI_LEFT,
        select_text_foreground(isHovered, false)
    );
}

// 绘制滑块
void draw_slider(uiCommandList* cmdlist, uiWidget* widget, float value, float min, float max)
{
    const bool isHovered = is_hovered(widget);
    const bool isDown    = is_pressed(widget);
    uiRect rect          = widget->bounds;

    // 背景
    draw_frame(
        cmdlist, rect,
        select_color(style()->widgetForeground, isHovered, false),
        style()->background[UI_COLOR_NORMAL]
    );

    // 滑块
    const float sliderSize  = context()->style.sliderSize;
    const float sliderRange = rect.w - sliderSize;
    const float sliderPos   = (value - min) / (max - min) * sliderRange;
    draw_thumb(cmdlist, rect.x + sliderPos, rect.y + 1.0f, sliderSize, rect.h - 2.0f, isHovered, isDown);

    // 标题
    draw_title(
        cmdlist,
        rect.x, rect.y, rect.w, rect.h,
        format(_T("%s: %0.3f"), widget->text.c_str(), value),
        UI_CENTER,
        select_text_foreground(isHovered, false)
    );
}

// 绘制滚动条 (需要重复计算参数)
/*
void draw_scroll(uiCommandList* cmdlist, uiWidget* widget, float value, float min, float max)
{
    const bool isHovered = is_hovered(widget);

    // 背景
    cmdlist->set_color(isHovered ? uiColor(0, 0, 0, 80) : uiColor(0, 0, 0, 64));
    cmdlist->fill_rect(widget->bounds.x, widget->bounds.y, widget->bounds.w, widget->bounds.h);

    // 滑块
    const float sliderSize  = context()->style.scrollWidth * 2.0f;
    const float sliderRange = widget->bounds.w - sliderSize;
    const float sliderPos   = (value - min) / (max - min) * sliderRange;
    cmdlist->set_color(isHovered ? uiColor(0, 0, 0, 128) : uiColor(0, 0, 0, 64));
    cmdlist->fill_rect(widget->bounds.x + sliderPos, widget->bounds.y, sliderSize, widget->bounds.h);
}
*/

// 绘制横向滚动条
void draw_hscroll(uiCommandList* cmdlist, uiWidget* widget, float value, float sliderPos, float sliderSize)
{
    const bool isHovered = is_hovered(widget);
    const bool isDown    = is_pressed(widget);
    uiRect rect          = widget->bounds;

    // 背景
    cmdlist->set_fill(style()->trackBackground);
    cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);

    // 滑块
    draw_thumb(cmdlist, rect.x + sliderPos, rect.y, sliderSize, rect.h, isHovered, isDown);

    // 标题
    /*
    draw_title(
        cmdlist,
        rect.x, rect.y, rect.w, rect.h,
        format("%s: %0.3f", widget->text.c_str(), value),
        UI_CENTER,
        isHovered ? uiColor(0, 0, 224, 255) : uiColor(0, 0, 0, 255)
    );
    /*/
    (void) value;
    //*/
}

// 绘制竖向滚动条
void draw_vscroll(uiCommandList* cmdlist, uiWidget* widget, float value, float sliderPos, float sliderSize)
{
    const bool isHovered = is_hovered(widget);
    const bool isDown    = is_pressed(widget);
    uiRect rect          = widget->bounds;

    // 背景
    cmdlist->set_fill(style()->trackBackground);
    cmdlist->fill_rect(rect.x, rect.y, rect.w, rect.h);

    // 滑块
    draw_thumb(cmdlist, rect.x, rect.y + sliderPos, rect.w, sliderSize, isHovered, isDown);

    (void) value;
}

// 绘制分隔符
void draw_separator(uiCommandList* cmdlist, const uiRect& rect, const uiString& text)
{
    const float y = rect.y + rect.h * 0.5f; // 线条 y 坐标

    cmdlist->set_stroke(style()->widgetForeground[UI_COLOR_NORMAL]);
    if (text.empty()) {
        cmdlist->draw_line(rect.x, y, rect.x + rect.w, y);
    }
    else {
        const float space    = style()->itemSpace.x;
        const float forepart = rect.h;                                  // 前半段长度
        const float cx       = text_width(text.c_str(), text.length()); // 文字长度
        cmdlist->draw_line(rect.x, y, rect.x + forepart, y);

        cmdlist->set_text(style()->foreground[UI_COLOR_NORMAL]);
        cmdlist->draw_string(rect.x + forepart + space, rect.y, text);

        float x = forepart + cx + space * 2.0f;
        if (x < rect.w) {
            cmdlist->draw_line(rect.x + x, y, rect.x + rect.w, y);
        }
    }
}

// 绘制树节点
void draw_treenode(uiCommandList* cmdlist, uiWidget* widget, bool opened, uiColor textColor)
{
    const bool isHovered = is_hovered(widget);
    uiRect rect          = widget->bounds;
    const float boxPos   = rect.h * 0.25f; // 箭头位置
    const float boxSize  = rect.h * 0.5f;  // 箭头大小

    // 背景
    if (isHovered) {
        cmdlist->set_fill(style()->background[UI_COLOR_HOTLIGHT]);
        uiRect bgRect = layout()->contentRect;
        bgRect.y      = rect.y;
        bgRect.h      = rect.h;
        cmdlist->fill_rect(bgRect.x, bgRect.y, bgRect.w, bgRect.h);
    }
    // cmdlist->set_stroke(colors::black);
    // cmdlist->draw_rect(rect.x + boxPos, rect.y + boxPos, boxSize, boxSize);

    // 按钮
    cmdlist->set_fill(uiColor(0, 0, 0, 128));
    fill_triangle_arrow(cmdlist, rect.x + boxPos, rect.y + boxPos, boxSize, boxSize, opened ? UI_DOWN : UI_RIGHT);

    // 标题
    draw_title(
        cmdlist,
        rect.x + rect.h, rect.y, rect.w - rect.h, rect.h,
        widget->text,
        UI_LEFT,
        textColor
    );
}

} // namespace minimgui
