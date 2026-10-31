#include "graphics.h"

bool vsync   = true;             // 是否垂直同步
vec2i screen = vec2i(1280, 720); // 窗口大小
vec2f mouse;                     // 鼠标位置
vec2f lastpos;
int mouse_button = -1;

// 为了简单，这里只用了 gl1.1 的函数，只供参考

// 准备绘制，设置顶点格式
void vtx_begin(const vertex* vs)
{
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), vs);
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), &vs->texcoord.x);
    glColorPointer(4, GL_FLOAT, sizeof(vertex), &vs->color.r);
    glNormalPointer(GL_FLOAT, sizeof(vertex), &vs->normal.x);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
}

// 结束绘制，停用顶点格式设置
void vtx_end(const vertex* vs)
{
    (void) vs;
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

// 绘制顶点列表
int draw_arrays(int shape, const vertex* vs, size_t pos, size_t size)
{
    vtx_begin(vs);
    glDrawArrays(shape, GLint(pos), GLsizei(size));
    vtx_end(vs);
    return 0;
}

// 绘制索引的顶点列表
int draw_elements(int shape, const vertex* vs, const uint32_t* indices, size_t size)
{
    vtx_begin(vs);
    glDrawElements(shape, GLsizei(size), GL_UNSIGNED_INT, indices);
    vtx_end(vs);
    return 0;
}

// 绘制立方体
void draw_cube(float x1, float y1, float z1, float x2, float y2, float z2)
{
    const vec4f& color = vec4f(1.0f);
    vertex vs[]        = {
        // front
        vertex(vec3f(x2, y2, z1), vec3f(0.0f, 0.0f, -1.0f), color, vec2f(0.0f, 0.0f)),
        vertex(vec3f(x2, y1, z1), vec3f(0.0f, 0.0f, -1.0f), color, vec2f(0.0f, 1.0f)),
        vertex(vec3f(x1, y1, z1), vec3f(0.0f, 0.0f, -1.0f), color, vec2f(1.0f, 1.0f)),
        vertex(vec3f(x1, y2, z1), vec3f(0.0f, 0.0f, -1.0f), color, vec2f(1.0f, 0.0f)),

        // back
        vertex(vec3f(x1, y2, z2), vec3f(0.0f, 0.0f, 1.0f), color, vec2f(0.0f, 0.0f)),
        vertex(vec3f(x1, y1, z2), vec3f(0.0f, 0.0f, 1.0f), color, vec2f(0.0f, 1.0f)),
        vertex(vec3f(x2, y1, z2), vec3f(0.0f, 0.0f, 1.0f), color, vec2f(1.0f, 1.0f)),
        vertex(vec3f(x2, y2, z2), vec3f(0.0f, 0.0f, 1.0f), color, vec2f(1.0f, 0.0f)),

        // left
        vertex(vec3f(x1, y2, z1), vec3f(-1.0f, 0.0f, 0.0f), color, vec2f(0.0f, 0.0f)),
        vertex(vec3f(x1, y1, z1), vec3f(-1.0f, 0.0f, 0.0f), color, vec2f(0.0f, 1.0f)),
        vertex(vec3f(x1, y1, z2), vec3f(-1.0f, 0.0f, 0.0f), color, vec2f(1.0f, 1.0f)),
        vertex(vec3f(x1, y2, z2), vec3f(-1.0f, 0.0f, 0.0f), color, vec2f(1.0f, 0.0f)),

        // right
        vertex(vec3f(x2, y2, z2), vec3f(1.0f, 0.0f, 0.0f), color, vec2f(0.0f, 0.0f)),
        vertex(vec3f(x2, y1, z2), vec3f(1.0f, 0.0f, 0.0f), color, vec2f(0.0f, 1.0f)),
        vertex(vec3f(x2, y1, z1), vec3f(1.0f, 0.0f, 0.0f), color, vec2f(1.0f, 1.0f)),
        vertex(vec3f(x2, y2, z1), vec3f(1.0f, 0.0f, 0.0f), color, vec2f(1.0f, 0.0f)),

        // top
        vertex(vec3f(x1, y2, z1), vec3f(0.0f, 1.0f, 0.0f), color, vec2f(0.0f, 0.0f)),
        vertex(vec3f(x1, y2, z2), vec3f(0.0f, 1.0f, 0.0f), color, vec2f(0.0f, 1.0f)),
        vertex(vec3f(x2, y2, z2), vec3f(0.0f, 1.0f, 0.0f), color, vec2f(1.0f, 1.0f)),
        vertex(vec3f(x2, y2, z1), vec3f(0.0f, 1.0f, 0.0f), color, vec2f(1.0f, 0.0f)),

        // bottom
        vertex(vec3f(x1, y1, z2), vec3f(0.0f, -1.0f, 0.0f), color, vec2f(0.0f, 0.0f)),
        vertex(vec3f(x1, y1, z1), vec3f(0.0f, -1.0f, 0.0f), color, vec2f(0.0f, 1.0f)),
        vertex(vec3f(x2, y1, z1), vec3f(0.0f, -1.0f, 0.0f), color, vec2f(1.0f, 1.0f)),
        vertex(vec3f(x2, y1, z2), vec3f(0.0f, -1.0f, 0.0f), color, vec2f(1.0f, 0.0f)),
    };

    // clang-format off
    static uint32_t indices[36] = {
        0,1,2,0,2,3,  4,5,6,4,6,7,  8,9,10, 8,10,11,  12,13,14,12,14,15,  16,17,18,16,18,19,  20,21,22,20,22,23
    };
    // clang-format on

    draw_elements(GL_TRIANGLES, vs, indices, 36u);
}

