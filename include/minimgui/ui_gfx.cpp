#include "ui_gfx.h"
#include "ui_user.h"

namespace minimgui {

// 绘制检查矩形
void draw_check_rect(uiCommandList* cmdlist, float x, float y, float w, float h)
{
    cmdlist->draw_rect(x, y, w, h);
    cmdlist->draw_line(x, y, x + w, y + h);
    cmdlist->draw_line(x + w, y, x, y + h);
}

void draw_check_rect(uiCommandList* cmdlist, const uiRect& rect)
{
    draw_check_rect(cmdlist, rect.x, rect.y, rect.w, rect.h);
}

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

// 绘制折叠图标
void draw_collapse_icon(uiCommandList* cmdlist, float x, float y, float size, float thickness, bool collapsed)
{
    const float t = thickness * 0.5f;

    y += size * 0.15f;

    cmdlist->fill_triangle(x, y - t, x, y + t, x + size, y + t);
    cmdlist->fill_triangle(x, y - t, x + size, y + t, x + size, y - t);

    const float lt   = t * 0.70710678f; // 1 / sqrt(2)
    const float ht   = t * 1.41421356f; // sqrt(2)
    const float half = size * 0.5f;

    // 计算 X 一角的两个顶点
    float a1x = half - lt - lt;
    float a1y = half - lt + lt;
    float a2x = half - lt + lt;
    float a2y = half - lt - lt;

    y += size * 0.2f;

    if (collapsed) {
        float ox = x + half;
        float oy = y + thickness * 0.5f;

        // 左下
        float x5 = ox - a1x;
        float y5 = oy + a1y;
        float x6 = ox - a2x;
        float y6 = oy + a2y;

        cmdlist->fill_triangle(x6, y6, x5, y5, ox, oy + ht);
        cmdlist->fill_triangle(x6, y6, ox, oy + ht, ox, oy - ht);

        // 右下
        float x7 = ox + a2x;
        float y7 = oy + a2y;
        float x8 = ox + a1x;
        float y8 = oy + a1y;

        cmdlist->fill_triangle(x8, y8, x7, y7, ox, oy - ht);
        cmdlist->fill_triangle(x8, y8, ox, oy - ht, ox, oy + ht);
    }
    else {
        float ox = x + half;
        float oy = y + half;

        // 右上
        float x1 = ox + a1x;
        float y1 = oy - a1y;
        float x2 = ox + a2x;
        float y2 = oy - a2y;

        cmdlist->fill_triangle(x2, y2, x1, y1, ox, oy - ht);
        cmdlist->fill_triangle(x2, y2, ox, oy - ht, ox, oy + ht);

        // 左上
        float x3 = ox - a1x;
        float y3 = oy - a1y;
        float x4 = ox - a2x;
        float y4 = oy - a2y;

        cmdlist->fill_triangle(x3, y3, x4, y4, ox, oy + ht);
        cmdlist->fill_triangle(x3, y3, ox, oy + ht, ox, oy - ht);
    }
}

// 绘制关闭图标
/*
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
*/

/* 绘制关闭图标
 *  a   d
 *    x
 *  b   c
 */
void draw_close_icon(uiCommandList* cmdlist, float x, float y, float size, float thickness)
{
    const float lt   = thickness * 0.5f * 0.70710678f; // 1 / sqrt(2)
    const float ht   = thickness * 0.5f * 1.41421356f; // sqrt(2)
    const float half = size * 0.5f;

    float ox = x + half;
    float oy = y + half;

    // 计算 X 一角的两个顶点
    float ax = half - lt;
    float ay = half - lt;

    float a1x = ax - lt;
    float a1y = ay + lt; // 靠左上的点
    float a2x = ax + lt;
    float a2y = ay - lt; // 靠右下的点

    // 右上
    float x1 = ox + a1x;
    float y1 = oy - a1y;
    float x2 = ox + a2x;
    float y2 = oy - a2y;

    // cmdlist->fill_triangle(x2, y2, x1, y1, ox, oy - d);
    // cmdlist->fill_triangle(x2, y2, ox, oy - d, ox + d, oy);

    // 左上
    float x3 = ox - a2x;
    float y3 = oy - a2y;
    float x4 = ox - a1x;
    float y4 = oy - a1y;

    cmdlist->fill_triangle(x4, y4, x3, y3, ox - ht, oy);
    cmdlist->fill_triangle(x4, y4, ox - ht, oy, ox, oy - ht);

    // 左下
    float x5 = ox - a1x;
    float y5 = oy + a1y;
    float x6 = ox - a2x;
    float y6 = oy + a2y;

    // cmdlist->fill_triangle(x6, y6, x5, y5, ox, oy + ht);
    // cmdlist->fill_triangle(x6, y6, ox, oy + ht, ox - ht, oy);

    // 右下
    float x7 = ox + a2x;
    float y7 = oy + a2y;
    float x8 = ox + a1x;
    float y8 = oy + a1y;

    cmdlist->fill_triangle(x8, y8, x7, y7, ox + ht, oy);
    cmdlist->fill_triangle(x8, y8, ox + ht, oy, ox, oy + ht);

    // 中心菱形
    // cmdlist->fill_triangle(ox, oy - ht, ox - ht, oy, ox, oy + ht);
    // cmdlist->fill_triangle(ox, oy - ht, ox, oy + ht, ox + ht, oy);

    cmdlist->fill_triangle(x2, y2, x1, y1, x6, y6);
    cmdlist->fill_triangle(x2, y2, x6, y6, x5, y5);
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

/*   ___________________
 *  |                   |
 *  |                c1 |
 *  |                 c2|
 *  |  a1               |
 *  | a2                |
 *  |       b1          |
 *  |       b2          |
 *  |___________________|
 */
// 绘制对勾
void draw_checkmark(uiCommandList* cmdlist, float x, float y, float size, float thickness)
{
    const float t = thickness * 0.5f * 0.70710678f; // 1 / sqrt(2)
    const float d = thickness * 0.5f * 1.41421356f; // sqrt(2)

    float ax = size * 0.0f + t;
    float ay = size * 0.4f + t;

    float bx = size * 0.4f;
    float by = size * 0.8f;

    float cx = size * 1.0f - t;
    float cy = size * 0.2f + t;

    float a1x = x + ax + t;
    float a1y = y + ay - t;
    float a2x = x + ax - t;
    float a2y = y + ay + t;

    float b1x = x + bx;
    float b1y = y + by - d;
    float b2x = x + bx;
    float b2y = y + by + d;

    float c1x = x + cx - t;
    float c1y = y + cy - t;
    float c2x = x + cx + t;
    float c2y = y + cy + t;

    cmdlist->fill_triangle(a1x, a1y, a2x, a2y, b2x, b2y);
    cmdlist->fill_triangle(a1x, a1y, b2x, b2y, b1x, b1y);
    cmdlist->fill_triangle(b1x, b1y, b2x, b2y, c2x, c2y);
    cmdlist->fill_triangle(b1x, b1y, c2x, c2y, c1x, c1y);
}

} // namespace minimgui
