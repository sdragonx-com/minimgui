// #include "ui_edit.h"
#include "ui_widgets.h"
#include "ui_user.h"

#include <ctype.h>
#include <string.h>
#include <time.h>

#include <windows.h>

namespace minimgui {

// 获取 utf8 字符串宽度
static float text_width_utf8(const unsigned char* buf, int byteLen)
{
    float w = 0.0f;
    for (int i = 0; i < byteLen;) {
        uint32_t cp;
        int n = (int) utf8_decode(buf + i, byteLen, &cp, nullptr);
        w += char_width(static_cast<int>(cp));
        i += n;
    }
    return w;
}

//------------------------------------------------------------------------------
// 编辑框
//------------------------------------------------------------------------------

// 文本变动事件
static void edit_on_changed(uiEdit_u8* edit)
{
    (void) edit;
}

// 文本选择变动事件
static void edit_on_select_changed(uiEdit_u8* edit)
{
    (void) edit;
    printf("edit: selstart: %d, selend: %d\n", edit->selectStart, edit->selectEnd);
}

// 删除选择的字符
static bool edit_delete(uiEdit_u8* edit, bool backspace)
{
    // 删除一个码点
    if (edit->selectStart == edit->selectEnd) {
        if (backspace) { //[backspace]
            if (edit->selectEnd > 0) {
                int prev    = utf8_prev(reinterpret_cast<const unsigned char*>(edit->buffer), edit->selectEnd);
                int n       = edit->selectEnd - prev;
                size_t size = edit->length - edit->selectEnd;
                memmove(edit->buffer + prev, edit->buffer + edit->selectEnd, size);
                edit->length -= n;
                edit->buffer[edit->length] = '\0';
                edit->selectStart = edit->selectEnd = prev;
            }
            else {
                return false;
            }
        }
        else { // [delete]
            if (edit->selectEnd < edit->length) {
                int next    = utf8_next(reinterpret_cast<const unsigned char*>(edit->buffer), edit->selectEnd, edit->length);
                int n       = next - edit->selectEnd;
                size_t size = edit->length - next;
                memmove(edit->buffer + edit->selectEnd, edit->buffer + next, size);
                edit->length -= n;
                edit->buffer[edit->length] = '\0';
            }
            else {
                return false;
            }
        }
    }
    else {
        // 删除选择范围
        // 计算选择范围，start 必须小于 end
        const int start = std::min(edit->selectStart, edit->selectEnd);
        const int end   = std::max(edit->selectStart, edit->selectEnd);
        const int size  = edit->length - end;
        memmove(edit->buffer + start, edit->buffer + end, size);
        edit->length -= (end - start);
        edit->buffer[edit->length] = '\0';
        edit->selectEnd = edit->selectStart = start;
    }

    edit_on_changed(edit);
    edit_on_select_changed(edit);

    return true;
}

// 处理按键输入
static bool edit_keyinput(uiEdit_u8* edit, int key)
{
    switch (key) {
    case UI_KEY_LEFT:
        if (edit->selectEnd > 0) {
            int prev = utf8_prev(reinterpret_cast<const unsigned char*>(edit->buffer), edit->selectEnd);
            if (key_state(UI_KEY_SHIFT)) { // 左移 selend
                edit->selectEnd = prev;
            }
            else { // 左移光标
                edit->selectStart = edit->selectEnd = prev;
            }
            edit_on_select_changed(edit);

            // edit->offset 偏移修正
            if (edit->selectEnd < edit->offset) {
                edit->offset = edit->selectEnd;
            }
        }
        else {
            // (edit->selectEnd == 0) 的情况下，没有按下 shift 键，取消当前选择
            if (!key_state(UI_KEY_SHIFT) && edit->selectEnd < edit->selectStart) {
                edit->selectStart = edit->selectEnd;
                edit_on_select_changed(edit);
            }
        }
        break;

    case UI_KEY_RIGHT:
        if (edit->selectEnd < edit->length) { // 右移 selend
            int next = utf8_next(reinterpret_cast<const unsigned char*>(edit->buffer), edit->selectEnd, edit->length);
            if (key_state(UI_KEY_SHIFT)) {
                edit->selectEnd = next;
            }
            else { // 右移光标
                edit->selectStart = edit->selectEnd = next;
            }
            edit_on_select_changed(edit);

            // edit->offset 偏移修正
            int size = edit->selectEnd - edit->offset;
            while (text_width_utf8(reinterpret_cast<const unsigned char*>(edit->buffer) + edit->offset, size) > edit->bounds.w) {
                next = utf8_next(reinterpret_cast<const unsigned char*>(edit->buffer), edit->offset, edit->length);
                size -= next - edit->offset;
                edit->offset = next;
            }
        }
        else {
            // (edit->selectEnd == edit.length - 1) 的情况下，没有按下 shift 键，取消当前选择
            if (!key_state(UI_KEY_SHIFT) && edit->selectStart < edit->selectEnd) {
                edit->selectStart = edit->selectEnd;
                edit_on_select_changed(edit);
            }
        }
        break;

    case UI_KEY_HOME:
        if (key_state(UI_KEY_SHIFT)) {
            edit->selectEnd = 0;
        }
        else {
            edit->selectStart = edit->selectEnd = 0;
        }
        edit->offset = 0;
        edit_on_select_changed(edit);
        break;

    case UI_KEY_END:
        if (key_state(UI_KEY_SHIFT)) {
            edit->selectEnd = edit->length;
        }
        else {
            edit->selectStart = edit->selectEnd = edit->length;
        }
        edit_on_select_changed(edit);

        // 从尾部向前回退，计算 edit->offset 使得尾部内容恰好填满宽度
        if (text_width_utf8(reinterpret_cast<const unsigned char*>(edit->buffer), edit->length) > edit->bounds.w) {
            int pos     = edit->length;
            float width = 0.0f;
            while (pos > 0) {
                int prev = utf8_prev(reinterpret_cast<const unsigned char*>(edit->buffer), pos);
                uint32_t cp;
                utf8_decode(reinterpret_cast<const unsigned char*>(edit->buffer) + prev, edit->length, &cp, nullptr);
                width += char_width(static_cast<int>(cp));
                if (width > edit->bounds.w) {
                    edit->offset = pos;
                    break;
                }
                pos = prev;
            }
        }
        break;

    case UI_KEY_BACKSPACE:
        return edit_delete(edit, true);
    case UI_KEY_DELETE:
        return edit_delete(edit, false);
    default:
        break;
    }
    return false;
}

// 处理字符输入
bool edit_input(uiEdit_u8* edit, int ch)
{
    if (ch == 0)
        return false;

    // 编码为 UTF-8
    unsigned char enc[4];
    int encLen = utf8_encode(static_cast<uint32_t>(ch), enc, nullptr);

    if (edit->selectStart == edit->selectEnd) {
        // 插入
        if (edit->length + encLen < edit->bufferSize) {
            if (edit->selectEnd < edit->length) {
                size_t size = edit->length - edit->selectEnd;
                memmove(edit->buffer + edit->selectEnd + encLen, edit->buffer + edit->selectEnd, size);
            }
            memcpy(edit->buffer + edit->selectEnd, enc, encLen);
            edit->length += encLen;
            edit->buffer[edit->length] = '\0';
            edit->selectEnd += encLen;
            edit->selectStart = edit->selectEnd;

            edit_on_changed(edit);
        }
        else {
            return false;
        }
    }
    else {
        // 替换选择范围
        const int start = std::min(edit->selectStart, edit->selectEnd);
        const int end   = std::max(edit->selectStart, edit->selectEnd);
        const int tail  = edit->length - end;

        memmove(edit->buffer + start + encLen, edit->buffer + end, tail);
        memcpy(edit->buffer + start, enc, encLen);

        edit->length += encLen - (end - start);
        edit->buffer[edit->length] = '\0';

        edit->selectEnd = edit->selectStart = start + encLen;

        edit_on_changed(edit);
    }

    // edit->offset 偏移修正
    // 计算从 offset 到 selectEnd 的显示宽度
    float w = text_width_utf8(reinterpret_cast<const unsigned char*>(edit->buffer) + edit->offset, edit->selectEnd - edit->offset);
    while (w > edit->bounds.w && edit->offset < edit->selectEnd) {
        uint32_t cp;
        int n = (int) utf8_decode(reinterpret_cast<const unsigned char*>(edit->buffer) + edit->offset, edit->length, &cp, nullptr);
        w -= char_width(static_cast<int>(cp));
        // edit->offset = utf8_next(reinterpret_cast<const unsigned char*>(edit->buffer), edit->offset, edit->length);
        edit->offset += n;
    }

    return true;
}

// 绘制编辑框
static void draw_edit(uiCommandList* cmdlist, uiEdit_u8* edit, bool focused)
{
    uiColor background = style()->background[UI_COLOR_NORMAL];
    uiColor foreground = select_color(style()->widgetForeground, is_hovered(edit), false);
    draw_frame(cmdlist, edit, foreground, background);

    // 剪裁区域
    uiRect rect = edit->bounds.shrink(style()->framePadding);

    // 剪裁文字区域
    push_clip(rect, true);

    // 文字起始坐标
    const float x = rect.x;
    const float y = rect.y;
    const float h = char_height();

    // 选择范围（字节偏移）
    int selStart = std::min(edit->selectStart, edit->selectEnd);
    int selEnd   = std::max(edit->selectStart, edit->selectEnd);

    // 遍历显示范围内所有码点
    int pos         = edit->offset;
    float posW      = 0.0f;
    float selStartX = 0.0f;
    float selEndX   = 0.0f;

    for (; pos < edit->length && posW < edit->bounds.w;) {
        if (pos == selStart) {
            selStartX = posW;
        }
        if (pos == selEnd) {
            selEndX = posW;
        }

        uint32_t cp;
        int n = (int) utf8_decode(reinterpret_cast<const unsigned char*>(edit->buffer) + pos, edit->length, &cp, nullptr);
        posW += char_width(static_cast<int>(cp));
        pos += n;
    }

    // 边界在末尾的情况
    if (selStart == edit->length)
        selStartX = posW;
    if (selEnd == edit->length)
        selEndX = posW;

    // 绘制选择背景
    if (selStartX >= 0.0f && selEndX >= 0.0f && selEndX > selStartX) {
        cmdlist->set_fill(style()->background[UI_COLOR_HIGHLIGHT]);
        cmdlist->fill_rect(x + selStartX, y, selEndX - selStartX, h);
    }

    // 绘制字符串
    // cmdlist->set_text(style()->foreground[UI_COLOR_NORMAL]);
    // cmdlist->draw_string(x, y, edit->buffer + edit->offset, pos - edit->offset);

    if (edit->offset < selStart) {
        cmdlist->set_text(style()->foreground[UI_COLOR_NORMAL]);
        cmdlist->draw_string(x, y, edit->buffer + edit->offset, selStart - edit->offset);
    }
    if (selStart < selEnd) {
        cmdlist->set_text(style()->foreground[UI_COLOR_HIGHLIGHT]);
        cmdlist->draw_string(x + selStartX, y, edit->buffer + edit->offset + selStart, selEnd - selStart);
    }
    if (selEnd < pos) {
        cmdlist->set_text(style()->foreground[UI_COLOR_NORMAL]);
        cmdlist->draw_string(x + selEndX, y, edit->buffer + edit->offset + selEnd, pos - selEnd);
    }

    // 绘制光标
    if (focused) {
        clock_t t = clock() % clock_t(CLOCKS_PER_SEC);
        if (t < clock_t(CLOCKS_PER_SEC) / 2) {
            cmdlist->set_stroke(colors::black);
            cmdlist->draw_line(x + selEndX, y, x + selEndX, y + h);
        }
    }

    // 弹出剪裁区域
    pop_clip();
}

// 编辑框部件
bool edit(char* buffer, size_t size)
{
    // 用缓冲地址做 ID
    uiID id = ui_hash(&buffer, sizeof(void*));

    uiEdit_u8* edit = static_cast<uiEdit_u8*>(find_child_by_id(id));
    if (!edit) {
        edit              = new uiEdit_u8(id, layout()->container);
        edit->buffer      = buffer;
        edit->bufferSize  = (int) size;
        edit->length      = (int) utf8_strlen(reinterpret_cast<const unsigned char*>(buffer));
        edit->offset      = 0;
        edit->selectStart = edit->selectEnd = edit->length;
        layout()->owner->widgets.push_back(edit);
        edit_on_changed(edit);
    }
    else {
        // buffer 可能被修改，重新同步
        int len = (int) utf8_strlen(reinterpret_cast<const unsigned char*>(buffer));
        if (len != edit->length) {
            edit->length      = len;
            edit->offset      = 0;
            edit->selectStart = edit->selectEnd = edit->length;
            edit_on_changed(edit);
        }
        edit->buffer     = buffer;
        edit->bufferSize = (int) size;
    }

    float cy     = char_height() + style()->framePadding.y * 2.0f;
    edit->bounds = allocate_widget_rect(-1.0f, cy);

    bool focused = is_focus(edit);

    uiFlags flags = check_widget_mouse_event(edit);
    if (check_flags(flags, UI_FLAGS_CLICKED)) {
        if (!focused) {
            set_focus(edit);
            focused = true;
        }

        // 鼠标点击定位：按显示宽度遍历码点，找到最近的码点边界
        float mx = context()->mouse.x - edit->bounds.x - style()->framePadding.x;

        int hit   = edit->offset; // 命中位置（字节偏移）
        float pos = 0.0f;
        bool done = false;

        for (int i = edit->offset; i < edit->length;) {
            uint32_t cp;
            int n   = (int) utf8_decode(reinterpret_cast<const unsigned char*>(edit->buffer) + i, edit->length, &cp, nullptr);
            float w = char_width(static_cast<int>(cp));
            if (mx < pos + w * 0.5f) {
                hit  = i;
                done = true;
                break;
            }
            pos += w;
            i += n;
        }
        if (!done)
            hit = edit->length;

        edit->selectStart = edit->selectEnd = hit;
    }

    bool changed = false;
    if (focused) {
        if (context()->key) {
            changed        = edit_keyinput(edit, context()->key);
            context()->key = 0;
        }
        if (context()->ch) { // ch 须为完整解码后的码点
            changed       = edit_input(edit, (unsigned) context()->ch);
            context()->ch = 0;
        }
    }

    draw_edit(window_cmdlist(), edit, focused);
    return changed;
}

} // namespace minimgui
