#include "ui_gfx.h"
#include "ui_user.h"

namespace minimgui {

// 上下箭头
void draw_updown_arrow(uiCommandList* cmdlist, float x, float y, float w, float h, float scale, bool isUp)
{
    float size  = char_height() * scale;
    float space = 3.0f;
    x += size * 0.25f;
    y += (h - size) * 0.5f;
    w = h = size;

    if (isUp) {
        for (int i = 0; i < 3; ++i) {
            cmdlist->draw_line(x, y + h * 0.5f + i * space, x + w * 0.5f, y + i * space);
            cmdlist->draw_line(x + w * 0.5f, y + i * space, x + w, y + h * 0.5f + i * space);
        }
    }
    else {
        for (int i = 0; i < 3; ++i) {
            cmdlist->draw_line(x, y + i * space, x + w * 0.5f, y + h * 0.5f + i * space);
            cmdlist->draw_line(x + w * 0.5f, y + h * 0.5f + i * space, x + w, y + i * space);
        }
    }
}

// 填充三角形箭头
void fill_triangle_arrow(uiCommandList* cmdlist, float x, float y, float w, float h, uiDirection dir)
{
    const float cx = x + w * 0.5f;
    const float cy = y + h * 0.5f;

    switch (dir) {
    case UI_LEFT: // 左箭头
        cmdlist->fill_triangle(
            x + w * 0.75f, y,
            cx, cy,
            x, cy
        );
        cmdlist->fill_triangle(
            cx, cy,
            x + w * 0.75f, y + h,
            x, cy
        );
        break;
    case UI_RIGHT: // 右箭头
        cmdlist->fill_triangle(
            x + w * 0.25f, y,
            cx, cy,
            x + w, cy
        );
        cmdlist->fill_triangle(
            cx, cy,
            x + w * 0.25f, y + h,
            x + w, cy
        );
        break;
    case UI_UP: // 上箭头
        cmdlist->fill_triangle(
            x, y + h * 0.75f,
            cx, y,
            cx, cy
        );
        cmdlist->fill_triangle(
            cx, cy,
            cx, y,
            x + w, y + h * 0.75f
        );
        break;
    case UI_DOWN: // 下箭头
        cmdlist->fill_triangle(
            x, y + h * 0.25f,
            cx, y + h,
            cx, cy
        );
        cmdlist->fill_triangle(
            cx, cy,
            cx, y + h,
            x + w, y + h * 0.25f
        );
        break;
    default:
        break;
    }
}

// 绘制关闭图标
void draw_close_icon(uiCommandList* cmdlist, float x, float y, float w, float h)
{
    float size = char_height() * 0.8f;
    x += size * 0.25f;
    y += (h - size) * 0.5f;
    w = h = size;

    // cmdlist->draw_rect(x, y, w, h);

    float x2 = x + w - size * 0.2f;
    float y2 = y + h - size * 0.2f;
    x += size * 0.2f;
    y += size * 0.2f;
    cmdlist->draw_line(x, y, x2, y2);
    cmdlist->draw_line(x2, y, x, y2);
}

} // namespace minimgui
