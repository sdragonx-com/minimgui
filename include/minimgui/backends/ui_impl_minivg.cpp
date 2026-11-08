#include "ui_impl_minivg.h"
#include "../ui_gfx.h"

#include <glm/glm.hpp>
#include <minivg.hpp>

namespace minimgui {

//------------------------------------------------------------------------------

// 初始化界面环境
void on_begin_context(void* instance)
{
    (void) instance;

    font_name(L"Microsoft YaHei"); // 字体名称
    font_size(11);                 // 字体大小
}

// 结束界面环境
void on_end_context(void* instance)
{
    (void) instance;
}

// 初始化绘制
void on_begin_draw(void* instance)
{
    (void) instance;
}

// 结束绘制
void on_end_draw(void* instance)
{
    (void) instance;
}

//------------------------------------------------------------------------------

// 绘制命令列表
void draw_cmd(const uiCommandList& cmdlist, void* instance)
{
    (void) instance;

    // Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    // format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces); // 防止忽略末尾空格

    std::wstring wstr;

    const uiVec2* points = cmdlist.point_data();
    for (size_t cmdID = 0; cmdID < cmdlist.command_count(); ++cmdID) {
        const uiCommandList::command& cmd = cmdlist.commands(cmdID);
        switch (cmd.inst) {
        case uiCommandList::STROKE:
            pen_color(RedBlueSwap(cmd.stroke.color));
            pen_width(cmd.stroke.width);
            break;
        case uiCommandList::FILL_COLOR:
            fill_color(RedBlueSwap(cmd.fill.color));
            break;
        case uiCommandList::FILL_IMAGE:
            break;
        case uiCommandList::LINE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                draw_line(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                draw_rect(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                fill_rect(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::ROUND_RECT:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                draw_roundrect(
                    points[i - 2].x, points[i - 2].y,
                    points[i - 1].x, points[i - 1].y,
                    points[i - 0].x, points[i - 0].y
                );
            }
            break;
        case uiCommandList::ROUND_RECT | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                fill_roundrect(
                    points[i - 2].x, points[i - 2].y,
                    points[i - 1].x, points[i - 1].y,
                    points[i - 0].x, points[i - 0].y
                );
            }
            break;
        case uiCommandList::ELLIPSE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                draw_ellipse_r(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::ELLIPSE | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                fill_ellipse_r(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::TRIANGLE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                draw_polygon((vec2f*) &points[i - 2], 3);
            }
            break;
        case uiCommandList::TRIANGLE | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                fill_polygon((vec2f*) &points[i - 2], 3);
            }
            break;
        case uiCommandList::POLYGON:
            points = cmdlist.point_data() + cmd.geom.index;
            draw_polygon((vec2f*) points, cmd.geom.count);
            break;
        case uiCommandList::POLYGON | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            fill_polygon((vec2f*) points, cmd.geom.count);
            break;
        case uiCommandList::STRING: // 绘制字符串
        {
            points              = cmdlist.point_data() + cmd.text.index;
            const uiString& str = cmdlist.strings(cmd.text.string);

            text_color(RedBlueSwap(cmd.text.color));

            #if defined(MINIMGUI_UTF8_ENCODING)
            utf8_to_unicode(wstr, str.c_str(), str.length());
            #else
            ansi_to_unicode(wstr, str.c_str(), str.length());
            #endif

            textout(points[0].x, points[0].y, wstr.c_str(), wstr.length());
            break;
        }
        case uiCommandList::IMAGE:
            points = cmdlist.point_data() + cmd.image.index;
            {
                vgImage* pImage = (vgImage*) cmdlist.images(cmd.image.id);
                drawimage(pImage, points[0].x, points[0].y, points[1].x, points[1].y);
            }
            break;
        case uiCommandList::PUSH_CLIP_RESET:
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
            minivg::push_cliprect(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h, false);
            break;
        case uiCommandList::PUSH_CLIP_INTERSECT:
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
            minivg::push_cliprect(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h, true);
            break;
        case uiCommandList::POP_CLIP:
            // printf("pop clip\n");
            minivg::pop_cliprect();
            break;
        default:
            break;
        }
    }
}

// 返回字符宽度
float char_width(int ch)
{
    wchar_t w = ch;
    return minivg::textwidth(unistring(&w, 1));
}

// 返回字符高度
float char_height()
{
    static const wchar_t ch = L'A';
    return minivg::textheight(unistring(&ch, 1));
}

// 返回文字宽度
float text_width(const char* str, size_t length)
{
    if (length == SIZE_MAX)
        length = strlen(str);
    std::wstring wstr;
    #if defined(MINIMGUI_UTF8_ENCODING)
    utf8_to_unicode(wstr, str, length);
    #else
    ansi_to_unicode(wstr, str, length);
    #endif
    return minivg::textwidth(wstr);
}

float text_width(const wchar_t* str, size_t length)
{
    if (length == SIZE_MAX)
        length = wcslen(str);
    return minivg::textwidth(unistring(str, length));
}

// 创建图片 (RGBA)
uiHandle create_image(int width, int height)
{
    return (uiHandle) minivg::newimage(width, height);
}

// 加载图片
uiHandle load_image(const char* filename)
{
    if (!filename) {
        return 0;
    }

    std::wstring wstr;
    #if defined(MINIMGUI_UTF8_ENCODING)
    utf8_to_unicode(wstr, filename, strlen(filename));
    #else
    ansi_to_unicode(wstr, filename, strlen(filename));
    #endif

    vgImage* pImage = minivg::loadimage(wstr);

    return (uiHandle) pImage;
}

uiHandle load_image(const wchar_t* filename)
{
    if (!filename) {
        return 0;
    }

    vgImage* pImage = minivg::loadimage(filename);

    return (uiHandle) pImage;
}

// 删除图片
void delete_image(uiHandle image)
{
    vgImage* pImage = (vgImage*) image;
    minivg::freeimage(pImage);
}

} // namespace minimgui
