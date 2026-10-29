#include "ui_demo.h"
#include "ui_widgets.h"

namespace minimgui {

static bool _m_showDebug    = true;
static bool _m_showSettings = false;

static uiString _m_msg;

//------------------------------------------------------------------------------
// 调试
//------------------------------------------------------------------------------

// 显示调试窗口
void show_debug(bool* isOpen)
{
    const uiContext* ctx = context();

    if (begin_window(_T("Debug"), isOpen, uiVec2(4.0f, 4.0f), uiVec2(300.0f, 720.0f))) {
        if (collapsible_panel(_T("Infomations"), UI_FLAGS_OPEN)) {
            label(_T("屏幕: %0.2f, %0.2f"), ctx->screen.x, ctx->screen.y);
            label(_T("鼠标: %0.2f, %0.2f %s"), ctx->mouse.x, ctx->mouse.y, ctx->clicked ? _T("B") : _T(""));
            label(_T("移动: %0.2f, %0.2f"), ctx->delta.x, ctx->delta.y);

            label(_T("悬停窗口: %llu"), ctx->hoveredWindow);
            label(_T("激活窗口: %llu"), ctx->activedWindow);
            label(_T("焦点窗口: %llu"), ctx->focusWindow);

            label(_T("悬停部件: %llu"), ctx->hoveredWidget);
            // label(_T("lasthot: %llu"), ctx->lastHoveredWidget);
            label(_T("点击部件: %llu"), ctx->clickedWidget);
        }

        if (collapsible_panel(_T("Windows"), UI_FLAGS_OPEN)) {
            if (!ctx->windows.empty()) {
                for (size_t i = ctx->windows.size() - 1; i < ctx->windows.size(); --i) {
                    uiWindow* w = ctx->windows[i];
                    // if (w->text == _T("Debug")) { continue; }
                    bool disabled = check_flags(w->flags, UI_FLAGS_DISABLED);
                    if (treenode(w->text.c_str(), disabled ? colors::gray : colors::black)) { // UI_FLAGS_OPEN
                        label(_T("%s - [%llu]\n"), w->text.c_str(), w->id);
                        label(_T("rect: [%0.2f, %0.2f, %0.2f, %0.2f]"), w->bounds.x, w->bounds.y, w->bounds.w, w->bounds.h);
                        label(_T("scroll: %0.2f/%0.2f, %0.2f/%0.2f\n"), w->scroll.x, w->scrollMax.x, w->scroll.y, w->scrollMax.y);

                        tree_pop();
                    }
                }
            }
        }

        end_window();
    }
}

// 显示设置窗口
void show_settings(bool* isOpen)
{
    const float width = 100.0f;

    // clang-format off

    if (begin_window(_T("Settings"), isOpen, uiVec2(320.0f, 412.0f))) {
        separator(_T("Title"));

        label(width, _T("background:")); sameline(); drag_color(&style()->titleBackground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->titleBackground[2].x);

        label(width, _T("foreground:")); sameline(); drag_color(&style()->titleForeground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->titleForeground[2].x);

        separator(_T("Window"));

        label(width, _T("background:")); sameline(); drag_color(&style()->windowBackground.x);
        label(width, _T("foreground:")); sameline(); drag_color(&style()->windowForeground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->windowForeground[2].x);

        separator(_T("Widget"));

        label(width, _T("background:")); sameline(); drag_color(&style()->widgetBackground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->widgetBackground[1].x);
        label(width, _T(""));            sameline(); drag_color(&style()->widgetBackground[2].x);
        label(width, _T("foreground:")); sameline(); drag_color(&style()->widgetForeground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->widgetForeground[1].x);
        label(width, _T(""));            sameline(); drag_color(&style()->widgetForeground[2].x);

        /*
        separator(_T("Button"));

        label(width, _T("background:")); sameline(); drag_color(&style()->buttonBackground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->buttonBackground[1].x);
        label(width, _T(""));            sameline(); drag_color(&style()->buttonBackground[2].x);
        */

        separator(_T("Thumb"));

        label(width, _T("background:")); sameline(); drag_color(&style()->trackBackground.x);
        label(width, _T("foreground:")); sameline(); drag_color(&style()->thumbForeground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->thumbForeground[1].x);
        label(width, _T(""));            sameline(); drag_color(&style()->thumbForeground[2].x);

        separator(_T("Text"));

        label(width, _T("background:")); sameline(); drag_color(&style()->background[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->background[1].x);
        label(width, _T(""));            sameline(); drag_color(&style()->background[2].x);
        label(width, _T("foreground:")); sameline(); drag_color(&style()->foreground[0].x);
        label(width, _T(""));            sameline(); drag_color(&style()->foreground[1].x);
        label(width, _T(""));            sameline(); drag_color(&style()->foreground[2].x);

        end_window();
    }

    // clang-format on
}

static void show_basic_widget(uiChar* str, size_t length)
{
    separator(_T("text separator"));
    separator();
    separator(_T("edit"));
    edit(str, length);
}

// 显示演示窗口
void show_demo()
{
    static bool bChecked   = true;
    static float fSlider   = 10.0f;
    static int iDrag       = 10;
    static float fDrag     = 10.0f;
    static uiChar str[256] = L"abcijklmnWM中文测试";

    if (begin_window(_T("DEMO"), uiVec2(320.0f, 4.0f), uiVec2(300, 400.0f))) {
        if (collapsible_panel(_T("Settings"), UI_FLAGS_OPEN)) {
            checkbox(_T("Debug"), &_m_showDebug);
            checkbox(_T("Settings"), &_m_showSettings);
            label(_T("message: %s"), _m_msg.c_str());
        }

        if (collapsible_panel(_T("Widgets"), UI_FLAGS_OPEN)) {
            checkbox(_T("check"), &bChecked);
            slider(_T("slider"), &fSlider, 0.0f, 100.0f);
            drag_int(-1.0f, &iDrag, -100, 100);
            drag_float(-1.0f, &fDrag, -100.0f, 100.0f);
            separator(_T("buttons"));
            for (int i = 0; i < 10; ++i) {
                label(80.0f, _T("label %d:"), i);
                sameline();
                if (button(format(_T("button %d"), i), uiVec2(0.0f))) {
                    _m_msg = format(_T("click %d\n"), i);
                }
            }
        }

        end_window();
    }

    if (begin_window(_T("Window"), nullptr, uiVec2(624.0f, 4.0f), uiVec2(300, 400.0f))) {
        checkbox(_T("check"), &bChecked);
        slider(_T("slider"), &fSlider, 0.0f, 100.0f);
        separator(_T("text separator"));
        separator();
        separator(_T("edit"));
        edit(str, _countof(str));
        separator(_T("tree"));
        if (treenode(_T("tree"), UI_FLAGS_OPEN)) {
            label(_T("label"));
            label(_T("label"));
            checkbox(_T("check"), &bChecked);
            slider(_T("slider"), &fSlider, 0.0f, 100.0f);
            if (treenode(_T("tree3"))) {
                if (treenode(_T("tree4"))) {
                    button(_T("buttonX"));
                    edit(str, _countof(str));
                    label(_T("node3"));
                    label(_T("node4"));
                    tree_pop();
                }
                label(_T("node3"));
                label(_T("node4"));
                tree_pop();
            }
            tree_pop();
        }

        end_window();
    }

    if (begin_window(_T("Items"), nullptr, uiVec2(928.0f, 4.0f), uiVec2(300, 400.0f))) {
        checkbox(_T("check"), &bChecked);
        slider(_T("slider"), &fSlider, 0.0f, 100.0f);

        separator(_T("scroll"));
        static float fScroll    = 10.0f;
        static float fScrollMax = 20.0f;
        hscroll(_T("scroll"), &fScroll, 0.0f, fScrollMax);
        slider(_T("scroll Max"), &fScrollMax, 10.0f, 400.0f);

        separator(_T("list box"));
        // test listbox
        if (begin_list(_T("listbox"))) {
            static int id = -1;
            for (int i = 0; i < 10; ++i) {
                if (selectable(format(_T("select %d"), i), id == i, uiVec2(-1.0f, 0.0f))) {
                    id = i;
                }
            }
            end_list();
            label(_T("select: %d"), id);
        }

        end_window();
    }

    if (_m_showDebug) {
        show_debug(&_m_showDebug);
    }

    if (_m_showSettings) {
        show_settings(&_m_showSettings);
    }
}

} // namespace minimgui
