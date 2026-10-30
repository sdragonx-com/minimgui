#include "ui_impl_win32.h"
#include "../ui_widgets.h"

#include <windows.h>

namespace minimgui {

static HWND _m_window = nullptr;
static HDC _m_hdc     = nullptr;
static HFONT _m_hFont = nullptr;

int init_win32(HWND hwnd, HDC hdc)
{
    _m_window = hwnd;
    _m_hdc    = hdc;

    if (!_m_hFont) {
        _m_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, GB2312_CHARSET, 0, 0, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Microsoft YaHei"));
    }

    // 初始化屏幕大小
    RECT rect;
    GetWindowRect(hwnd, &rect);
    on_resize(rect.right - rect.left, rect.bottom - rect.top);

    return 0;
}

// clang-format off

// 按键映射
int key_map(int key)
{
    switch (key) {
    case VK_BACK:                 return UI_KEY_BACK;              // 8    '\b' backspace
    case VK_TAB:                  return UI_KEY_TAB;               // 9    '\t' tab
    case VK_RETURN:               return UI_KEY_ENTER;             // 13   '\r' enter
    case VK_SHIFT:                return UI_KEY_SHIFT;             // 16   shift
    case VK_CONTROL:              return UI_KEY_CONTROL;           // 17   ctrl
    case VK_MENU:                 return UI_KEY_ALT;               // 18   alt
    case VK_PAUSE:                return UI_KEY_PAUSE;             // 19   pause
    case VK_CAPITAL:              return UI_KEY_CAPS_LOCK;         // 20   capslock
    case VK_ESCAPE:               return UI_KEY_ESCAPE;            // 27   escape

    case VK_SPACE:                return UI_KEY_SPACE;             // 32   ' '

    case VK_PRIOR:                return UI_KEY_PAGE_UP;           // 33   page up
    case VK_NEXT:                 return UI_KEY_PAGE_DOWN;         // 34   page down
    case VK_END:                  return UI_KEY_END;               // 35   end
    case VK_HOME:                 return UI_KEY_HOME;              // 36   home

    case VK_LEFT:                 return UI_KEY_LEFT;              // 37   left
    case VK_UP:                   return UI_KEY_UP;                // 38   up
    case VK_RIGHT:                return UI_KEY_RIGHT;             // 39   right
    case VK_DOWN:                 return UI_KEY_DOWN;              // 40   down

    case VK_SNAPSHOT:             return UI_KEY_PRINT_SCREEN;      // 44   print screen
    case VK_INSERT:               return UI_KEY_INSERT;            // 45   insert
    case VK_DELETE:               return UI_KEY_DELETE;            // 46   delete

    // '0' ~ '9'
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return UI_KEY_0 + (key - '0');

    // 'A' ~ 'Z'
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        return UI_KEY_A + (key - 'A');

    case VK_LWIN:                 return UI_KEY_META;              // 91   left win
    case VK_RWIN:                 return UI_KEY_META;              // 92   right win
    case VK_APPS:                 return UI_KEY_MENU;              // 93   apps menu

    // 数字小键盘
    case VK_NUMPAD0:              return UI_KEY_KP_0;              // 96   KP 0
    case VK_NUMPAD1:              return UI_KEY_KP_1;              // 97   KP 1
    case VK_NUMPAD2:              return UI_KEY_KP_2;              // 98   KP 2
    case VK_NUMPAD3:              return UI_KEY_KP_3;              // 99   KP 3
    case VK_NUMPAD4:              return UI_KEY_KP_4;              // 100  KP 4
    case VK_NUMPAD5:              return UI_KEY_KP_5;              // 101  KP 5
    case VK_NUMPAD6:              return UI_KEY_KP_6;              // 102  KP 6
    case VK_NUMPAD7:              return UI_KEY_KP_7;              // 103  KP 7
    case VK_NUMPAD8:              return UI_KEY_KP_8;              // 104  KP 8
    case VK_NUMPAD9:              return UI_KEY_KP_9;              // 105  KP 9
    case VK_MULTIPLY:             return UI_KEY_KP_MULTIPLY;       // 106  KP *
    case VK_ADD:                  return UI_KEY_KP_ADD;            // 107  KP +
    case VK_SEPARATOR:            return UI_KEY_COMMA;             // 108  separator
    case VK_SUBTRACT:             return UI_KEY_KP_SUBTRACT;       // 109  KP -
    case VK_DECIMAL:              return UI_KEY_KP_DECIMAL;        // 110  KP .
    case VK_DIVIDE:               return UI_KEY_KP_DIVIDE;         // 111  KP /

    // Fn [0x70 ~ 0x87]
    case VK_F1: case VK_F2: case VK_F3: case VK_F4: case VK_F5: case VK_F6:
    case VK_F7: case VK_F8: case VK_F9: case VK_F10: case VK_F11: case VK_F12:
    case VK_F13: case VK_F14: case VK_F15: case VK_F16: case VK_F17: case VK_F18:
    case VK_F19: case VK_F20: case VK_F21: case VK_F22: case VK_F23: case VK_F24:
        return UI_KEY_F1 + (key - VK_F1);

    case VK_NUMLOCK:              return UI_KEY_NUM_LOCK;          // 144  numlock
    case VK_SCROLL:               return UI_KEY_SCROLL_LOCK;       // 145  scroll lock

    // 修饰键
    case VK_LSHIFT:               return UI_KEY_LEFT_SHIFT;        // 160  left shift
    case VK_RSHIFT:               return UI_KEY_RIGHT_SHIFT;       // 161  right shift
    case VK_LCONTROL:             return UI_KEY_LEFT_CONTROL;      // 162  left ctrl
    case VK_RCONTROL:             return UI_KEY_RIGHT_CONTROL;     // 163  right ctrl
    case VK_LMENU:                return UI_KEY_LEFT_ALT;          // 164  left alt
    case VK_RMENU:                return UI_KEY_RIGHT_ALT;         // 165  right alt
    default:
        break;
    }

    return 0;
}

// clang-format on

// 判断按键是否按下
bool key_state(int key)
{
    return GetAsyncKeyState(key_map(key)) & 0x8000;
}

} // namespace minimgui
