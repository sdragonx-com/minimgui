#include "ui_demo.h"
#include "ui_widgets.h"
#include "ui_user.h"

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

    if (begin_window("Debug", isOpen, uiVec2(4.0f, 4.0f), uiVec2(300.0f, 720.0f))) {
        /*
        if (begin_menu_bar("debug.mainmenu")) {


            end_menu_bar();
        }
        */

        if (collapsible_panel("Infomations", UI_FLAGS_OPEN)) {
            uiString str;

            label("屏幕: %0.2f, %0.2f", ctx->screen.x, ctx->screen.y);
            label("鼠标: %0.2f, %0.2f %s", ctx->mouse.x, ctx->mouse.y, ctx->clicked ? "B" : "");
            label("移动: %0.2f, %0.2f", ctx->delta.x, ctx->delta.y);

            // modifier
            str.clear();
            if (key_state(UI_KEY_CONTROL)) {
                str += "ctrl ";
            }
            if (key_state(UI_KEY_SHIFT)) {
                str += "shift ";
            }
            if (key_state(UI_KEY_ALT)) {
                str += "alt";
            }
            label("修饰键: %s", str.c_str());

            label("悬停窗口: %llu", ctx->hoveredWindow);
            label("激活窗口: %llu", ctx->activedWindow);
            label("焦点窗口: %llu", ctx->focusWindow);

            label("悬停部件: %llu", ctx->hoveredWidget);
            // label("lasthot: %llu", ctx->lastHoveredWidget);
            label("点击部件: %llu", ctx->clickedWidget);
        }

        if (collapsible_panel("Debug Flags", UI_FLAGS_OPEN)) {
            checkbox("display widget bounds", &context()->debugFlags, UI_DEBUG_SHOW_WIDGET_BOUNDS);
        }

        if (collapsible_panel("Windows", UI_FLAGS_OPEN)) {
            if (!ctx->windows.empty()) {
                for (size_t i = ctx->windows.size() - 1; i < ctx->windows.size(); --i) {
                    uiWindow* w = ctx->windows[i];
                    // if (w->text == "Debug")) { continue; }
                    bool disabled = check_flags(w->flags, UI_FLAGS_DISABLED);
                    if (treenode(w->text.c_str(), disabled ? colors::gray : colors::black)) { // UI_FLAGS_OPEN
                        label("%s - [%llu]\n", w->text.c_str(), w->id);
                        label("rect: [%0.2f, %0.2f, %0.2f, %0.2f]", w->bounds.x, w->bounds.y, w->bounds.w, w->bounds.h);
                        label("scroll: %0.2f/%0.2f, %0.2f/%0.2f\n", w->scroll.x, w->scrollMax.x, w->scroll.y, w->scrollMax.y);

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

    if (begin_window("Settings", isOpen, uiVec2(320.0f, 412.0f))) {
        separator("Title");

        label(width, "background:"); sameline(); drag_color(&style()->titleBackground[0].x);
        label(width, "");            sameline(); drag_color(&style()->titleBackground[2].x);

        label(width, "foreground:"); sameline(); drag_color(&style()->titleForeground[0].x);
        label(width, "");            sameline(); drag_color(&style()->titleForeground[2].x);

        separator("Window");

        label(width, "background:"); sameline(); drag_color(&style()->windowBackground.x);
        label(width, "foreground:"); sameline(); drag_color(&style()->windowForeground[0].x);
        label(width, "");            sameline(); drag_color(&style()->windowForeground[2].x);

        separator("Widget");

        label(width, "background:"); sameline(); drag_color(&style()->widgetBackground[0].x);
        label(width, "");            sameline(); drag_color(&style()->widgetBackground[1].x);
        label(width, "");            sameline(); drag_color(&style()->widgetBackground[2].x);
        label(width, "foreground:"); sameline(); drag_color(&style()->widgetForeground[0].x);
        label(width, "");            sameline(); drag_color(&style()->widgetForeground[1].x);
        label(width, "");            sameline(); drag_color(&style()->widgetForeground[2].x);

        /*
        separator("Button");

        label(width, "background:"); sameline(); drag_color(&style()->buttonBackground[0].x);
        label(width, "");            sameline(); drag_color(&style()->buttonBackground[1].x);
        label(width, "");            sameline(); drag_color(&style()->buttonBackground[2].x);
        */

        separator("Thumb");

        label(width, "background:"); sameline(); drag_color(&style()->trackBackground.x);
        label(width, "foreground:"); sameline(); drag_color(&style()->thumbForeground[0].x);
        label(width, "");            sameline(); drag_color(&style()->thumbForeground[1].x);
        label(width, "");            sameline(); drag_color(&style()->thumbForeground[2].x);

        separator("Text");

        label(width, "background:"); sameline(); drag_color(&style()->background[0].x);
        label(width, "");            sameline(); drag_color(&style()->background[1].x);
        label(width, "");            sameline(); drag_color(&style()->background[2].x);
        label(width, "foreground:"); sameline(); drag_color(&style()->foreground[0].x);
        label(width, "");            sameline(); drag_color(&style()->foreground[1].x);
        label(width, "");            sameline(); drag_color(&style()->foreground[2].x);

        end_window();
    }

    // clang-format on
}

static void show_basic_widget(char* str, size_t length)
{
    separator("text separator");
    separator();
    separator("edit");
    edit(str, length);
}

// 显示演示窗口
void show_demo()
{
    static bool bChecked     = true;
    static float fSlider     = 10.0f;
    static int iDrag         = 10;
    static float fDrag       = 10.0f;
    static char u8buf[256]   = u8"abcijklmnWM中文测试abcdefghijklmnopqrstuvwxyz";
    // static wchar_t wbuf[256] = L"abcijklmnWM中文测试abcdefghijklmnopqrstuvwxyz";

    if (begin_window("DEMO", nullptr, uiVec2(320.0f, 4.0f), uiVec2(300.0f, 400.0f))) {
        if (collapsible_panel("Settings", UI_FLAGS_OPEN)) {
            checkbox("Debug", &_m_showDebug);
            checkbox("Settings", &_m_showSettings);
            label("message: %s", _m_msg.c_str());
        }

        if (collapsible_panel("Widgets", UI_FLAGS_OPEN)) {
            checkbox("check", &bChecked);
            slider("slider", &fSlider, 0.0f, 100.0f);
            drag_int(-1.0f, &iDrag, -100, 100);
            drag_float(-1.0f, &fDrag, -100.0f, 100.0f);
            separator("buttons");
            for (int i = 0; i < 10; ++i) {
                label(80.0f, "label %d:", i);
                sameline();
                if (button(format("button %d", i), uiVec2(0.0f))) {
                    _m_msg = format("click %d\n", i);
                }
            }
        }

        end_window();
    }

    if (begin_window("Window", nullptr, uiVec2(624.0f, 4.0f), uiVec2(300.0f, 400.0f))) {
        checkbox("check", &bChecked);
        slider("slider", &fSlider, 0.0f, 100.0f);
        separator("text separator");
        separator();

        label("Edit: (utf-8)");
        edit(u8buf, countof(u8buf));
        edit(u8buf, countof(u8buf));

        // label("Edit: (wchar_t)");
        // edit(wbuf, countof(wbuf));
        // edit(wbuf, countof(wbuf));

        separator("tree");
        if (treenode("tree", UI_FLAGS_OPEN)) {
            label("label");
            label("label");
            checkbox("check", &bChecked);
            slider("slider", &fSlider, 0.0f, 100.0f);
            if (treenode("tree3")) {
                if (treenode("tree4")) {
                    button("buttonX");
                    edit(u8buf, countof(u8buf));
                    // edit(wbuf, countof(wbuf));
                    label("node3");
                    label("node4");
                    tree_pop();
                }
                label("node3");
                label("node4");
                tree_pop();
            }
            tree_pop();
        }

        end_window();
    }

    if (begin_window("Items", nullptr, uiVec2(928.0f, 4.0f), uiVec2(300.0f, 400.0f))) {
        checkbox("check", &bChecked);
        slider("slider", &fSlider, 0.0f, 100.0f);

        separator("scroll");
        static float fScroll    = 10.0f;
        static float fScrollMax = 20.0f;
        hscroll("scroll", &fScroll, 0.0f, fScrollMax);
        slider("scroll Max", &fScrollMax, 10.0f, 400.0f);

        separator("list box");
        // test listbox
        if (begin_list("listbox")) {
            static int id = -1;
            for (int i = 0; i < 100; ++i) {
                if (selectable(format("select %d", i), id == i, uiVec2(-1.0f, 0.0f))) {
                    id = i;
                }
            }
            end_list();
            label("select: %d", id);
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
