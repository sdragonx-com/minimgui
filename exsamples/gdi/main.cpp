
#include "window.h"
#include <gdiplus.h>

#include "minimgui/ui.h"
#include "minimgui/ui_utils.h"
#include "minimgui/ui_widgets.h"
#include "minimgui/ui_demo.h"
#include "minimgui/ui_gfx.h"

#if defined(UI_BACKEND_GDIPLUS)
#include "minimgui/backends/ui_impl_gdiplus.h"
#else
#include "minimgui/backends/ui_impl_gdi.h"
#endif

#include "minimgui/backends/ui_impl_win32.h"

glm::ivec2 mouse;
minimgui::uiButton mousebutton = minimgui::UI_BUTTON_NONE;

#if defined(UI_BACKEND_GDIPLUS)
// 初始化 gdiplus
ULONG_PTR g_token;
Gdiplus::GdiplusStartupInput g_input;
#endif

minimgui::uiHandle g_image = 0;

// int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
int main(int argc, char* argv[])
{
    HWND hwnd = init_window(_T("Win32 程序"), 1280, 800, true);
    HDC hdc   = GetDC(hwnd);

    #if defined(UI_BACKEND_GDIPLUS)
    // 初始化Gdiplus环境
    Gdiplus::GdiplusStartup(&g_token, &g_input, NULL);

    #endif

    minimgui::init_gui();
    minimgui::init_win32(hwnd, hdc);

    g_image = minimgui::load_image("lena_std.png");
    printf("load image: %lld\n", g_image);

    while (running()) {
        do_events();
    }

    ReleaseDC(hwnd, hdc);

    #if defined(UI_BACKEND_GDIPLUS)
    // 释放Gdiplus环境
    Gdiplus::GdiplusShutdown(g_token);
    #endif

    return 0;
}


// 窗口缩放事件
void reshape(int width, int height)
{
    if (minimgui::on_resize(width, height)) {
        return;
    }
}

// 按键按下
void key_push(int key)
{
    if (minimgui::on_key_push(minimgui::key_map(key))) {
        return;
    }
}

// 按键弹起
void key_pop(int key)
{
    if (minimgui::on_key_pop(minimgui::key_map(key))) {
        return;
    }
}

// 字符输入
void input(int ch)
{
    if (minimgui::on_input(ch)) {
        return;
    }
}

// 鼠标按下
void mouse_push(int x, int y, int button)
{
    switch (button) {
    case VK_LBUTTON:
        mousebutton = minimgui::UI_BUTTON_LEFT;
        break;
    case VK_RBUTTON:
        mousebutton = minimgui::UI_BUTTON_RIGHT;
        break;
    case VK_MBUTTON:
        mousebutton = minimgui::UI_BUTTON_MIDDLE;
        break;
    default:
        mousebutton = minimgui::UI_BUTTON_NONE;
        break;
    }
    if (minimgui::on_mouse_push(x, y, mousebutton)) {
        return;
    }
}

// 鼠标弹起
void mouse_pop(int x, int y, int button)
{
    switch (button) {
    case VK_LBUTTON:
        mousebutton = minimgui::UI_BUTTON_LEFT;
        break;
    case VK_RBUTTON:
        mousebutton = minimgui::UI_BUTTON_RIGHT;
        break;
    case VK_MBUTTON:
        mousebutton = minimgui::UI_BUTTON_MIDDLE;
        break;
    default:
        mousebutton = minimgui::UI_BUTTON_NONE;
        break;
    }
    if (minimgui::on_mouse_pop(x, y, mousebutton)) {
        return;
    }
    mousebutton = minimgui::UI_BUTTON_NONE;
}

// 鼠标移动
void mouse_move(int x, int y)
{
    if (minimgui::on_mouse_move(x, y, mousebutton)) {
        return;
    }
}

// 滚轮滚动
void scroll(int x, int y, int z)
{
    minimgui::on_scroll(z);
}

// 计时器更新事件
void update(float delay)
{
}

void show_sample_menus()
{
    using namespace minimgui;

    if (begin_menu(_T("File"))) {
        if (begin_menu(_T("New"))) {
            menu_item(_T("Text"));
            menu_item(_T("Jpg"));
            menu_item(_T("Png"));
            end_menu();
        }
        menu_item(_T("Open"));
        menu_item(_T("Close"));
        menu_item(_T("-"));
        if (begin_menu(_T("Recent"))) {
            menu_item(_T("1.txt"));
            menu_item(_T("2.txt"));
            menu_item(_T("3.txt"));
            end_menu();
        }
        menu_item(_T("-"));
        if (menu_item(_T("Exit"))) {
            SendMessage(window(), WM_CLOSE, 0, 0);
        }

        end_menu();
    }

    if (begin_menu(_T("Edit"))) {
        menu_item(_T("Undo"));
        menu_item(_T("Redo"));
        menu_item(_T("-"));
        menu_item(_T("Cut"));
        menu_item(_T("Copy"));
        menu_item(_T("Paste"));
        menu_item(_T("Delete"));
        menu_item(_T("-"));
        end_menu();
    }
    /*
    if (begin_menu(_T("Widget"))) {
        static bool bValue  = true;
        static float fValue = 0.0f;
        static uiColor color = colors::white;
        button(_T("button"));
        checkbox(_T("check"), &bValue);
        slider(_T("slider menu"), &fValue, 0.0f, 100.0f);
        label(_T("Color"));
        // sameline();
        drag_color(color);
        end_menu();
    }
    */
    if (begin_menu(_T("View"))) {
        show_sample_menus(); // 这里 help 还没计算
        end_menu();
    }
    if (begin_menu(_T("Help"))) {
        menu_item(_T("About"));
        end_menu();
    }
}

void show_menu()
{
    using namespace minimgui;

    if (begin_menu_bar(_T("MainMenu"))) {
        show_sample_menus();

        end_menu_bar();
    }
}

// 绘图事件
void display(HDC hdc)
{
    using namespace minimgui;

    // clear(0xFF8000);
    clear(0x404040);

    begin_context(hdc);

    // if (custom_test(L"test", 400.0f, 500.0f, 100.0f, 40.0f)) {printf("click custom\n"); }

    static bool bChecked = true;
    static float fSlider = 10.0f;
    static int iDrag     = 10;
    static float fDrag   = 10.0f;

    static float fSize      = 100.0f;
    static float fThickness = 10.0f;

    // show_menu();

    //*
    if (begin_window(_T("design"), uiVec2(814.0f, 412.0f), uiVec2(400, 300.0f))) {
        show_menu();

        label(_T("container: %ls"), layout()->container->text.c_str());
        uiRect rect = context()->clipRect;
        label(_T("clip: %0.2f, %0.2f  %0.2f, %0.2f"), rect.x, rect.y, rect.w, rect.h);

        checkbox(_T("check"), &bChecked);
        slider(_T("slider"), &fSlider, 0.0f, 100.0f);
        drag_int(-1.0f, &iDrag, -100, 100);
        drag_float(-1.0f, &fDrag, -100.0f, 100.0f);

        slider(_T("size"), &fSize, 0.0f, 500.0f);
        slider(_T("thickness"), &fThickness, 0.0f, 100.0f);

        if (begin_combo(_T("combobox"))) {
            static int id = -1;
            for (int i = 0; i < 10; ++i) {
                if (selectable(format(_T("select %d"), i), id == i, uiVec2(-1.0f, 0.0f))) {
                    id = i;
                }
            }
            end_combo();
        }


        separator(_T("buttons"));
        if (collapsible_panel(_T("Buttons"), UI_FLAGS_OPEN)) {
            for (int i = 0; i < 10; ++i) {
                label(_T("label %d:"), i);
                sameline();
                if (button(format(_T("button %d"), i), uiVec2(0.0f))) {
                    printf("click %d\n", i);
                }
            }
        }

        end_window();
    }

    if (begin_window(_T("Image"))) {
        static uiVec2 imageSize = uiVec2(64.0f);
        static uiVec2 size      = uiVec2(120.0f, 90.0f);

        label(_T("image size:"));
        slider(_T("width"), &imageSize.x, 0.0f, 512.0f);
        slider(_T("height"), &imageSize.y, 0.0f, 512.0f);
        label(_T("size:"));
        slider(_T("width"), &size.x, 0.0f, 512.0f);
        slider(_T("height"), &size.y, 0.0f, 512.0f);

        static uiFlags align = UI_FLAGS_NONE;
        checkbox(_T("左"), &align, UI_LEFT);
        sameline();
        checkbox(_T("右"), &align, UI_RIGHT);
        sameline();
        checkbox(_T("上"), &align, UI_TOP);
        sameline();
        checkbox(_T("下"), &align, UI_BOTTOM);
        image(_T("image"), g_image, imageSize, size, align);

        end_window();
    }

    // 显示演示窗口
    show_demo();

    // foreground_cmdlist()->draw_rect(400, 400, fSize, fSize);
    // draw_up_icon(foreground_cmdlist(), 400, 400, fSize, fThickness);


    /*
    if (begin_window(_T("design"), uiVec2(814.0f, 412.0f), uiVec2(400, 300.0f))) {

        end_window();
    }

    show_debug(nullptr);
    */

    end_context(hdc);
    render(hdc);
}
