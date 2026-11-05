#include "ui_impl_win32.h"
#include "../ui_widgets.h"

#include <windows.h>

namespace minimgui {

static HWND _m_window = nullptr;
// static HDC _m_hdc     = nullptr;
// static HFONT _m_hFont = nullptr;

int init_win32(HWND hwnd, HDC hdc)
{
    _m_window = hwnd;
    // _m_hdc    = hdc;

    // if (!_m_hFont) {
    //     _m_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, GB2312_CHARSET, 0, 0, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Microsoft YaHei"));
    // }

    // 初始化屏幕大小
    RECT rect;
    GetWindowRect(hwnd, &rect);
    on_resize(rect.right - rect.left, rect.bottom - rect.top);

    return 0;
}

// clang-format off

// 按键映射 VKey -> UI Key
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

// 按键映射 UI Keys -> VKey
int key_unmap(int key)
{
    switch (key) {
    case UI_KEY_BACK:            return VK_BACK;           // 8    '\b'
    case UI_KEY_TAB:             return VK_TAB;            // 9    't'
    case UI_KEY_ENTER:           return VK_RETURN;         // 13   '\r'
    case UI_KEY_SHIFT:           return VK_SHIFT;          // 14   shift
    case UI_KEY_CONTROL:         return VK_CONTROL;        // 15   ctrl
    case UI_KEY_ALT:             return VK_MENU;           // 16   alt
    case UI_KEY_META:            return VK_LWIN;           // 17   meta
    case UI_KEY_MENU:            return VK_APPS;           // 18   menu
    case UI_KEY_PRINT_SCREEN:    return VK_SNAPSHOT;       // 19   print screen
    case UI_KEY_PAUSE:           return VK_PAUSE;          // 20   pause

    case UI_KEY_HOME:            return VK_HOME;           // 21   home
    case UI_KEY_END:             return VK_END;            // 22   end
    case UI_KEY_PAGE_UP:         return VK_PRIOR;          // 23   page up
    case UI_KEY_PAGE_DOWN:       return VK_NEXT;           // 24   page down
    case UI_KEY_INSERT:          return VK_INSERT;         // 25   insert
    case UI_KEY_DELETE:          return VK_DELETE;         // 26   delete

    case UI_KEY_ESCAPE:          return VK_ESCAPE;         // 27   escape

    case UI_KEY_LEFT:            return VK_LEFT;           // 28   left
    case UI_KEY_UP:              return VK_UP;             // 29   up
    case UI_KEY_RIGHT:           return VK_RIGHT;          // 30   right
    case UI_KEY_DOWN:            return VK_DOWN;           // 31   down

    case UI_KEY_SPACE:           return VK_SPACE;          // 32   ' '
    case UI_KEY_EXCLAM:          return '1';               // 33   '!' (shift)
    case UI_KEY_DOUBLE_QUOTE:    return VK_OEM_7;          // 34   '"'
    case UI_KEY_NUMBER_SIGN:     return '3';               // 35   '#' (shift)
    case UI_KEY_DOLLAR:          return '4';               // 36   '$' (shift)
    case UI_KEY_PERCENT_SIGN:    return '5';               // 37   '%' (shift)
    case UI_KEY_AMPERSAND:       return '7';               // 38   '&' (shift)
    case UI_KEY_APOSTROPHE:      return VK_OEM_7;          // 39   '\''
    case UI_KEY_LEFT_PAREN:      return '9';               // 40   '(' (shift)
    case UI_KEY_RIGHT_PAREN:     return '0';               // 41   ')' (shift)
    case UI_KEY_ASTERISK:        return VK_MULTIPLY;       // 42   '*'
    case UI_KEY_PLUS:            return VK_OEM_PLUS;       // 43   '+'
    case UI_KEY_COMMA:           return VK_OEM_COMMA;      // 44   ','
    case UI_KEY_MINUS:           return VK_OEM_MINUS;      // 45   '-'
    case UI_KEY_PERIOD:          return VK_OEM_PERIOD;     // 46   '.'
    case UI_KEY_SLASH:           return VK_OEM_2;          // 47   '/'

    // 48 ~ 57 '0' ~ '9'
    case UI_KEY_0: case UI_KEY_1: case UI_KEY_2: case UI_KEY_3: case UI_KEY_4:
    case UI_KEY_5: case UI_KEY_6: case UI_KEY_7: case UI_KEY_8: case UI_KEY_9:
        return '0' + (key - UI_KEY_0);

    case UI_KEY_COLON:           return VK_OEM_1;          // 58   ':' (shift)
    case UI_KEY_SEMICOLON:       return VK_OEM_1;          // 59   ';'
    case UI_KEY_LESS:            return VK_OEM_COMMA;      // 60   '<' (shift)
    case UI_KEY_EQUAL:           return VK_OEM_PLUS;       // 61   '='
    case UI_KEY_GREATER:         return VK_OEM_PERIOD;     // 62   '>' (shift)
    case UI_KEY_QUESTION:        return VK_OEM_2;          // 63   '?' (shift)
    case UI_KEY_AT:              return '2';               // 64   '@' (shift)

    // 65 ~ 90 'A' ~ 'Z'
    case UI_KEY_A: case UI_KEY_B: case UI_KEY_C: case UI_KEY_D: case UI_KEY_E:
    case UI_KEY_F: case UI_KEY_G: case UI_KEY_H: case UI_KEY_I: case UI_KEY_J:
    case UI_KEY_K: case UI_KEY_L: case UI_KEY_M: case UI_KEY_N: case UI_KEY_O:
    case UI_KEY_P: case UI_KEY_Q: case UI_KEY_R: case UI_KEY_S: case UI_KEY_T:
    case UI_KEY_U: case UI_KEY_V: case UI_KEY_W: case UI_KEY_X: case UI_KEY_Y:
    case UI_KEY_Z:
        return 'A' + (key - UI_KEY_A);

    case UI_KEY_LEFT_BRACKET:    return VK_OEM_4;          // 91   '['
    case UI_KEY_BACKSLASH:       return VK_OEM_5;          // 92   '\\'
    case UI_KEY_RIGHT_BRACKET:   return VK_OEM_6;          // 93   ']'
    case UI_KEY_CARET:           return '6';               // 94   '^' (shift)
    case UI_KEY_UNDERSCORE:      return VK_OEM_MINUS;      // 95   '_' (shift)
    case UI_KEY_GRAVE_ACCENT:    return VK_OEM_3;          // 96   '`'

    // 97 ~ 122 'a' ~ 'z'
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        return UI_KEY_A + (key - 'a');

    case UI_KEY_LEFT_BRACE:      return VK_OEM_4;          // 123  '{' (shift)
    case UI_KEY_BAR:             return VK_OEM_5;          // 124  '|' (shift)
    case UI_KEY_RIGHT_BRACE:     return VK_OEM_6;          // 125  '}' (shift)
    case UI_KEY_TILDE:           return VK_OEM_3;          // 126  '~' (shift)
    case UI_KEY_BACKSPACE:       return VK_BACK;           // 127  backspace

    // 修饰键
    case UI_KEY_LEFT_SHIFT:      return VK_LSHIFT;         // 144  left shift
    case UI_KEY_RIGHT_SHIFT:     return VK_RSHIFT;         // 145  right shift
    case UI_KEY_LEFT_CONTROL:    return VK_LCONTROL;       // 146  left ctrl
    case UI_KEY_RIGHT_CONTROL:   return VK_RCONTROL;       // 147  right ctrl
    case UI_KEY_LEFT_ALT:        return VK_LMENU;          // 148  left alt
    case UI_KEY_RIGHT_ALT:       return VK_RMENU;          // 149  right alt
    case UI_KEY_LEFT_META:       return VK_LWIN;           // 150  left meta
    case UI_KEY_RIGHT_META:      return VK_RWIN;           // 151  right meta

    case UI_KEY_CAPS_LOCK:       return VK_CAPITAL;        // 152  capslock
    case UI_KEY_SCROLL_LOCK:     return VK_SCROLL;         // 153  scroll lock
    case UI_KEY_NUM_LOCK:        return VK_NUMLOCK;        // 154  numlock

    // Fn [192 ~ 215]
    case UI_KEY_F1: case UI_KEY_F2: case UI_KEY_F3: case UI_KEY_F4: case UI_KEY_F5:
    case UI_KEY_F6: case UI_KEY_F7: case UI_KEY_F8: case UI_KEY_F9: case UI_KEY_F10:
    case UI_KEY_F11: case UI_KEY_F12: case UI_KEY_F13: case UI_KEY_F14: case UI_KEY_F15:
    case UI_KEY_F16: case UI_KEY_F17: case UI_KEY_F18: case UI_KEY_F19: case UI_KEY_F20:
    case UI_KEY_F21: case UI_KEY_F22: case UI_KEY_F23: case UI_KEY_F24:
        return VK_F1 + (key - UI_KEY_F1);

    // 数字小键盘
    case UI_KEY_KP_0:            return VK_NUMPAD0;         // 224  KP 0
    case UI_KEY_KP_1:            return VK_NUMPAD1;         // 225  KP 1
    case UI_KEY_KP_2:            return VK_NUMPAD2;         // 226  KP 2
    case UI_KEY_KP_3:            return VK_NUMPAD3;         // 227  KP 3
    case UI_KEY_KP_4:            return VK_NUMPAD4;         // 228  KP 4
    case UI_KEY_KP_5:            return VK_NUMPAD5;         // 229  KP 5
    case UI_KEY_KP_6:            return VK_NUMPAD6;         // 230  KP 6
    case UI_KEY_KP_7:            return VK_NUMPAD7;         // 231  KP 7
    case UI_KEY_KP_8:            return VK_NUMPAD8;         // 232  KP 8
    case UI_KEY_KP_9:            return VK_NUMPAD9;         // 233  KP 9
    case UI_KEY_KP_DECIMAL:      return VK_DECIMAL;         // 234  KP .
    case UI_KEY_KP_DIVIDE:       return VK_DIVIDE;          // 235  KP /
    case UI_KEY_KP_MULTIPLY:     return VK_MULTIPLY;        // 236  KP *
    case UI_KEY_KP_SUBTRACT:     return VK_SUBTRACT;        // 237  KP -
    case UI_KEY_KP_ADD:          return VK_ADD;             // 238  KP +
    case UI_KEY_KP_ENTER:        return VK_RETURN;          // 239  KP Enter
    case UI_KEY_KP_EQUAL:        return VK_OEM_NEC_EQUAL;   // 240  KP =

    default:
        break;
    }
    return 0;
}

// clang-format on

// 判断按键是否按下
bool key_state(int key)
{
    return GetAsyncKeyState(key_unmap(key)) & 0x8000;
}

} // namespace minimgui
