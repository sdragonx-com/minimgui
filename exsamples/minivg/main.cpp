#include "pch.h"

#include <minimgui/ui.h>
#include <minimgui/ui_widgets.h>
#include <minimgui/ui_demo.h>
#include <minimgui/ui_gfx.h>

#include <minimgui/backends/ui_impl_minivg.h>
#include <minimgui/backends/ui_impl_win32.h>


vec2i screen = vec2i(1280, 720);
vec2f mouse;
vec2f lastpos;
bool buttonDown = false;

std::vector<vec2f> points; // 控制点
int pointID = -1;          // 当前控制点 ID

int select_point(const vec2f& pos)
{
    float d;
    for (size_t i = 0; i < points.size(); ++i) {
        d = distance(pos, points[i]);
        if (d < 4.0f) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

const wchar_t* level = L"速度";

void draw_ui();

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    initgraph("MiniGui", screen.x, screen.y, VG_SIZEABLE);
    set_title("minivg 图形库");

    key_push_event(key_push);
    key_pop_event(key_pop);
    input_event(input);
    mouse_push_event(mouse_push);
    mouse_pop_event(mouse_pop);
    mouse_move_event(mouse_move);
    scroll_event(scroll);
    display_event(display);

    // 2. 设置进程DPI感知（仅启动时调用一次，关键：避免鼠标缩放异常）
    // 优先使用高版本DPI感知（Win10 1703+）
    /*
    if (SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        // 成功设置每监视器DPI感知
    }
    else {
        // 兼容低版本系统
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    }
    */

    // effect_level(VG_SPEED);
    effect_level(VG_MEDIUM);
    // effect_level(VG_QUALITY);

    // wchar_t* str = (wchar_t*)"\x88\x6D\x6F\x60";
    minimgui::init_gui();
    minimgui::init_win32(graph_window(), true);

    return start_app();
}
//------------------------------------------------------------------------------

// 按键按下
void key_push(int key)
{
    if (minimgui::on_key_push(key)) {
        return;
    }

    switch (key) {
    case VK_F1:
        effect_level(VG_SPEED);
        level = L"速度";
        break;
    case VK_F2:
        effect_level(VG_MEDIUM);
        level = L"中等";
        break;
    case VK_F3:
        effect_level(VG_QUALITY);
        level = L"高质量";
        break;
    }
}

// 按键弹起
void key_pop(int key)
{
    if (minimgui::on_key_pop(key)) {
        return;
    }
}

// 字符输入
void input(int key)
{
    if (minimgui::on_input(key)) {
        return;
    }
}

// 鼠标按下
void mouse_push(int x, int y, int button)
{
    lastpos    = mouse;
    buttonDown = true;

    if (button == VG_LEFT) {
        pointID = select_point(mouse);
        if (pointID != -1) {
        }
        else {
            points.push_back(vec2f(x, y));
        }
    }
    else {
        points.clear();
    }
}

// 鼠标弹起
void mouse_pop(int x, int y, int button)
{
    buttonDown = false;
}

// 鼠标移动
void mouse_move(int x, int y, int button)
{
    mouse = vec2f(x, y);

    if (buttonDown) {
        if (pointID != -1) {
            vec2f offset = mouse - lastpos;

            points[pointID] += offset;

            lastpos = mouse;
        }
    }
}

// 滚轮滚动
void scroll(int x, int y, int z)
{
    (void) x;
    (void) y;
}

// 测试命令列表
void test_cmdlist()
{
    using namespace minimgui;

    uiCommandList cmdlist;

    cmdlist.set_stroke(uiColor(255, 255, 0, 255));
    cmdlist.fill_rect(100.0f, 100.0f, 200.0f, 200.0f);

    cmdlist.set_stroke(uiColor(0, 255, 0, 255));
    cmdlist.draw_rect(100.0f, 100.0f, 200.0f, 200.0f);

    cmdlist.set_stroke(uiColor(255, 0, 0, 255));
    cmdlist.draw_line(100.0f, 100.0f, 200.0f, 200.0f);

    cmdlist.set_stroke(uiColor(0, 0, 0, 255));
    cmdlist.draw_string(100.0f, 150.0f, "string test");

    // draw_c draw_cmd(cmdlist);
}

// 显示
void display()
{
    // 清屏
    // clear(196, 196, 196);
    drawimage(loadimage(L"background.jpg"), 0.0f, 0.0f, screen.x, screen.y);

    // test_draw();
    // test_cmdlist();

    pen_color(255, 0, 0);
    for (auto p : points) {
        // draw_point(p.x, p.y, 10);
    }

    draw_ui();

    text_color(255, 255, 255);
    print(0.0f, 0.0f, "mouse: %0.2f, %0.2f", mouse.x, mouse.y);

    show_fps();
}


void show_sample_menus()
{
    using namespace minimgui;

    if (begin_menu("File")) {
        if (begin_menu("New")) {
            menu_item("Text");
            menu_item("Jpg");
            menu_item("Png");
            end_menu();
        }
        menu_item("Open");
        menu_item("Close");
        menu_item("-");
        if (begin_menu("Recent")) {
            menu_item("1.txt");
            menu_item("2.txt");
            menu_item("3.txt");
            end_menu();
        }
        menu_item("-");
        if (menu_item("Exit")) {
            quit();
        }

        end_menu();
    }

    if (begin_menu("Edit")) {
        menu_item("Undo");
        menu_item("Redo");
        menu_item("-");
        menu_item("Cut");
        menu_item("Copy");
        menu_item("Paste");
        menu_item("Delete");
        menu_item("-");
        end_menu();
    }
    /*
    if (begin_menu("Widget")) {
        static bool bValue  = true;
        static float fValue = 0.0f;
        static uiColor color = colors::white;
        button("button");
        checkbox("check"), &bValue);
        slider("slider menu"), &fValue, 0.0f, 100.0f);
        label("Color");
        // sameline();
        drag_color(color);
        end_menu();
    }
    */
    if (begin_menu("View")) {
        show_sample_menus(); // 这里 help 还没计算
        end_menu();
    }
    if (begin_menu("Help")) {
        menu_item("About");
        end_menu();
    }
}

void show_menu()
{
    using namespace minimgui;

    if (begin_menu_bar("MainMenu")) {
        show_sample_menus();

        end_menu_bar();
    }
}

void show_image()
{
    using namespace minimgui;

    if (begin_window("Image")) {
        static uiVec2 imageSize = uiVec2(64.0f);
        static uiVec2 size      = uiVec2(120.0f, 90.0f);

        label("image size:");
        slider("width", &imageSize.x, 0.0f, 512.0f);
        slider("height", &imageSize.y, 0.0f, 512.0f);
        label("size:");
        slider("width", &size.x, 0.0f, 512.0f);
        slider("height", &size.y, 0.0f, 512.0f);

        static uiFlags align = UI_FLAGS_NONE;
        checkbox("左", &align, UI_LEFT);
        sameline();
        checkbox("右", &align, UI_RIGHT);
        sameline();
        checkbox("上", &align, UI_TOP);
        sameline();
        checkbox("下", &align, UI_BOTTOM);
        // image("image", g_image, imageSize, size, align);

        end_window();
    }
}

void draw_ui()
{
    using namespace minimgui;

    begin_context(nullptr);

    static float fSize      = 100.0f;
    static float fThickness = 10.0f;

    if (begin_window("design", nullptr, uiVec2(814.0f, 412.0f), uiVec2(400.0f, 300.0f))) {
        show_menu();

        slider("size", &fSize, 0.0f, 500.0f);
        slider("thickness", &fThickness, 0.0f, 100.0f);

        if (begin_combo("combobox")) {
            static int id = -1;
            for (int i = 0; i < 10; ++i) {
                if (selectable(format("select %d", i), id == i, uiVec2(-1.0f, 0.0f))) {
                    id = i;
                }
            }
            end_combo();
        }


        separator("buttons");
        if (collapsible_panel("Buttons", UI_FLAGS_OPEN)) {
            for (int i = 0; i < 10; ++i) {
                label("label %d:", i);
                sameline();
                if (button(format("button %d", i), uiVec2(0.0f))) {
                    printf("click %d\n", i);
                }
            }
        }

        end_window();
    }

    // 显示演示窗口
    show_demo();

    end_context(nullptr);
    render(nullptr);
}
