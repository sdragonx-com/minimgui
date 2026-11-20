#include "ui_impl_glfw.h"
#include "../ui_widgets.h"

namespace minimgui {

static GLFWwindow* _m_window = nullptr;
static uiVec2 _m_mouse;
static uiButton _m_button = UI_BUTTON_NONE;

static GLFWframebuffersizefun _m_prevOnResize = nullptr;
static GLFWkeyfun _m_prevOnKeyboard           = nullptr;
static GLFWcharfun _m_prevOnInput             = nullptr;
static GLFWmousebuttonfun _m_prevOnMouse      = nullptr;
static GLFWcursorposfun _m_prevOnMotion       = nullptr;
static GLFWscrollfun _m_prevOnScroll          = nullptr;

static void reshape_event(GLFWwindow* window, int width, int height);
static void keyboard_event(GLFWwindow* window, int key, int scancode, int action, int modifier);
static void input_event(GLFWwindow* window, unsigned int codepoint);
static void mouse_event(GLFWwindow* window, int button, int action, int mods);
static void motion_event(GLFWwindow* window, double x, double y);
static void scroll_event(GLFWwindow* window, double x, double y);

int init_glfw(GLFWwindow* window, bool installEvents)
{
    _m_window = window;

    // 接管 GLFW 消息
    if (installEvents) {
        _m_prevOnResize   = glfwSetFramebufferSizeCallback(window, reshape_event);
        _m_prevOnKeyboard = glfwSetKeyCallback(window, keyboard_event);
        _m_prevOnInput    = glfwSetCharCallback(window, input_event);
        _m_prevOnMouse    = glfwSetMouseButtonCallback(window, mouse_event);
        _m_prevOnMotion   = glfwSetCursorPosCallback(window, motion_event);
        _m_prevOnScroll   = glfwSetScrollCallback(window, scroll_event);
    }
    return 0;
}

// clang-format off

// 按键映射 GLFW keys -> UI keys
int key_map(int key)
{
    switch (key) {
    case GLFW_KEY_SPACE:            return UI_KEY_SPACE;           // 32  -> 32
    case GLFW_KEY_APOSTROPHE:       return UI_KEY_APOSTROPHE;      // 39  -> 39
    case GLFW_KEY_COMMA:            return UI_KEY_COMMA;           // 44  -> 44
    case GLFW_KEY_MINUS:            return UI_KEY_MINUS;           // 45  -> 45
    case GLFW_KEY_PERIOD:           return UI_KEY_PERIOD;          // 46  -> 46
    case GLFW_KEY_SLASH:            return UI_KEY_SLASH;           // 47  -> 47

    // '0' ~ '9' [48 ~ 57]
    case GLFW_KEY_0: case GLFW_KEY_1: case GLFW_KEY_2: case GLFW_KEY_3: case GLFW_KEY_4:
    case GLFW_KEY_5: case GLFW_KEY_6: case GLFW_KEY_7: case GLFW_KEY_8: case GLFW_KEY_9:
        return UI_KEY_0 + (key - GLFW_KEY_0);

    case GLFW_KEY_SEMICOLON:        return UI_KEY_SEMICOLON;       // 59  -> 59
    case GLFW_KEY_EQUAL:            return UI_KEY_EQUAL;           // 61  -> 61

    // 'A' ~ 'Z' [65 ~ 90]
    case GLFW_KEY_A: case GLFW_KEY_B: case GLFW_KEY_C: case GLFW_KEY_D: case GLFW_KEY_E:
    case GLFW_KEY_F: case GLFW_KEY_G: case GLFW_KEY_H: case GLFW_KEY_I: case GLFW_KEY_J:
    case GLFW_KEY_K: case GLFW_KEY_L: case GLFW_KEY_M: case GLFW_KEY_N: case GLFW_KEY_O:
    case GLFW_KEY_P: case GLFW_KEY_Q: case GLFW_KEY_R: case GLFW_KEY_S: case GLFW_KEY_T:
    case GLFW_KEY_U: case GLFW_KEY_V: case GLFW_KEY_W: case GLFW_KEY_X: case GLFW_KEY_Y:
    case GLFW_KEY_Z:
        return UI_KEY_A + (key - GLFW_KEY_A);

    case GLFW_KEY_LEFT_BRACKET:     return UI_KEY_LEFT_BRACKET;    // 91  -> 91
    case GLFW_KEY_BACKSLASH:        return UI_KEY_BACKSLASH;       // 92  -> 92
    case GLFW_KEY_RIGHT_BRACKET:    return UI_KEY_RIGHT_BRACKET;   // 93  -> 93
    case GLFW_KEY_GRAVE_ACCENT:     return UI_KEY_GRAVE_ACCENT;    // 96  -> 96
    // case GLFW_KEY_WORLD_1:                                       // 161
    // case GLFW_KEY_WORLD_2:                                       // 162

    // 功能键区
    case GLFW_KEY_ESCAPE:           return UI_KEY_ESCAPE;          // 256 -> 27
    case GLFW_KEY_ENTER:            return UI_KEY_ENTER;           // 257 -> 13
    case GLFW_KEY_TAB:              return UI_KEY_TAB;             // 258 -> 9
    case GLFW_KEY_BACKSPACE:        return UI_KEY_BACKSPACE;       // 259 -> 127
    case GLFW_KEY_INSERT:           return UI_KEY_INSERT;          // 260 -> 25
    case GLFW_KEY_DELETE:           return UI_KEY_DELETE;          // 261 -> 26
    case GLFW_KEY_RIGHT:            return UI_KEY_RIGHT;           // 262 -> 30
    case GLFW_KEY_LEFT:             return UI_KEY_LEFT;            // 263 -> 28
    case GLFW_KEY_DOWN:             return UI_KEY_DOWN;            // 264 -> 31
    case GLFW_KEY_UP:               return UI_KEY_UP;              // 265 -> 29
    case GLFW_KEY_PAGE_UP:          return UI_KEY_PAGE_UP;         // 266 -> 23
    case GLFW_KEY_PAGE_DOWN:        return UI_KEY_PAGE_DOWN;       // 267 -> 24
    case GLFW_KEY_HOME:             return UI_KEY_HOME;            // 268 -> 21
    case GLFW_KEY_END:              return UI_KEY_END;             // 269 -> 22
    case GLFW_KEY_CAPS_LOCK:        return UI_KEY_CAPS_LOCK;       // 280 -> 152
    case GLFW_KEY_SCROLL_LOCK:      return UI_KEY_SCROLL_LOCK;     // 281 -> 153
    case GLFW_KEY_NUM_LOCK:         return UI_KEY_NUM_LOCK;        // 282 -> 154
    case GLFW_KEY_PRINT_SCREEN:     return UI_KEY_PRINT_SCREEN;    // 283 -> 19
    case GLFW_KEY_PAUSE:            return UI_KEY_PAUSE;           // 284 -> 20

    // Fn [290 ~ 313] -> [192 ~ 215]
    case GLFW_KEY_F1:  case GLFW_KEY_F2:  case GLFW_KEY_F3:  case GLFW_KEY_F4:  case GLFW_KEY_F5:
    case GLFW_KEY_F6:  case GLFW_KEY_F7:  case GLFW_KEY_F8:  case GLFW_KEY_F9:  case GLFW_KEY_F10:
    case GLFW_KEY_F11: case GLFW_KEY_F12: case GLFW_KEY_F13: case GLFW_KEY_F14: case GLFW_KEY_F15:
    case GLFW_KEY_F16: case GLFW_KEY_F17: case GLFW_KEY_F18: case GLFW_KEY_F19: case GLFW_KEY_F20:
    case GLFW_KEY_F21: case GLFW_KEY_F22: case GLFW_KEY_F23: case GLFW_KEY_F24:
    // case GLFW_KEY_F25:                                           // 314
        return UI_KEY_F1 + (key - GLFW_KEY_F1);

    // 数字小键盘 [320 ~ 336] -> [224 ~ 240]
    case GLFW_KEY_KP_0: case GLFW_KEY_KP_1: case GLFW_KEY_KP_2: case GLFW_KEY_KP_3:
    case GLFW_KEY_KP_4: case GLFW_KEY_KP_5: case GLFW_KEY_KP_6: case GLFW_KEY_KP_7:
    case GLFW_KEY_KP_8: case GLFW_KEY_KP_9:
    case GLFW_KEY_KP_DECIMAL:
    case GLFW_KEY_KP_DIVIDE:
    case GLFW_KEY_KP_MULTIPLY:
    case GLFW_KEY_KP_SUBTRACT:
    case GLFW_KEY_KP_ADD:
    case GLFW_KEY_KP_ENTER:
    case GLFW_KEY_KP_EQUAL:
        return UI_KEY_KP_0 + (key - GLFW_KEY_KP_0);

    // 修饰键
    case GLFW_KEY_LEFT_SHIFT:       return UI_KEY_LEFT_SHIFT;      // 340 -> 144
    case GLFW_KEY_LEFT_CONTROL:     return UI_KEY_LEFT_CONTROL;    // 341 -> 146
    case GLFW_KEY_LEFT_ALT:         return UI_KEY_LEFT_ALT;        // 342 -> 148
    case GLFW_KEY_LEFT_SUPER:       return UI_KEY_LEFT_META;       // 343 -> 150
    case GLFW_KEY_RIGHT_SHIFT:      return UI_KEY_RIGHT_SHIFT;     // 344 -> 145
    case GLFW_KEY_RIGHT_CONTROL:    return UI_KEY_RIGHT_CONTROL;   // 345 -> 147
    case GLFW_KEY_RIGHT_ALT:        return UI_KEY_RIGHT_ALT;       // 346 -> 149
    case GLFW_KEY_RIGHT_SUPER:      return UI_KEY_RIGHT_META;      // 347 -> 151
    case GLFW_KEY_MENU:             return UI_KEY_MENU;            // 348 -> 18
    default:
        break;
    }
    return UI_KEY_NONE;
}

// 按键映射 UI keys -> GLFW keys
int key_unmap(int key)
{
    switch (key) {
    case UI_KEY_BACK:            return GLFW_KEY_BACKSPACE;        // '\b'  8 -> 259
    case UI_KEY_TAB:             return GLFW_KEY_TAB;              // '\t'  9 -> 258
    case UI_KEY_ENTER:           return GLFW_KEY_ENTER;            // '\r' 13 -> 257
    case UI_KEY_SHIFT:           return GLFW_KEY_LEFT_SHIFT;       // 14 -> 340
    case UI_KEY_CONTROL:         return GLFW_KEY_LEFT_CONTROL;     // 15 -> 341
    case UI_KEY_ALT:             return GLFW_KEY_LEFT_ALT;         // 16 -> 342
    case UI_KEY_META:            return GLFW_KEY_LEFT_SUPER;       // 17 -> 343
    case UI_KEY_MENU:            return GLFW_KEY_MENU;             // 18 -> 348
    case UI_KEY_PRINT_SCREEN:    return GLFW_KEY_PRINT_SCREEN;     // 19 -> 283
    case UI_KEY_PAUSE:           return GLFW_KEY_PAUSE;            // 20 -> 284

    case UI_KEY_HOME:            return GLFW_KEY_HOME;             // 21 -> 268
    case UI_KEY_END:             return GLFW_KEY_END;              // 22 -> 269
    case UI_KEY_PAGE_UP:         return GLFW_KEY_PAGE_UP;          // 23 -> 266
    case UI_KEY_PAGE_DOWN:       return GLFW_KEY_PAGE_DOWN;        // 24 -> 267
    case UI_KEY_INSERT:          return GLFW_KEY_INSERT;           // 25 -> 260
    case UI_KEY_DELETE:          return GLFW_KEY_DELETE;           // 26 -> 261

    case UI_KEY_ESCAPE:          return GLFW_KEY_ESCAPE;           // 27 -> 256

    case UI_KEY_LEFT:            return GLFW_KEY_LEFT;             // 28 -> 263
    case UI_KEY_UP:              return GLFW_KEY_UP;               // 29 -> 265
    case UI_KEY_RIGHT:           return GLFW_KEY_RIGHT;            // 30 -> 262
    case UI_KEY_DOWN:            return GLFW_KEY_DOWN;             // 31 -> 264

    case UI_KEY_SPACE:           return GLFW_KEY_SPACE;            // ' ' 32 -> 32
    // case UI_KEY_EXCLAM:                                         // '!' 33
    // case UI_KEY_DOUBLE_QUOTE:                                   // '"' 34
    // case UI_KEY_NUMBER_SIGN:                                    // '#' 35
    // case UI_KEY_DOLLAR:                                         // '$' 36
    // case UI_KEY_PERCENT_SIGN:                                   // '%' 37
    // case UI_KEY_AMPERSAND:                                      // '&' 38
    case UI_KEY_APOSTROPHE:      return GLFW_KEY_APOSTROPHE;       // ''' 39 -> 39
    // case UI_KEY_LEFT_PAREN:                                     // '(' 40
    // case UI_KEY_RIGHT_PAREN:                                    // ')' 41
    // case UI_KEY_ASTERISK:                                       // '*' 42
    // case UI_KEY_PLUS:                                           // '+' 43
    case UI_KEY_COMMA:           return GLFW_KEY_COMMA;            // ',' 44 -> 44
    case UI_KEY_MINUS:           return GLFW_KEY_MINUS;            // '-' 45 -> 45
    case UI_KEY_PERIOD:          return GLFW_KEY_PERIOD;           // '.' 46 -> 46
    case UI_KEY_SLASH:           return GLFW_KEY_SLASH;            // '/' 47 -> 47

    // '0' ~ '9' [48 ~ 57]
    case UI_KEY_0: case UI_KEY_1: case UI_KEY_2: case UI_KEY_3: case UI_KEY_4:
    case UI_KEY_5: case UI_KEY_6: case UI_KEY_7: case UI_KEY_8: case UI_KEY_9:
        return GLFW_KEY_0 + (key - UI_KEY_0);
    // case UI_KEY_COLON:                                          // ':' 58
    case UI_KEY_SEMICOLON:       return GLFW_KEY_SEMICOLON;        // ';' 59 -> 59
    // case UI_KEY_LESS:                                           // '<' 60
    case UI_KEY_EQUAL:           return GLFW_KEY_EQUAL;            // '=' 61 -> 61
    // case UI_KEY_GREATER:                                        // '>' 62
    // case UI_KEY_QUESTION:                                       // '?' 63
    // case UI_KEY_AT:                                             // '@' 64

    // 'A' ~ 'Z' [65 ~ 90]
    case UI_KEY_A: case UI_KEY_B: case UI_KEY_C: case UI_KEY_D: case UI_KEY_E:
    case UI_KEY_F: case UI_KEY_G: case UI_KEY_H: case UI_KEY_I: case UI_KEY_J:
    case UI_KEY_K: case UI_KEY_L: case UI_KEY_M: case UI_KEY_N: case UI_KEY_O:
    case UI_KEY_P: case UI_KEY_Q: case UI_KEY_R: case UI_KEY_S: case UI_KEY_T:
    case UI_KEY_U: case UI_KEY_V: case UI_KEY_W: case UI_KEY_X: case UI_KEY_Y:
    case UI_KEY_Z:
        return GLFW_KEY_A + (key - UI_KEY_A);

    // case UI_KEY_LEFT_BRACKET:                                   // '[' 91
    // case UI_KEY_BACKSLASH:                                      // '\' 92
    // case UI_KEY_RIGHT_BRACKET:                                  // ']' 93
    // case UI_KEY_CARET:                                          // '^' 94
    // case UI_KEY_UNDERSCORE:                                     // '_' 95
    // case UI_KEY_GRAVE_ACCENT:                                   // '`' 96

    // 'a' ~ 'z' [97 ~ 122]

    // case UI_KEY_LEFT_BRACE:                                     // 123 '{'
    // case UI_KEY_BAR:                                            // 124 '|'
    // case UI_KEY_RIGHT_BRACE:                                    // 125 '}'
    // case UI_KEY_TILDE:                                          // 126 '~'
    case UI_KEY_BACKSPACE:       return GLFW_KEY_BACKSPACE;        // 127 -> 259

    // 扩展功能键
    case UI_KEY_LEFT_SHIFT:      return GLFW_KEY_LEFT_SHIFT;       // 144 -> 340
    case UI_KEY_RIGHT_SHIFT:     return GLFW_KEY_RIGHT_SHIFT;      // 145 -> 344
    case UI_KEY_LEFT_CONTROL:    return GLFW_KEY_LEFT_CONTROL;     // 146 -> 341
    case UI_KEY_RIGHT_CONTROL:   return GLFW_KEY_RIGHT_CONTROL;    // 147 -> 345
    case UI_KEY_LEFT_ALT:        return GLFW_KEY_LEFT_ALT;         // 148 -> 342
    case UI_KEY_RIGHT_ALT:       return GLFW_KEY_RIGHT_ALT;        // 149 -> 346
    case UI_KEY_LEFT_META:       return GLFW_KEY_LEFT_SUPER;       // 150 -> 343
    case UI_KEY_RIGHT_META:      return GLFW_KEY_RIGHT_SUPER;      // 151 -> 347

    case UI_KEY_CAPS_LOCK:       return GLFW_KEY_CAPS_LOCK;        // 152 -> 280
    case UI_KEY_SCROLL_LOCK:     return GLFW_KEY_SCROLL_LOCK;      // 153 -> 281
    case UI_KEY_NUM_LOCK:        return GLFW_KEY_NUM_LOCK;         // 154 -> 282

    // UI F1=192 -> GLFW F1=290
    case UI_KEY_F1: case UI_KEY_F2: case UI_KEY_F3: case UI_KEY_F4: case UI_KEY_F5:
    case UI_KEY_F6: case UI_KEY_F7: case UI_KEY_F8: case UI_KEY_F9: case UI_KEY_F10:
    case UI_KEY_F11: case UI_KEY_F12: case UI_KEY_F13: case UI_KEY_F14: case UI_KEY_F15:
    case UI_KEY_F16: case UI_KEY_F17: case UI_KEY_F18: case UI_KEY_F19: case UI_KEY_F20:
    case UI_KEY_F21: case UI_KEY_F22: case UI_KEY_F23: case UI_KEY_F24:
        return GLFW_KEY_F1 + (key - UI_KEY_F1);

    // 数字小键盘 [224 ~ 240] -> [320 ~ 336]
    case UI_KEY_KP_0: case UI_KEY_KP_1: case UI_KEY_KP_2: case UI_KEY_KP_3:
    case UI_KEY_KP_4: case UI_KEY_KP_5: case UI_KEY_KP_6: case UI_KEY_KP_7:
    case UI_KEY_KP_8: case UI_KEY_KP_9:
    case UI_KEY_KP_DECIMAL:    return GLFW_KEY_KP_DECIMAL;
    case UI_KEY_KP_DIVIDE:     return GLFW_KEY_KP_DIVIDE;
    case UI_KEY_KP_MULTIPLY:   return GLFW_KEY_KP_MULTIPLY;
    case UI_KEY_KP_SUBTRACT:   return GLFW_KEY_KP_SUBTRACT;
    case UI_KEY_KP_ADD:        return GLFW_KEY_KP_ADD;
    case UI_KEY_KP_ENTER:      return GLFW_KEY_KP_ENTER;
    case UI_KEY_KP_EQUAL:      return GLFW_KEY_KP_EQUAL;
          return GLFW_KEY_KP_0 + (key - UI_KEY_KP_0);
    default:
        break;
    }
    return GLFW_KEY_UNKNOWN;
}

// clang-format on

// 判断按键是否按下
bool key_state(int key)
{
    switch (key) {
    case UI_KEY_SHIFT:
        return glfwGetKey(_m_window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(_m_window, GLFW_KEY_RIGHT_SHIFT);
    case UI_KEY_CONTROL:
        return glfwGetKey(_m_window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(_m_window, GLFW_KEY_RIGHT_CONTROL);
    case UI_KEY_ALT:
        return glfwGetKey(_m_window, GLFW_KEY_LEFT_ALT) || glfwGetKey(_m_window, GLFW_KEY_RIGHT_ALT);
    default:
        break;
    }
    return glfwGetKey(_m_window, key_unmap(key));
}

static void reshape_event(GLFWwindow* window, int width, int height)
{
    context()->screen = uiVec2(width, height);

    if (_m_prevOnResize) {
        _m_prevOnResize(window, width, height);
    }
}

// 键盘消息
static void keyboard_event(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
    switch (action) {
    case GLFW_PRESS:
        minimgui::on_key_push(minimgui::key_map(key));
        break;
    case GLFW_RELEASE:
        minimgui::on_key_pop(minimgui::key_map(key));
        break;
    case GLFW_REPEAT:
        break;
    default: break;
    }

    if (_m_prevOnKeyboard) {
        _m_prevOnKeyboard(window, key, scancode, action, modifier);
    }
}

// 字符输入消息
static void input_event(GLFWwindow* window, unsigned int codepoint)
{
    minimgui::on_input(codepoint);

    if (_m_prevOnInput) {
        _m_prevOnInput(window, codepoint);
    }
}

// 鼠标按键消息
static void mouse_event(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            _m_button = UI_BUTTON_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            _m_button = UI_BUTTON_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            _m_button = UI_BUTTON_RIGHT;
            break;
        default:
            return;
        }
        minimgui::on_mouse_push(_m_mouse.x, _m_mouse.y, _m_button);
    }
    else if (action == GLFW_RELEASE) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            _m_button = UI_BUTTON_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            _m_button = UI_BUTTON_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            _m_button = UI_BUTTON_RIGHT;
            break;
        default:
            return;
        }
        minimgui::on_mouse_pop(_m_mouse.x, _m_mouse.y, _m_button);
    }

    if (_m_prevOnMouse) {
        _m_prevOnMouse(window, button, action, mods);
    }
}

// 鼠标移动消息
static void motion_event(GLFWwindow* window, double x, double y)
{
    // printf("mouse move : %d, %d\n", int(x), int(y));
    _m_mouse = uiVec2(x, y);

    minimgui::on_mouse_move(_m_mouse.x, _m_mouse.y, _m_button);

    if (_m_prevOnMotion) {
        _m_prevOnMotion(window, x, y);
    }
}

// 滚轮滚动
static void scroll_event(GLFWwindow* window, double x, double y)
{
    minimgui::on_scroll(y);

    if (_m_prevOnScroll) {
        _m_prevOnScroll(window, x, y);
    }
}

} // namespace minimgui
