//---------------------------------------------------------------------------

#include "window.h"
#include <windowsx.h>

// 窗口类名
#define CLASSNAME TEXT("win32.window")

// 变量

static HWND hWindow        = NULL; // 主窗口
static UINT_PTR uTimerID   = 1;    // 计时器ID
static HDC hMemDC          = NULL; // 绘图设备
static HBITMAP hBackBuffer = NULL; // 背景缓冲图片
static HFONT hFont         = NULL; // 字体
static DWORD dwLastTime    = 0;    // 帧最后时间计时

// 主消息处理函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

// 初始化窗口
HWND init_window(LPCTSTR title, int width, int height, bool scalable)
{
    HWND hWnd;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIconW(hInstance, L"ICO_MAIN");
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = CLASSNAME;
    wc.hIconSm       = LoadIconW(hInstance, L"ICO_MAIN");

    if (!RegisterClassEx(&wc)) {
        return NULL;
    }

    DWORD style;
    if (scalable) {
        // 窗口可以缩放
        style = WS_OVERLAPPEDWINDOW;
    }
    else {
        // 固定大小窗口
        style = WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION;
    }

    RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRectEx(&rect, style, FALSE, WS_EX_CLIENTEDGE);
    width  = static_cast<int>(rect.right - rect.left);
    height = static_cast<int>(rect.bottom - rect.top);

    hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE, CLASSNAME, title, style,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL
    );

    if (hWnd) {
        ShowWindow(hWnd, SW_SHOW);
    }

    return hWnd;
}

// 初始化双缓冲
void initbuffer(HWND hWnd)
{
    // 获取窗口客户区大小
    RECT rect;
    GetClientRect(hWnd, &rect);

    // 创建内存 HDC
    if (!hMemDC) {
        hMemDC = CreateCompatibleDC(NULL);
    }

    // 创建背景缓冲区
    if (hBackBuffer) {
        DeleteObject(hBackBuffer);
    }
    hBackBuffer = CreateBitmap(rect.right - rect.left, rect.bottom - rect.top, 1, 32, NULL);
    SelectObject(hMemDC, hBackBuffer);

    // 创建字体
    if (!hFont) {
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, GB2312_CHARSET, 0, 0, 0, DEFAULT_PITCH, TEXT("msyh"));
    }

    // 选择字体
    SelectObject(hMemDC, hFont);

    // 字体透明
    SetBkMode(hMemDC, TRANSPARENT);
}

// 释放双缓冲
void freebuffer()
{
    // 删除背景缓冲区
    if (hBackBuffer) {
        DeleteObject(hBackBuffer);
        hBackBuffer = NULL;
    }

    // 删除字体
    if (hFont) {
        DeleteObject(hFont);
        hFont = NULL;
    }

    // 删除内存 HDC
    if (hMemDC) {
        DeleteDC(hMemDC);
        hMemDC = NULL;
    }
}

// 重绘窗口
void repaint(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    RedrawWindow(hwnd, &rect, 0, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
}

// 主消息处理函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message) {
    case WM_CREATE:
        hWindow = hWnd;
        initbuffer(hWnd);
        // 设置计时器
        uTimerID = SetTimer(hWnd, 0, 40, NULL);
        break;
    case WM_DESTROY:
        // 停止计时器
        KillTimer(hWnd, uTimerID);

        freebuffer();
        // 退出消息
        // PostQuitMessage(0);
        hWindow = NULL;
        break;

    case WM_ERASEBKGND: // 不擦除背景
        return TRUE;

    case WM_SIZE: // 窗口大小改变
        initbuffer(hWnd);
        reshape(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_PAINT: // 窗口绘制
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...

        RECT rect;

        // 获取窗口客户区域大小
        GetClientRect(hWnd, &rect);

        // 绘制游戏，用背景缓冲的 HDC
        display(hMemDC);

        // 背景缓冲绘制到窗口 hdc
        BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CLOSE:
        break;

    case WM_TIMER: // 计时器事件
    {
        DWORD t    = GetTickCount() - dwLastTime;
        dwLastTime = GetTickCount();
        update(t / 1000.0f);

        repaint(hWnd);
        break;
    }
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            PostQuitMessage(0);
        key_push(int(wParam));
        break;
    case WM_KEYUP:
        key_pop(int(wParam));
        break;
    case WM_CHAR:
        input(int(wParam));
        break;

    case WM_MOUSEMOVE:
        mouse_move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONDOWN:
        mouse_push(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), VK_LBUTTON);
        break;
    case WM_LBUTTONUP:
        mouse_pop(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), VK_LBUTTON);
        break;
    case WM_RBUTTONDOWN:
        mouse_push(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), VK_RBUTTON);
        break;
    case WM_RBUTTONUP:
        mouse_pop(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), VK_RBUTTON);
        break;
    case WM_MOUSEWHEEL:
        scroll(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_Y_LPARAM(wParam) / WHEEL_DELTA);
        break;

    default:
        break;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

// 返回窗口句柄
HWND window()
{
    return hWindow;
}

// 判断程序是否运行
bool running()
{
    return hWindow != NULL;
}

// 执行消息循环
void do_events()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// 运行程序
int run_app()
{
    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, NULL, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

// 清屏
void clear(COLORREF color)
{
    RECT rect;

    // 获取窗口客户区大小
    GetClientRect(hWindow, &rect);

    // 创建背景画刷
    HBRUSH brush = CreateSolidBrush(color);

    // 填充背景
    FillRect(hMemDC, &rect, brush);

    // 删除画刷
    DeleteObject(brush);
}

// 显示消息对话框
void msgbox(PCTSTR param, ...)
{
    TCHAR buf[1024] = { };
    va_list body;
    va_start(body, param);
    _vsntprintf(buf, 1024, param, body);
    va_end(body);

    MessageBox(hWindow, buf, TEXT("消息"), MB_OK);
}
