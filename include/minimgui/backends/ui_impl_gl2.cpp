#include "ui_impl_gl2.h"
#include "../ui_gfx.h"

#include <cgl/graphics/gl/glfont_stb.hpp>
#include <cgl/graphics/gl/glfont_freetype.hpp>

namespace minimgui {

static cgl::graphics::glfont _m_font;

// 初始化 OpenGL 实例环境
void init_gl2(void* instance)
{
    // _m_font.load("MaokenZhuyuanTi.ttf", 16.0f);
    _m_font.load("HarmonyOS_Sans_SC_Regular.ttf", 16.0f);
}

// 释放 OpenGL 实例环境
void dispose_gl2(void* instance)
{
    _m_font.dispose();
}

// 绘制顶点列表
static void draw_arrays(GLenum primitive, const uiVec2* vertices, size_t pos, size_t size)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    // glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(uiVec2), vertices);
    // glTexCoordPointer(2, GL_FLOAT, sizeof(uiVec2), ...);

    glDrawArrays(primitive, GLint(pos), GLsizei(size));

    glDisableClientState(GL_VERTEX_ARRAY);
    // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

// 绘制顶点列表
static void draw_elements(
    GLenum primitive,
    const uiVertex* vertices, size_t vertexSize,
    const uint32_t* indices, size_t indexSize
)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(uiVertex), &vertices->position);
    glTexCoordPointer(2, GL_FLOAT, sizeof(uiVertex), &vertices->texcoord);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(uiVertex), &vertices->color);

    glDrawElements(primitive, GLsizei(indexSize), GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

static void draw_rect(float x, float y, float w, float h)
{
    const uiVec2 vs[] = {
        uiVec2(x, y),
        uiVec2(x, y + h),
        uiVec2(x + w, y + h),
        uiVec2(x + w, y),
    };
    draw_arrays(GL_LINE_LOOP, vs, 0, 4);
}

static void push_rect(std::vector<uiVec2>& vertices, float x, float y, float w, float h)
{
    vertices.push_back(uiVec2(x, y));
    vertices.push_back(uiVec2(x, y + h));
    vertices.push_back(uiVec2(x + w, y + h));
    vertices.push_back(uiVec2(x + w, y));
}

// 初始化界面环境
void on_begin_context(void* instance)
{
    (void) instance;
}

// 结束界面环境
void on_end_context(void* instance)
{
    (void) instance;
}

// 初始化绘制
void on_begin_draw(void* instance)
{
    GLFWwindow* window = (GLFWwindow*) instance;

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    // 重置状态
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 颜色
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // 线宽
    glLineWidth(1.0f);

    // 开启 2D 纹理
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 开启混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 关闭背面剔除
    // glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);

    // 关闭深度
    glDisable(GL_DEPTH_TEST);

    // 开启剪裁
    glEnable(GL_SCISSOR_TEST);
}

// 结束绘制
void on_end_draw(void* instance)
{
    // 还原状态
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // 关闭剪裁
    glDisable(GL_SCISSOR_TEST);
}

// 绘制渲染命令
void draw_cmd(const uiRenderList& cmdlist, void* instance)
{
    for (size_t cmdID = 0; cmdID < cmdlist.command_count(); ++cmdID) {
        const uiRenderList::command& cmd = cmdlist.commands(cmdID);
        switch (cmd.inst) {
        case uiRenderList::POINTS:
            draw_elements(
                GL_POINTS,
                cmdlist.vertex_data(), 0,
                cmdlist.index_data() + cmd.index, cmd.count
            );
            break;
        case uiRenderList::LINES:
            draw_elements(
                GL_LINES,
                cmdlist.vertex_data(), 0,
                cmdlist.index_data() + cmd.index, cmd.count
            );
            break;
        case uiRenderList::TRIANGLES:
            draw_elements(
                GL_TRIANGLES,
                cmdlist.vertex_data(), 0,
                cmdlist.index_data() + cmd.index, cmd.count
            );
            break;
        case uiRenderList::TEXTURE_2D:
            glBindTexture(GL_TEXTURE_2D, cmd.handle);
            break;
        case uiRenderList::CLIPBOX:
            glScissor((GLint) cmd.x, (GLint) cmd.y, (GLsizei) cmdlist.commands(cmdID + 1).x, (GLsizei) cmdlist.commands(cmdID + 1).y);
            break;
        default:
            break;
        }
    }
}

// 绘制命令列表
void draw_cmd(const uiCommandList& cmdlist, void* instance)
{
    GLFWwindow* window = (GLFWwindow*) instance;

    uiColor pen;
    // float penWidth = 1.0f; // todo...
    uiColor brush;
    uiRenderList renderer;
    std::vector<uiRect> clipStack;
    uiRect rect;

    const uiVec2* points;
    for (size_t cmdID = 0; cmdID < cmdlist.command_count(); ++cmdID) {
        const uiCommandList::command& cmd = cmdlist.commands(cmdID);
        switch (cmd.inst) {
        case uiCommandList::STROKE:
            pen = uiColor(
                RGBA_RED(cmd.stroke.color),
                RGBA_GREEN(cmd.stroke.color),
                RGBA_BLUE(cmd.stroke.color),
                RGBA_ALPHA(cmd.stroke.color)
            );
            break;
        case uiCommandList::FILL_COLOR:
            brush = uiColor(
                RGBA_RED(cmd.fill.color),
                RGBA_GREEN(cmd.fill.color),
                RGBA_BLUE(cmd.fill.color),
                RGBA_ALPHA(cmd.fill.color)
            );
            break;
        case uiCommandList::FILL_IMAGE:
            break;
        case uiCommandList::LINE:
            renderer.set_color(pen);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                renderer.draw_line(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT:
            renderer.set_color(pen);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                renderer.draw_rect(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT | uiCommandList::FILL_MODE:
            renderer.set_color(brush);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                renderer.fill_rect(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
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
            renderer.set_color(pen);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                renderer.draw_triangle(
                    points[i - 2].x, points[i - 2].y,
                    points[i - 1].x, points[i - 1].y,
                    points[i - 0].x, points[i - 0].y
                );
            }
            break;
        case uiCommandList::TRIANGLE | uiCommandList::FILL_MODE:
            renderer.set_color(brush);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                renderer.fill_triangle(
                    points[i - 2].x, points[i - 2].y,
                    points[i - 1].x, points[i - 1].y,
                    points[i - 0].x, points[i - 0].y
                );
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
            renderer.set_color(cmd.text.color);
            const uiString& str = cmdlist.strings(cmd.text.string);
            uiVec2 pos          = cmdlist.points(cmd.text.index);
            GLuint texID        = 0;
            for (size_t i = 0; i < str.length(); ++i) {
                uiChar ch                         = str[i];
                cgl::graphics::glfont::glyph icon = _m_font.get_char(ch);
                if (!icon) {
                    continue;
                }
                if (texID != icon->image.image) {
                    texID = icon->image.image;
                    renderer.set_texture(texID);
                }
                renderer.draw_image(
                    // icon->image.image,
                    pos.x + icon->data.bearingX,
                    pos.y + (_m_font.size() - icon->data.bearingY),
                    static_cast<float>(icon->image.width),
                    static_cast<float>(icon->image.height),
                    icon->image.u1, icon->image.v1, icon->image.u2, icon->image.v2
                );
                pos.x += icon->data.advanceX;
            }
            // 还原纹理
            if (texID != 0) {
                renderer.set_texture(GL_NONE);
            }
            break;
        }
        case uiCommandList::PUSH_CLIP_RESET: {
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", points[0].x, points[0].y, points[1].x, points[1].y);
            glGetFloatv(GL_SCISSOR_BOX, &rect.x);
            clipStack.push_back(rect);

            // renderer.set_color(colors::magenta);
            // renderer.draw_rect(rect.x, rect.y, rect.w, rect.h);

            int h;
            glfwGetFramebufferSize(window, nullptr, &h);
            rect = uiRect(cmd.clip.x, (float) h - (cmd.clip.y + cmd.clip.h), cmd.clip.w, cmd.clip.h);
            renderer.set_clipbox(rect.x, rect.y, rect.w, rect.h);

            // renderer.set_color(colors::yellow);
            // renderer.draw_rect(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
            break;
        }
        case uiCommandList::PUSH_CLIP_INTERSECT: {
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", points[0].x, points[0].y, points[1].x, points[1].y);
            glGetFloatv(GL_SCISSOR_BOX, &rect.x);
            clipStack.push_back(rect);

            // renderer.set_color(colors::magenta);
            // renderer.draw_rect(rect.x, rect.y, rect.w, rect.h);

            int h;
            glfwGetFramebufferSize(window, nullptr, &h);
            rect = rect & uiRect(cmd.clip.x, (float) h - (cmd.clip.y + cmd.clip.h), cmd.clip.w, cmd.clip.h);
            renderer.set_clipbox(rect.x, rect.y, rect.w, rect.h);

            // renderer.set_color(colors::yellow);
            // renderer.draw_rect(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
            break;
        }
        case uiCommandList::POP_CLIP: {
            // printf("pop clip\n");
            if (!clipStack.empty()) {
                rect = clipStack.back();
                clipStack.pop_back();
                // glScissor((GLint) rect.x, (GLint) rect.y, (GLsizei) rect.w, (GLsizei) rect.h);
                renderer.set_clipbox(rect.x, rect.y, rect.w, rect.h);
            }
            break;
        }
        default:
            break;
        }
    }

    // 绘制命令
    draw_cmd(renderer, instance);
}

// 返回字符宽度
float char_width(int ch)
{
    return _m_font.char_width(ch);
}

// 返回字符高度
float char_height()
{
    return _m_font.char_height() * 1.25f;
}

// 返回文字宽度
float text_width(const uiChar* text, size_t length)
{
    return _m_font.text_width(text, length);
}

} // namespace minimgui
