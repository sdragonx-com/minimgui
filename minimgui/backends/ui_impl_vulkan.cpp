#include "ui_impl_vulkan.h"
#include "../ui_graph.h"

namespace minimgui {

// 初始化上下文设备
void init_context(void* instance)
{
    (void) instance;
}

// 重置上下文设备
void reset_context(void* instance)
{
    (void) instance;
}


// 绘制命令列表
void draw_cmd(const uiCommandList& cmdlist, void* instance)
{
    const vec2f* points;
    for (size_t cmdID = 0; cmdID < cmdlist.command_count(); ++cmdID) {
        const uiCommandList::command& cmd = cmdlist.commands(cmdID);
        switch (cmd.inst) {
        case uiCommandList::STROKE:
            // set_pen(cmd.stroke.color);
            break;
        case uiCommandList::FILL_COLOR:
            // set_brush(cmd.fill.color);
            break;
        case uiCommandList::FILL_IMAGE:
            break;
        case uiCommandList::LINE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                // draw_line(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                // draw_rect(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                // fill_rect(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::ROUND_RECT:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                // draw_round_rect(
                //     points[i - 2].x, points[i - 2].y,
                //     points[i - 1].x, points[i - 1].y,
                //     points[i - 0].x, points[i - 0].y
                // );
            }
            break;
        case uiCommandList::ROUND_RECT | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                // fill_round_rect(
                //     points[i - 2].x, points[i - 2].y,
                //     points[i - 1].x, points[i - 1].y,
                //     points[i - 0].x, points[i - 0].y
                // );
            }
            break;
        case uiCommandList::ELLIPSE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                // draw_ellipse(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::ELLIPSE | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                // fill_ellipse(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::TRIANGLE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                // draw_triangle(&points[i - 2], 3);
            }
            break;
        case uiCommandList::TRIANGLE | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i +=3) {
                // fill_triangle(&points[i - 2], 3);
            }
            break;
        case uiCommandList::POLYGON:
            points = cmdlist.point_data() + cmd.geom.index;
            // draw_polygon(points, cmd.geom.count);
            break;
        case uiCommandList::POLYGON | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            // fill_polygon(points, cmd.geom.count);
            break;
        case uiCommandList::STRING: // 绘制字符串
        {
            // set_text_color(cmd.text.color);
            // draw_string(cmdlist.strings(cmd.index));
            break;
        }
        case uiCommandList::PUSH_CLIP:
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", points[0].x, points[0].y, points[1].x, points[1].y);
            // rect = rect2f(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.y);
            break;
        case uiCommandList::POP_CLIP:
            // printf("pop clip\n");
            break;
        default:
            break;
        }
    }
}

// 返回文字宽度
float text_width(const uiChar* text, size_t length)
{
    return 0.0f;
}

// 返回文字高度
float text_height()
{
    return 0.0f;
}

// 统计字符宽度列表
int count_char_widths(std::vector<float>& widths, const uiChar* text, size_t length)
{
    return 0;
}

} // namespace minimgui
