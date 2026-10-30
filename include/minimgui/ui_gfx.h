/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_graph.h

 2026-03-16 08:59:32

*/
#ifndef UI_GRAPH_H_20260316085932
#define UI_GRAPH_H_20260316085932

#include "ui_std.h"

namespace minimgui {

//
// 2D 绘图指令缓冲
//

class uiCommandList
{
public:
    typedef void* handle_type;

    enum COMMAND
    {
        NONE = 0x0000,       // 空指令

        CLEAR,               // 清屏

        TRANSFORM,           // 矩阵变换

        PUSH_CLIP_RESET,     // 压入剪裁区域，重新设置剪裁区域
        PUSH_CLIP_INTERSECT, // 压入剪裁区域，和之前的剪裁区域相交
        POP_CLIP,            // 弹出剪裁区域

        STROKE,              // 线条样式
        STROKE_DASH,         // 点画模式
        FILL_COLOR,          // 填充颜色
        FILL_PATTERN,        // 填充图案
        FILL_IMAGE,          // 填充图片

        MOVETO,              // 移动
        LINETO,              // 绘制直线

        POINT,               // 点
        LINE,                // 直线
        RECT,                // 矩形
        ROUND_RECT,          // 圆角矩形
        ELLIPSE,             // 椭圆
        TRIANGLE,            // 三角形
        POLYLINE,            // 折线
        POLYGON,             // 多边形
        BEZIER,              // 三次贝赛尔曲线
        CURVE,               // 样条曲线

        FONT,                // 字体
        STRING,              // 字符串
        FORMAT_STRING,       // 格式化输出字符串

        IMAGE,               // 图片


        INST_MASK = 0x0000FFFF, // 指令掩码
        FILL_MODE = 0x00010000, // 填充模式
    };

    // 线段端点类型
    enum ENDCAP
    {
        ENDCAP_BUTT,   // 无扩展端点
        ENDCAP_SQUARE, // 方形端点
        ENDCAP_ROUND,  // 圆形端点
        ENDCAP_CLOSED, // 闭合多边形
    };

    // 线段连接类型
    enum JOINT
    {
        JOINT_SQUARE, // 方形连接
        JOINT_ROUND,  // 圆形连接
        JOINT_MITER,  // 斜接
    };

    // 画笔样式(点画模式)
    enum PEN
    {
        PEN_NONE         = 0x00, // 无
        PEN_SOLID        = 0x01, // 实心线
        PEN_DASH         = 0x02, // - - - -
        PEN_DOT          = 0x03, // .......
        PEN_DASH_DOT     = 0x04, // -.-.-.-
        PEN_DASH_DOT_DOT = 0x05, // -..-..-
        PEN_CUSTOM       = 0x0F, // 自定义点画模式
    };

    #pragma pack(push, 1)

    struct geometry
    {
        uint32_t index; // 顶点索引
        uint32_t count; // 顶点数量
    };

    struct stroke_style
    {
        uint32_t color;    // 画笔颜色
        float width;       // 画笔宽度
        uint16_t style;    // 画笔样式
        uint16_t fraction; // 画笔精度
        uint16_t endcap;   // 端点样式
        uint16_t joint;    // 连接样式
    };

    struct fill_style
    {
        uint32_t color; // 填充颜色
        union
        {
            uint32_t pattern; // 画刷纹样
            uint32_t image;   // 画刷图片
        };
        struct
        {
            float offsetx; // 填充偏移量
            float offsety;
        };
    };

    struct font_style
    {
        uint32_t family; // 家族名字索引
        float size;      // 字体大小
        uint32_t style;  // 字体样式
    };

    struct text_style
    {
        uint32_t string; // 文字索引
        uint32_t index;  // 顶点索引
        uint32_t color;  // 文字颜色
        uint32_t style;  // 文字样式
    };

    struct image_style
    {
        uint32_t id;    // 图片索引
        uint32_t index; // 顶点索引
        uint32_t color; // 图片混合颜色
        uint32_t blend; // 混合效果
    };

    struct rectangle
    {
        float x, y, w, h;
    };

    struct command
    {
        uint32_t inst; // 指令
        union
        {
            geometry geom;
            stroke_style stroke;
            fill_style fill;
            font_style font;
            text_style text;
            image_style image;
            rectangle clip;
        };

        command() : inst() { }
        command(uint32_t inst, uint32_t index, uint32_t count) :
            inst(inst)
        {
            geom.index = index;
            geom.count = count;
        }
    };

    #pragma pack(pop)

    /*
    struct font_info
    {
        uiString family;
        float size;
        uint32_t style;
    };

    struct text_span
    {
        float x, y, w, h;
        uiString text;

        text_span() { }
        text_span(float x, float y, float w, float h, const uiString& text) :
            x(x), y(y), w(w), h(h), text(text) { }
    };

    struct sprite
    {
        handle_type image;
        float x, y;
    };
    */



private:
    stroke_style m_stroke;  // 线条样式
    uint32_t m_fillColor;   // 填充颜色
    uint32_t m_fillPattern; // 填充图案
    uint32_t m_fillImage;   // 填充图片
    uiVec2 m_fillOffset;    // 填充偏移量
    uint32_t m_textColor;   // 文字颜色
    glm::mat3x2 m_transform;

    std::vector<command> m_commands;   // 命令列表
    std::vector<uiVec2> m_points;      // 顶点列表
    std::vector<uiString> m_strings;   // 字符串列表
    std::vector<handle_type> m_images; // 图片列表

public:
    uiCommandList()
    {
        m_stroke.color    = RGBA(0, 0, 0, 255);
        m_stroke.width    = 1.0f;
        m_stroke.style    = PEN_SOLID;
        m_stroke.fraction = 0;
        m_stroke.endcap   = ENDCAP_BUTT;
        m_stroke.joint    = JOINT_SQUARE;

        m_fillColor   = RGBA(255, 255, 255, 255);
        m_fillPattern = 0;
        m_fillImage   = 0;
        m_fillOffset  = uiVec2();

        m_textColor = RGBA(0, 0, 0, 255);

        m_transform = glm::mat3x2(1.0f);
    }

    // 判断是否为空
    bool empty() const
    {
        return m_commands.empty();
    }

    // 释放
    void dispose()
    {
        m_commands.clear();
        m_points.clear();
        m_strings.clear();
        m_images.clear();
    }

    // 返回命令数量
    size_t command_count() const { return m_commands.size(); }

    // 返回命令
    const command& commands(size_t id) const { return m_commands[id]; }

    // 返回命令数组
    const command* command_data() const { return m_commands.data(); }

    // 返回顶点数量
    size_t point_count() const { return m_points.size(); }

    // 返回顶点
    uiVec2& points(size_t id) { return m_points[id]; }
    const uiVec2& points(size_t id) const { return m_points[id]; }

    // 返回顶点数组
    uiVec2* point_data() { return m_points.data(); }
    const uiVec2* point_data() const { return m_points.data(); }

    // 返回字符串
    const uiString& strings(size_t id) const { return m_strings[id]; }

    // 压入剪裁矩形
    void push_clip(float x, float y, float w, float h, bool intersect)
    {
        command cmd;
        cmd.inst   = intersect ? PUSH_CLIP_INTERSECT : PUSH_CLIP_RESET;
        cmd.clip.x = x;
        cmd.clip.y = y;
        cmd.clip.w = w;
        cmd.clip.h = h;
        m_commands.push_back(cmd);
    }

    void push_clip(const uiRect& rect, bool intersect)
    {
        this->push_clip(rect.x, rect.y, rect.w, rect.h, intersect);
    }

    // 弹出剪裁矩形
    void pop_clip()
    {
        m_commands.push_back(command(POP_CLIP, 0, 0));
    }

    // 返回当前矩阵变换
    glm::mat3x2 transform() const
    {
        return m_transform;
    }

    // 设置矩阵变换
    void set_transform(const glm::mat3x2& transform)
    {
        command cmd;
        cmd.inst       = TRANSFORM;
        cmd.geom.index = static_cast<uint32_t>(m_points.size());
        cmd.geom.count = 3;
        m_commands.push_back(cmd);

        m_points.push_back(transform[0]);
        m_points.push_back(transform[1]);
        m_points.push_back(transform[2]);

        m_transform = transform;
    }

    /*
    void identity()
    {
        m_transform = mat3x2f(1.0f);
    }

    void translate(float x, float y)
    {
        m_transform = math::translate(m_transform, x, y);
    }

    void scale(float x, float y)
    {
        m_transform = math::scale(m_transform, x, y);
    }

    void rotate(float a)
    {
        m_transform = math::rotate(m_transform, a);
    }
    */

    // 返回画笔
    stroke_style stroke() const
    {
        return m_stroke;
    }

    // 返回线条颜色
    uiColor stroke_color() const
    {
        return uiColor(
            RGBA_RED(m_stroke.color),
            RGBA_GREEN(m_stroke.color),
            RGBA_BLUE(m_stroke.color),
            RGBA_ALPHA(m_stroke.color)
        );
    }

    // 返回线条宽度
    float stroke_width() const
    {
        return m_stroke.width;
    }

    // 返回线帽样式
    int stroke_endcap() const
    {
        return m_stroke.endcap;
    }

    // 返回线条交接样式
    int stroke_joint() const
    {
        return m_stroke.joint;
    }

    // 设置画笔
    void set_stroke(const uiColor& color, float width = 1.0f, uint32_t style = PEN_SOLID, int endcap = ENDCAP_BUTT, int joint = JOINT_SQUARE)
    {
        m_stroke.color = RGBA(color.r, color.g, color.b, color.a);
        m_stroke.width = width;
        m_stroke.style = style;
        // m_stroke.fraction = 0;
        m_stroke.endcap = endcap;
        m_stroke.joint  = joint;

        command cmd;
        cmd.inst   = STROKE;
        cmd.stroke = m_stroke;
        m_commands.push_back(cmd);
    }

    // 返回填充颜色
    uiColor fill_color() const
    {
        return uiColor(
            RGBA_RED(m_fillColor),
            RGBA_GREEN(m_fillColor),
            RGBA_BLUE(m_fillColor),
            RGBA_ALPHA(m_fillColor)
        );
    }

    // 返回填充图案
    uint32_t fill_pattern() const
    {
        return m_fillPattern;
    }

    // 设置纯色填充
    void set_fill(const uiColor& color)
    {
        m_fillColor = RGBA(color.r, color.g, color.b, color.a);

        command cmd;
        cmd.inst       = FILL_COLOR;
        cmd.fill.color = m_fillColor;
        m_commands.push_back(cmd);
    }

    // 设置纯色填充
    void set_fill(const uiColor& color, uint32_t pattern, float offsetX = 0.0f, float offsetY = 0.0f)
    {
        m_fillColor   = RGBA(color.r, color.g, color.b, color.a);
        m_fillPattern = pattern;
        m_fillOffset  = uiVec2(offsetX, offsetY);

        command cmd;
        cmd.inst         = FILL_PATTERN;
        cmd.fill.color   = m_fillColor;
        cmd.fill.pattern = m_fillPattern;
        cmd.fill.offsetx = offsetX;
        cmd.fill.offsety = offsetY;
        m_commands.push_back(cmd);
    }

    // 设置图片填充
    void set_fill(const uiColor& color, handle_type image, float offsetX = 0.0f, float offsetY = 0.0f)
    {
        m_fillColor  = RGBA(color.r, color.g, color.b, color.a);
        m_fillImage  = static_cast<uint32_t>(m_images.size());
        m_fillOffset = uiVec2(offsetX, offsetY);

        command cmd;
        cmd.inst         = FILL_IMAGE;
        cmd.fill.color   = m_fillColor;
        cmd.fill.image   = m_fillImage;
        cmd.fill.offsetx = offsetX;
        cmd.fill.offsety = offsetY;
        m_commands.push_back(cmd);

        m_images.push_back(image);
    }

    // 移动画笔
    void move_to(float x, float y)
    {
        this->move_to(uiVec2(x, y));
    }

    void move_to(uiVec2 v)
    {
        add_cmd(MOVETO, 1);
        m_points.push_back(v);
    }

    // 绘制直线到顶点
    void line_to(float x, float y)
    {
        this->line_to(uiVec2(x, y));
    }

    void line_to(uiVec2 v)
    {
        add_cmd(LINETO, 1);
        m_points.push_back(v);
    }

    // 添加直线
    void draw_line(float x1, float y1, float x2, float y2)
    {
        this->draw_line(uiVec2(x1, y1), uiVec2(x2, y2));
    }

    void draw_line(uiVec2 v1, uiVec2 v2)
    {
        add_cmd(LINE, 2);
        m_points.push_back(v1);
        m_points.push_back(v2);
    }

    // 绘制矩形
    void draw_rect(float x, float y, float w, float h)
    {
        add_cmd(RECT, 2);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
    }

    void draw_rect(const uiRect& rect) { this->draw_rect(rect.x, rect.y, rect.w, rect.h); }

    // 填充矩形
    void fill_rect(float x, float y, float w, float h)
    {
        add_cmd(RECT | FILL_MODE, 2);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
    }

    void fill_rect(const uiRect& rect)
    {
        this->fill_rect(rect.x, rect.y, rect.w, rect.h);
    }

    // 绘制圆角矩形
    void draw_round_rect(float x, float y, float w, float h, float rx, float ry)
    {
        add_cmd(ROUND_RECT, 3);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
        m_points.push_back(uiVec2(rx, ry));
    }

    void draw_round_rect(const uiRect& rect, float rx, float ry)
    {
        this->draw_round_rect(rect.x, rect.y, rect.w, rect.h, rx, ry);
    }

    // 填充圆角矩形
    void fill_round_rect(float x, float y, float w, float h, float rx, float ry)
    {
        add_cmd(ROUND_RECT | FILL_MODE, 3);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
        m_points.push_back(uiVec2(rx, ry));
    }

    void fill_round_rect(const uiRect& rect, float rx, float ry)
    {
        this->fill_round_rect(rect.x, rect.y, rect.w, rect.h, rx, ry);
    }

    // 绘制椭圆
    void draw_ellipse(float x, float y, float w, float h)
    {
        add_cmd(ELLIPSE, 2);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
    }

    void draw_ellipse(const uiRect& rect)
    {
        this->draw_ellipse(rect.x, rect.y, rect.w, rect.h);
    }

    // 填充椭圆
    void fill_ellipse(float x, float y, float w, float h)
    {
        add_cmd(ELLIPSE | FILL_MODE, 2);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
    }

    void fill_ellipse(const uiRect& rect)
    {
        this->fill_ellipse(rect.x, rect.y, rect.w, rect.h);
    }

    // 绘制三角形
    void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        this->draw_triangle(uiVec2(x1, y1), uiVec2(x2, y2), uiVec2(x3, y3));
    }

    void draw_triangle(uiVec2 v1, uiVec2 v2, uiVec2 v3)
    {
        add_cmd(TRIANGLE, 3);
        m_points.push_back(v1);
        m_points.push_back(v2);
        m_points.push_back(v3);
    }

    void draw_triangle(const uiVec2* points)
    {
        add_cmd(TRIANGLE, 3);
        m_points.push_back(points[0]);
        m_points.push_back(points[1]);
        m_points.push_back(points[2]);
    }

    // 填充三角形
    void fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        this->fill_triangle(uiVec2(x1, y1), uiVec2(x2, y2), uiVec2(x3, y3));
    }

    void fill_triangle(uiVec2 v1, uiVec2 v2, uiVec2 v3)
    {
        add_cmd(TRIANGLE | FILL_MODE, 3);
        m_points.push_back(v1);
        m_points.push_back(v2);
        m_points.push_back(v3);
    }

    void fill_triangle(const uiVec2* points)
    {
        add_cmd(TRIANGLE | FILL_MODE, 3);
        m_points.push_back(points[0]);
        m_points.push_back(points[1]);
        m_points.push_back(points[2]);
    }

    // 绘制折线
    void draw_polyline(const uiVec2* points, size_t size)
    {
        add_cmd(POLYLINE, size);
        m_points.insert(m_points.end(), points, points + size);
    }

    // 绘制多边形
    void draw_polygon(const uiVec2* points, size_t size)
    {
        add_cmd(POLYGON, size);
        m_points.insert(m_points.end(), points, points + size);
    }

    // 填充多边形
    void fill_polygon(const uiVec2* points, size_t size)
    {
        add_cmd(POLYGON | FILL_MODE, size);
        m_points.insert(m_points.end(), points, points + size);
    }

    // 绘制三次贝塞尔曲线
    void draw_bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        this->draw_bezier(uiVec2(x1, y1), uiVec2(x2, y2), uiVec2(x3, y3), uiVec2(x4, y4));
    }

    void draw_bezier(const uiVec2& v1, const uiVec2& v2, const uiVec2& v3, const uiVec2& v4)
    {
        add_cmd(BEZIER, 4);
        m_points.push_back(v1);
        m_points.push_back(v2);
        m_points.push_back(v3);
        m_points.push_back(v4);
    }

    // 设置字体
    void set_font(const uiString& family, float size, uint32_t style)
    {
        command cmd;
        cmd.inst        = FONT;
        cmd.font.family = static_cast<uint32_t>(m_strings.size());
        cmd.font.size   = size;
        cmd.font.style  = style;
        m_commands.push_back(cmd);

        m_strings.push_back(family);
    }

    // 返回文字颜色
    uiColor text_color() const
    {
        return uiColor(
            RGBA_RED(m_textColor),
            RGBA_GREEN(m_textColor),
            RGBA_BLUE(m_textColor),
            RGBA_ALPHA(m_textColor)
        );
    }

    // 设置文字颜色
    void set_text(const uiColor& color)
    {
        m_textColor = RGBA(color.r, color.g, color.b, color.a);
    }

    // 绘制字符串
    void draw_string(float x, float y, const uiChar* str, size_t size)
    {
        this->draw_string(x, y, uiString(str, size));
    }

    void draw_string(float x, float y, const uiString& str)
    {
        this->draw_string(uiVec2(x, y), str);
    }

    void draw_string(uiVec2 pos, const uiString& str)
    {
        command cmd;
        cmd.inst        = STRING;
        cmd.text.string = static_cast<uint32_t>(m_strings.size());
        cmd.text.index  = static_cast<uint32_t>(m_points.size());
        cmd.text.color  = m_textColor;
        cmd.text.style  = 0;
        m_commands.push_back(cmd);

        m_strings.push_back(str);
        m_points.push_back(pos);
    }

    // 绘制矩形内格式化的字符串
    void draw_string(float x, float y, float w, float h, const uiChar* str, size_t size, uint32_t style)
    {
        this->draw_string(x, y, w, h, uiString(str, size), style);
    }

    void draw_string(float x, float y, float w, float h, const uiString& str, uint32_t style)
    {
        command cmd;
        cmd.inst        = FORMAT_STRING;
        cmd.text.string = static_cast<uint32_t>(m_strings.size());
        cmd.text.index  = static_cast<uint32_t>(m_points.size());
        cmd.text.color  = m_textColor;
        cmd.text.style  = style;
        m_commands.push_back(cmd);

        m_strings.push_back(str);
        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
    }

    void draw_string(const uiRect& rect, const uiString& str, uint32_t style)
    {
        this->draw_string(rect.x, rect.y, rect.w, rect.h, str, style);
    }

    // 绘制图片
    void draw_image(handle_type image, float x, float y, float w, float h, uiColor color)
    {
        command cmd;
        cmd.inst        = IMAGE;
        cmd.image.id    = static_cast<uint32_t>(m_images.size());
        cmd.image.index = static_cast<uint32_t>(m_points.size());
        cmd.image.color = RGBA(color.r, color.g, color.b, color.a);
        cmd.image.blend = 0;
        m_commands.push_back(cmd);

        m_images.push_back(image);

        m_points.push_back(uiVec2(x, y));
        m_points.push_back(uiVec2(w, h));
    }

private:
    // 判断绘图指令是否可以合批
    bool is_batchable(uint32_t inst) const
    {
        switch (inst & INST_MASK) {
        case MOVETO:
        case POINT:
        case LINE:
        case RECT:
        case ROUND_RECT:
        case ELLIPSE:
        case BEZIER:
            return true;
        }
        return false;
    }

    // 添加命令，合批
    void add_cmd(uint32_t inst, uint32_t vertices)
    {
        // 新建命令
        if (m_commands.empty() || m_commands.back().inst != inst || !is_batchable(m_commands.back().inst)) {
            m_commands.push_back(command(inst, m_points.size(), vertices));
        }
        else {
            // 增加顶点数量
            m_commands.back().geom.count += vertices;
        }
    }
};

#pragma pack(push, 1)
struct uiVertex
{
    uiVec2 position;
    uiVec2 texcoord;
    uiColor color;

    uiVertex() { }
    uiVertex(uiVec2 pos, uiVec2 uv, uiColor diff) : position(pos), texcoord(uv), color(diff) { }
};
#pragma pack(pop)

//
// 3D 渲染指令缓冲
//

class uiRenderList
{
public:
    enum COMMAND
    {
        NONE,

        POINTS,
        LINES,
        TRIANGLES,

        TEXTURE_2D,
        CLIPBOX,
    };

    // clang-format off
    struct command
    {
        uint32_t inst;
        union {
            struct {
                uiHandle handle;
            };
            struct {
                uint32_t index;
                uint32_t count;
            };
            struct {
                float x;
                float y;
            };
        };

        command() : inst() { }
        command(uint32_t inst, uiHandle handle) :
            inst(inst), handle(handle) { }
        command(uint32_t inst, uint32_t index, uint32_t count) :
            inst(inst), index(index), count(count) { }
        command(uint32_t inst, float x, float y) :
            inst(inst), x(x), y(y) { }
    };
    // clang-format on

private:
    std::vector<command> m_commands;
    std::vector<uiVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<uiRect> m_clipStack;
    uiColor m_color;

public:
    uiRenderList() :
        m_commands(), m_vertices(), m_indices() { }

    // 判断是否为空
    bool empty() const
    {
        return m_commands.empty();
    }

    // 释放
    void dispose()
    {
        m_commands.clear();
        m_vertices.clear();
        m_indices.clear();
    }

    // 返回命令数量
    size_t command_count() const { return m_commands.size(); }

    // 返回命令
    const command& commands(size_t id) const { return m_commands[id]; }

    // 返回命令数组
    const command* command_data() const { return m_commands.data(); }

    // 返回顶点数量
    size_t vertex_count() const { return m_vertices.size(); }

    // 返回顶点数组
    const uiVertex* vertex_data() const { return m_vertices.data(); }

    // 返回索引数量
    size_t index_count() const { return m_indices.size(); }

    // 返回索引数组
    const uint32_t* index_data() const { return m_indices.data(); }

    // 返回当前颜色
    uiColor color() const { return m_color; }

    // 设置剪裁矩形
    void set_clipbox(float x, float y, float w, float h)
    {
        m_commands.push_back(command(CLIPBOX, x, y));
        m_commands.push_back(command(NONE, w, h));
    }

    // 设置颜色
    void set_color(const uiColor& color) { m_color = color; }
    void set_color(const uint32_t& color)
    {
        m_color = uiColor(
            RGBA_RED(color),
            RGBA_GREEN(color),
            RGBA_BLUE(color),
            RGBA_ALPHA(color)
        );
    }

    // 绘制直线
    void draw_line(float x1, float y1, float x2, float y2)
    {
        add_cmd(LINES, 2);

        size_t id = m_vertices.size();

        // 添加顶点
        m_vertices.push_back(uiVertex(uiVec2(x1, y1), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x2, y2), uiVec2(0.0f, 0.0f), m_color));

        // 添加索引
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 1);
    }

    // 绘制矩形
    void draw_rect(float x, float y, float w, float h)
    {
        add_cmd(LINES, 8);

        size_t id = m_vertices.size();

        // 添加顶点
        m_vertices.push_back(uiVertex(uiVec2(x, y), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x, y + h), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x + w, y + h), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x + w, y), uiVec2(0.0f, 0.0f), m_color));

        // 添加索引
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 3);
        m_indices.push_back(id + 3);
        m_indices.push_back(id + 0);
    }

    // 填充矩形
    void fill_rect(float x, float y, float w, float h)
    {
        add_cmd(TRIANGLES, 6);

        size_t id = m_vertices.size();

        // 添加顶点
        m_vertices.push_back(uiVertex(uiVec2(x, y), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x, y + h), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x + w, y + h), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x + w, y), uiVec2(0.0f, 0.0f), m_color));

        // 添加索引
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 3);
    }

    // 绘制三角形
    void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        add_cmd(LINES, 4);

        size_t id = m_vertices.size();

        // 添加顶点
        m_vertices.push_back(uiVertex(uiVec2(x1, y1), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x2, y2), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x3, y3), uiVec2(0.0f, 0.0f), m_color));

        // 添加索引
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 0);
    }

    // 填充三角形
    void fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        add_cmd(TRIANGLES, 3);

        size_t id = m_vertices.size();

        // 添加顶点
        m_vertices.push_back(uiVertex(uiVec2(x1, y1), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x2, y2), uiVec2(0.0f, 0.0f), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x3, y3), uiVec2(0.0f, 0.0f), m_color));

        // 添加索引
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 2);
    }

    // 绘制纹理
    void draw_image(
        float x, float y, float w, float h,
        float u1 = 0.0f, float v1 = 0.0f, float u2 = 0.0f, float v2 = 0.0f
    )
    {
        add_cmd(TRIANGLES, 6);

        size_t id = m_vertices.size();

        // 添加顶点
        m_vertices.push_back(uiVertex(uiVec2(x, y), uiVec2(u1, v1), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x, y + h), uiVec2(u1, v2), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x + w, y + h), uiVec2(u2, v2), m_color));
        m_vertices.push_back(uiVertex(uiVec2(x + w, y), uiVec2(u2, v1), m_color));

        // 添加索引
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 1);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 0);
        m_indices.push_back(id + 2);
        m_indices.push_back(id + 3);
    }

    // 绑定纹理
    void set_texture(uiHandle tex2D)
    {
        m_commands.push_back(command(TEXTURE_2D, tex2D));
    }

private:
    // 判断绘图指令是否可以合批
    bool is_batchable(uint32_t inst) const
    {
        switch (inst) {
        case POINTS:
        case LINES:
        case TRIANGLES:
            return true;
        }
        return false;
    }

    // 添加命令，合批
    void add_cmd(uint32_t inst, uint32_t count)
    {
        // uint32_t color = RGBA(m_color.r, m_color.g, m_color.b, m_color.a);

        // 新建命令
        if (m_commands.empty() ||
            m_commands.back().inst != inst ||
            !is_batchable(m_commands.back().inst)) {
            m_commands.push_back(command(inst, m_indices.size(), count));
        }
        else {
            // 增加索引数量
            m_commands.back().count += count;
        }
    }
};

//
// 扩展绘制
//

// 绘制检查矩形
void draw_check_rect(uiCommandList* cmdlist, float x, float y, float w, float h);
void draw_check_rect(uiCommandList* cmdlist, const uiRect& rect);

// 上下箭头
void draw_updown_arrow(uiCommandList* cmdlist, float x, float y, float w, float h, float scale, bool isUp);

// 绘制折叠图标
void draw_collapse_icon(uiCommandList* cmdlist, float x, float y, float size, float thickness, bool collapsed);

// 绘制关闭图标
void draw_close_icon(uiCommandList* cmdlist, float x, float y, float size, float thickness);

// 填充三角形箭头
void fill_triangle_arrow(uiCommandList* cmdlist, float x, float y, float w, float h, uiDirection dir);

// 绘制对勾
void draw_checkmark(uiCommandList* cmdlist, float x, float y, float size, float thickness);

} // namespace minimgui

#endif // UI_GRAPH_H_20260316085932
