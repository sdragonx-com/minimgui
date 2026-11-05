#ifndef UNICODE
    #define UNICODE
    #define _UNICODE
#endif

#include "pch.h"

// #include <codecvt>
#include <windows.h>

#include <minimgui/ui.h>
#include <minimgui/ui_demo.h>
#include <minimgui/backends/ui_impl_glfw.h>
#include <minimgui/backends/ui_impl_gl2.h>

// utf8 to unicode
inline std::wstring utf8_to_unicode(const char* str, size_t length)
{
    size_t size = (size_t) MultiByteToWideChar(CP_UTF8, 0, str, (int) length, NULL, 0);
    std::vector<wchar_t> buf(size);
    MultiByteToWideChar(CP_UTF8, 0, str, (int) length, &buf[0], (int) size);
    return std::wstring(buf.begin(), buf.end());
}

// 全局变量

// 摄像机
vec3f viewPos    = vec3f(0.0f, 0.0f, 10.0f);
vec3f viewTarget = vec3f(0.0f, 0.0f, 0.0f);
vec3f viewUp     = vec3f(0.0f, 1.0f, 0.0f);

// imgui 变量
bool show_demo_window    = false;
bool show_another_window = false;
vec4f clear_color        = vec4f(0.45f, 0.55f, 0.60f, 1.00f);

GLuint g_image = 0;    // 纹理
bool useTex    = true; // 是否使用纹理

const wchar_t* TEXT_CN  = L"1234abcd中文字符";
const wchar_t* TEXT_ABC = L"0123456789abcdefghijklmnABCDEFGHIJKLMN";

// char buf[1024]    = u8"1234abcd中文字符";
// std::wstring text = utf8_to_unicode(buf, strlen(buf));

// 函数
GLFWwindow* initgl(const std::string& title, int width, int height, bool resizable);
void initgui(GLFWwindow* window);
void reshape_event(GLFWwindow* window, int width, int height);
void keyboard_event(GLFWwindow* window, int key, int scancode, int action, int modifier);
void mouse_event(GLFWwindow* window, int button, int action, int mods);
void motion_event(GLFWwindow* window, double x, double y);
void scroll_event(GLFWwindow* window, double x, double y);
// void update(double t);
void display(GLFWwindow* window);
void draw_imgui(GLFWwindow* window);

// main
int main(int argc, char* argv[])
// int _tmain(int argc, TCHAR* argv[])
{
    // init glfw, glew
    GLFWwindow* window = initgl("OpenGL Sample", screen.x, screen.y, true);
    if (window == NULL) {
        return -1;
    }

    // init imgui
    minimgui::init_gui();
    minimgui::init_glfw(window, true);
    minimgui::init_gl2(nullptr);

    // 加载纹理
    g_image = minimgui::load_image("background.jpg");

    // 创建字体

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // glfw 事件处理
        glfwPollEvents();

        // update
        // update(delay);

        // display
        display(window);

        // swap buffers
        glfwSwapBuffers(window);
    }


    // 关闭 glfw 窗口，退出 glfw。
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// init GLFW window
GLFWwindow* initgl(const std::string& title, int width, int height, bool resizable)
{
    // init glfw
    if (!glfwInit()) {
        return nullptr;
    }

    // 初始化GL配置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // multisampling
    glfwWindowHint(GLFW_SAMPLES, 4);

    // window resizable
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

    // create window
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    // std::vector<float> vt;
    // glm::value_ptr(vt);

    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    // make current
    glfwMakeContextCurrent(window);

    // vertical sync
    glfwSwapInterval(vsync ? 1 : 0);

    // init glad
    if (0 == gladLoaderLoadGL()) {
        printf("glad> OpenGL extensions loading failed.\n");
    }

    printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    printf("GL_VERSION: %s\n", glGetString(GL_VERSION));

    // init opengl state
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    // 开启 2D 纹理
    glEnable(GL_TEXTURE_2D);

    // 开启混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 开启深度
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);

    // events callback
    glfwSetFramebufferSizeCallback(window, reshape_event);
    glfwSetKeyCallback(window, keyboard_event);
    glfwSetMouseButtonCallback(window, mouse_event);
    glfwSetCursorPosCallback(window, motion_event);
    glfwSetScrollCallback(window, scroll_event);

    return window;
}

// 窗口大小改变
void reshape_event(GLFWwindow* window, int width, int height)
{
    // Window size cannot be 0
    if (width == 0 || height == 0) {
        return;
    }
    screen = vec2i(width, height);
    glViewport(0, 0, width, height);
}

// 键盘消息
void keyboard_event(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_ESCAPE: // press esc to exit
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;

    case GLFW_KEY_F1:
        break;
    case GLFW_KEY_F2:
        break;
    case GLFW_KEY_F3:
        break;
    case GLFW_KEY_F4:
        break;
        // ...

    case GLFW_KEY_1:
        break;
    case GLFW_KEY_2:
        break;
    case GLFW_KEY_3:
        break;
    case GLFW_KEY_4:
        break;
        // ...

    default:
        break;
    }
}

// 鼠标按键消息
void mouse_event(GLFWwindow* window, int button, int action, int mods)
{
    // if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows | ImGuiHoveredFlags_AnyWindow)) {


    if (action == GLFW_PRESS) {
        mouse_button = button;
        lastpos      = mouse;
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            // printf("mouse left button down\n");
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            break;
        default:
            return;
        }
    }
    else if (action == GLFW_RELEASE) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            // printf("mouse left button up\n");
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            break;
        default:
            return;
        }
        mouse_button = -1;
    }
}

// 轨迹球
void trackball(float x, float y)
{
    vec3f dir  = viewPos - viewTarget;
    float n    = length(dir);
    dir        = normalize(dir);
    vec3f side = cross(dir, viewUp);

    dir = dir * cosf(y) + side * sinf(y);   // axis y
    dir = dir * cosf(x) + viewUp * sinf(x); // axis x
    dir = normalize(dir);

    viewUp = cross(side, dir);

    viewPos = viewTarget + dir * n;
}

// 鼠标移动消息
void motion_event(GLFWwindow* window, double x, double y)
{
    // printf("mouse move : %d, %d\n", int(x), int(y));
    mouse = vec2f(x, y);

    // if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows | ImGuiHoveredFlags_AnyWindow)) {


    if (mouse_button == GLFW_MOUSE_BUTTON_LEFT) {
        vec2f offset = (mouse - lastpos) * 0.005f;
        trackball(offset.y, offset.x);
        lastpos = mouse;
    }
}

// 滚轮滚动
void scroll_event(GLFWwindow* window, double x, double y)
{
    // if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows | ImGuiHoveredFlags_AnyWindow)) {


    vec3f dir = viewPos - viewTarget;
    float n   = length(dir);
    dir       = normalize(dir);
    if (0.0f < y) {
        n *= 0.9f;
        if (n < 1.0f) {
            n = 1.0f;
        }
    }
    else {
        n *= 1.1f;
    }
    viewPos = viewTarget + dir * n;
}

// 显示
void display(GLFWwindow* window)
{
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect       = float(screen.x) / float(screen.y);
    static float angle = 0.0f;

    glm::mat4 proj  = glm::perspective(glm::radians(60.0f), aspect, 1.0f, 1000.0f);
    glm::mat4 view  = glm::lookAt(viewPos, viewTarget, viewUp);
    glm::mat4 model = glm::identity<glm::mat4>();
    model           = glm::rotate(model, glm::radians(angle), vec3f(0.0f, 1.0f, 0.0f));
    angle += 0.01f;

    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr((proj)));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr((view * model)));

    // 绘制立方体
    if (useTex) {
        glBindTexture(GL_TEXTURE_2D, g_image);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    draw_cube(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

    // 2D 绘制
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, screen.x, screen.y, 0.0, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);



    // 绘制 imgui 界面
    draw_imgui(window);
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
            glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
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
        image("image", g_image, imageSize, size, align);

        end_window();
    }
}

float g_fontOffset = 0.5f;
bool g_fontReset   = false;

// 显示 imgui demo 界面
void draw_imgui(GLFWwindow* window)
{
    using namespace minimgui;

    begin_context(window);

    static bool bFill       = true;
    static float fSize      = 100.0f;
    static float fThickness = 10.0f;

    // show_menu();

    //*
    if (begin_window("design", nullptr, uiVec2(814.0f, 412.0f), uiVec2(400.0f, 300.0f))) {
        show_menu();

        checkbox("fill", &bFill);
        slider("size", &fSize, 0.0f, 500.0f);
        slider("thickness", &fThickness, 0.0f, 100.0f);
        if (slider("font", &g_fontOffset, 0.0f, 10.0f)) {
            g_fontReset = true;
        }

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

    // foreground_cmdlist()->draw_rect(400, 420, fSize, fSize);
    // draw_close_icon(foreground_cmdlist(), 400, 420, fSize, fThickness);
    // draw_collapse_icon(foreground_cmdlist(), 400, 420, fSize, fThickness, bChecked);

    glPolygonMode(GL_FRONT, bFill ? GL_FILL : GL_LINE);

    // 显示演示窗口
    try {
        show_demo();
    }
    catch (const std::exception& e) {
        // 打印异常类型和内容
        printf("Catch exception: %s\n", e.what());

        // 如果你在 Windows 下用 Qt，用 qDebug() 甚至能直接弹窗或者输出到控制台
        // qDebug() << "Exception:" << e.what();

        __debugbreak(); // 直接触发断点，这时候看调用栈！
    }
    catch (...) {
        printf("Unknown exception!\n");
        __debugbreak();
    }

    end_context(window);
    render(window);
}
