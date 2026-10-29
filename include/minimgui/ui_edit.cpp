#include "ui_edit.h"
#include "ui_widgets.h"
#include "ui_user.h"

#include <ctype.h>
#include <string.h>
#include <time.h>

namespace minimgui {

//------------------------------------------------------------------------------
// 编辑框
//------------------------------------------------------------------------------

// 文本变动事件
static void edit_on_change(uiEdit* edit)
{
    // 获取字符宽度列表
    edit->sizeCache.clear();
    for (int i = 0; i < edit->length; ++i) {
        uiChar ch = edit->buffer[i];
        edit->sizeCache.push_back(char_width(ch));
    }
    edit->sizeCache.push_back(32768.0f); // 尾部增加一个很大的数字，防止鼠标点击判断越界
}

// 删除选择的字符
static bool edit_delete(uiEdit* edit, bool backspace)
{
    // 判断选择范围
    if (edit->selectStart == edit->selectEnd) {
        if (backspace) { // [backspace]
            if (edit->selectEnd > 0) {
                size_t size = edit->length - edit->selectEnd;
                memmove(edit->buffer + edit->selectEnd - 1, edit->buffer + edit->selectEnd, size);
                --edit->selectStart;
                --edit->selectEnd;
            }
            else {
                return false;
            }
        }
        else { // [delete]
            if (edit->selectEnd < edit->length) {
                size_t size = edit->length - edit->selectEnd;
                memmove(edit->buffer + edit->selectEnd, edit->buffer + edit->selectEnd + 1, size - 1);
            }
            else {
                return false;
            }
        }

        --edit->length;
        edit->buffer[edit->length] = '\0';
    }
    else {
        // 删除选择的字符
        const int start = std::min(edit->selectStart, edit->selectEnd); // 选择范围
        const int end   = std::max(edit->selectStart, edit->selectEnd);
        const int size  = end - start;
        memmove(edit->buffer + start, edit->buffer + end, size);

        edit->length -= size;
        edit->buffer[edit->length] = '\0';

        edit->selectEnd = edit->selectStart = start;
    }

    edit_on_change(edit);

    return true;
}

// 处理按键输入
static bool edit_keyinput(uiEdit* edit, int key)
{
    switch (key) {
    case UI_KEY_LEFT:
        if (edit->selectEnd > 0) {
            if (key_state(UI_KEY_SHIFT)) {
                --edit->selectEnd;
            }
            else {
                edit->selectStart = --edit->selectEnd;
            }
        }
        break;
    case UI_KEY_RIGHT:
        if (edit->selectEnd < edit->length) {
            if (key_state(UI_KEY_SHIFT)) {
                ++edit->selectEnd;
            }
            else {
                edit->selectStart = ++edit->selectEnd;
            }
        }
        break;
    case UI_KEY_HOME:
        if (key_state(UI_KEY_SHIFT)) {
            edit->selectEnd = 0;
            edit->offset    = 0;
        }
        else {
            edit->selectStart = edit->selectEnd = 0;
            edit->offset                        = 0;
        }
        edit->offset = 0;

        // tudo... 字符显示数量

        break;
    case UI_KEY_END:
        if (key_state(UI_KEY_SHIFT)) {
            edit->selectEnd = edit->length;
        }
        else {
            edit->selectStart = edit->selectEnd = edit->length;
        }

        // 计算从尾部算起，显示字符的数量
        if (text_width(edit->buffer, edit->length) > edit->bounds.w) {
            int pos     = edit->length - 1;
            float width = 0.0f;
            while (pos > 0) {
                width += edit->sizeCache[pos - 1];
                // float w = text_width(edit->buffer + pos - 1, edit->length - pos + 1);
                if (width > edit->bounds.w) {
                    edit->offset = pos;
                    break;
                }
                --pos;
            }
        }
        break;
    case UI_KEY_BACK:
        return edit_delete(edit, true);
        break;
    case UI_KEY_DELETE:
        return edit_delete(edit, false);
        break;
    default:
        break;
    }
    return false;
}

// 处理字符输入
bool edit_input(uiEdit* edit, int ch)
{
    // 判断是否控制字符
    if (iscntrl(ch)) {
        switch (ch) {
        case '\b': // 退格键
        case 127:
            // edit_delete(edit, true);
            break;
        default:
            printf("ch: %d\n", ch);
            break;
        }
        return false;
    }

    // 判断选择范围
    if (edit->selectStart == edit->selectEnd) {
        // 插入模式
        if (edit->length + 1 < edit->bufferSize) {
            // 插入模式，把 edit->select 往后移动
            if (edit->selectEnd < edit->length) {
                size_t size = edit->length - edit->selectEnd;
                memmove(edit->buffer + edit->selectEnd + 1, edit->buffer + edit->selectEnd, size);
            }
            edit->buffer[edit->selectEnd] = ch;

            ++edit->length;
            edit->buffer[edit->length] = '\0';

            edit->selectStart = ++edit->selectEnd;

            edit_on_change(edit);
        }
    }
    else {
        // 替换选择
        const int start = std::min(edit->selectStart, edit->selectEnd); // 选择范围
        const int end   = std::max(edit->selectStart, edit->selectEnd);
        const int size  = edit->length - end;

        memmove(edit->buffer + start + 1, edit->buffer + end, size);

        edit->buffer[start] = ch;

        edit->length += 1 - (end - start);
        edit->buffer[edit->length] = '\0';

        edit->selectStart = edit->selectEnd = start + 1;

        edit_on_change(edit);
    }

    // 键入字符之后，修正字符显示偏移量
    /*
    while (true) {
        const uiChar* str = edit->buffer + edit->offset;    // 显示范围内的字符串
        int length        = edit->selectEnd - edit->offset; // 显示范围内的字符串长度
        float w           = length > 0 ? text_width(str, length) : 0.0f;
        if (w > edit->bounds.w) {
            edit->offset++;
        }
        else {
            break;
        }
    }
    */
    float w = text_width(edit->buffer + edit->offset, edit->selectEnd - edit->offset);
    while (w > edit->bounds.w) {
        w -= edit->sizeCache[edit->offset];
        ++edit->offset;
    }

    return true;
}

// 绘制编辑框
static void draw_edit(uiCommandList* cmdlist, uiEdit* edit, bool focused)
{
    draw_frame(
        cmdlist, edit,
        // select_widget_foreground(is_hovered(edit), is_focus(edit)),
        select_color(style()->widgetForeground, is_hovered(edit), false),
        style()->background[UI_COLOR_NORMAL]
    );

    // 文字起始坐标
    const float x = edit->bounds.x + style()->framePadding.x;
    const float y = edit->bounds.y + style()->framePadding.y;

    const uiChar* str = edit->buffer + edit->offset; // 显示范围内的字符串

    // 选择范围
    const int start  = edit->selectStart - edit->offset;
    const int end    = edit->selectEnd - edit->offset;
    const int length = end - start; // 选择的字符串长度

    float startPos = start > 0 ? text_width(str, start) : 0.0f;
    float endPos   = end > 0 ? text_width(str, end) : 0.0f;
    float h        = char_height();

    // 显示选择范围背景
    if (length) {
        cmdlist->set_fill(style()->background[UI_COLOR_HIGHLIGHT]);
        if (length < 0) {
            cmdlist->fill_rect(x + endPos, y, startPos - endPos, h);
        }
        else {
            cmdlist->fill_rect(x + startPos, y, endPos - startPos, h);
        }
    }

    // 绘制字符串
    cmdlist->set_text(style()->foreground[UI_COLOR_NORMAL]);
    cmdlist->draw_string(x, y, str);

    // 绘制光标
    if (focused) {
        clock_t t = clock() % clock_t(CLOCKS_PER_SEC);
        if (t < clock_t(CLOCKS_PER_SEC) / 2) {
            cmdlist->set_stroke(colors::black);
            cmdlist->draw_line(x + endPos, y, x + endPos, y + h);
        }
    }
}

// 编辑框部件
bool edit(uiChar* buffer, size_t size)
{
    // 使用字符串地址作为 ID
    uiID id = ui_hash(&buffer, sizeof(void*));

    // 查找缓存部件
    uiEdit* edit = static_cast<uiEdit*>(find_child_by_id(id));
    if (!edit) {
        edit              = new uiEdit(id, layout()->container);
        edit->buffer      = buffer;
        edit->bufferSize  = size;
        edit->length      = _tcslen(buffer);
        edit->offset      = 0;
        edit->selectStart = edit->selectEnd = edit->length;
        edit->sizeCache.reserve(size + 1);
        layout()->owner->widgets.push_back(edit);

        // 初始化字符宽度缓存
        if (edit->length) {
            edit_on_change(edit);
        }
    }

    // 计算部件范围
    float cy     = char_height() + style()->framePadding.y * 2.0f;
    edit->bounds = allocate_widget_rect(-1.0f, cy);

    bool focused = is_focus(edit);

    // 鼠标点击事件
    uiFlags flags = check_widget_mouse_event(edit);
    bool clicked  = check_flags(flags, UI_FLAGS_CLICKED);
    if (clicked) {
        // 设置焦点
        if (!focused) {
            set_focus(edit);
            focused = true;
        }

        // 鼠标点击设置光标
        float x = context()->mouse.x - edit->bounds.x + style()->framePadding.x;
        // printf("%f\n", x);

            #if defined(UI_USE_UTF8)
        float pos = 0.0f;
        // int caret = 0;
        for (int i = edit->offset; i <= (int) sizes.size(); ++i) {
            if (pos + sizes[i] > x) {
                edit->selectStart = edit->selectEnd = i;
                break;
            }
            pos += sizes[i];
        }
            #else
        float pos = 0.0f;
        for (int i = edit->offset; i <= static_cast<int>(edit->sizeCache.size()); ++i) {
            if (pos + edit->sizeCache[i] > x) {
                edit->selectStart = edit->selectEnd = i;
                printf("mouse.x: %f  pos: %f\n", x, pos);
                break;
            }
            pos += edit->sizeCache[i];
        }
            #endif
    }

    bool changed = false;
    if (focused) {
        // 判断按键输入
        if (context()->key) {
            changed        = edit_keyinput(edit, context()->key);
            context()->key = 0; // 清空按键
        }

        // 判断字符输入
        if (context()->ch) {
            // printf("input: %d\n", context()->ch);
            changed       = edit_input(edit, context()->ch);
            context()->ch = 0; // 清空字符
        }
    }

    // 绘制
    draw_edit(command_list(), edit, focused);

    return changed;
}

} // namespace minimgui
