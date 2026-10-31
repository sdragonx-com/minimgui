/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_math.h

 2026-10-31 17:44:06

*/
#ifndef UI_MATH_H_20261031174406
#define UI_MATH_H_20261031174406

// glm
#if (defined(__BORLANDC__) && !defined(__clang__))
    #include <cgl/external/glm/glm/glm.hpp>

#else // CGL_USE_GLM98
    // glm 1.0.1
    #if !defined(GLM_ENABLE_EXPERIMENTAL)
        #define GLM_ENABLE_EXPERIMENTAL
    #endif

    #include <glm/glm.hpp>

    #ifndef GLM_API
        #define GLM_API     inline
        #define GLM_INS_API inline
    #endif

    #ifndef GLM_CLASS_TYPEDEF
        #define GLM_CLASS_TYPEDEF CGL_CLASS_TYPEDEF
    #endif

#endif // CGL_USE_GLM98

namespace minimgui {

// 统计数组元素数量
#if !defined(countof)
    #define countof(a) (sizeof(a) / sizeof(a[0]))
#endif

// 二维向量
typedef glm::vec2 uiVec2;
typedef glm::ivec2 uiVec2i;

// 矩形
class uiRect
{
public:
    float x;
    float y;
    float w;
    float h;

public:
    uiRect() : x(), y(), w(), h() { }

    uiRect(float x, float y, float width, float height) :
        x(x),
        y(y),
        w(width),
        h(height) { }

    uiRect(uiVec2 pos, uiVec2 size) :
        x(pos.x),
        y(pos.y),
        w(size.x),
        h(size.y) { }

    // 返回位置
    uiVec2 position() const
    {
        return uiVec2(x, y);
    }

    // 返回大小
    uiVec2 size() const
    {
        return uiVec2(w, h);
    }

    // 返回中心点
    uiVec2 center() const
    {
        return uiVec2(x + w * 0.5f, y + h * 0.5f);
    }

    // 返回位移后的矩形
    uiRect offset(float x, float y) const
    {
        return uiRect(this->x + x, this->y + y, this->w, this->h);
    }

    uiRect offset(const uiVec2& v) const
    {
        return this->offset(v.x, v.y);
    }

    // 返回放大后的矩形
    uiRect expand(float cx, float cy) const
    {
        return uiRect(
            x - cx,
            y - cy,
            w + cx * 2.0f,
            h + cy * 2.0f
        );
    }

    // 返回缩小后的矩形
    uiRect shrink(float cx, float cy) const
    {
        return uiRect(
            x + cx,
            y + cy,
            w - cx * 2.0f,
            h - cy * 2.0f
        );
    }

    // 返回点是否在矩形内
    bool contains(float x, float y) const
    {
        return x > this->x &&
               y > this->y &&
               x < this->x + this->w &&
               y < this->y + this->h;
    }

    bool contains(const uiVec2& v) const
    {
        return this->contains(v.x, v.y);
    }

    // 返回矩形是否包含另外一个矩形
    bool contains(const uiRect& other) const
    {
        return x <= other.x &&
               y <= other.y &&
               x + w >= other.x + other.w &&
               y + h >= other.y + other.h;
    }

    bool contains(const uiVec2& pos, const uiVec2& size) const
    {
        return x <= pos.x &&
               y <= pos.y &&
               x + w >= pos.x + size.x &&
               y + h >= pos.y + size.y;
    }

    // 返回矩形交集
    uiRect operator&(const uiRect& other) const
    {
        float left   = std::max(x, other.x);
        float top    = std::max(y, other.y);
        float right  = std::min(x + w, other.x + other.w);
        float bottom = std::min(y + h, other.y + other.h);

        return uiRect(left, top, right - left, bottom - top);
    }

    // 返回矩形并集
    uiRect operator|(const uiRect& other) const
    {
        float left   = std::min(x, other.x);
        float top    = std::min(y, other.y);
        float right  = std::max(x + w, other.x + other.w);
        float bottom = std::max(y + h, other.y + other.h);

        return uiRect(left, top, right - left, bottom - top);
    }

    // 判断矩形是否相交
    bool collision(const uiRect& other) const
    {
        float left   = std::max(x, other.x);
        float top    = std::max(y, other.y);
        float right  = std::min(x + w, other.x + other.w);
        float bottom = std::min(y + h, other.y + other.h);

        return left < right && top < bottom;
    }
};

// 边界
class uiBounds
{
public:
    float left;
    float top;
    float right;
    float bottom;

    uiBounds() : left(), top(), right(), bottom() { }
    uiBounds(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) { }
};

using glm::clamp;

// 判断浮点数是否零
inline bool is_zero(float n)
{
    return fabs(n) < FLT_EPSILON;
}

// 叉乘
inline float cross(uiVec2 a, uiVec2 b)
{
    return a.x * b.y - a.y * b.x;
}

// 判断点 p 是否在三角形 abc 内
inline bool point_in_triangle(uiVec2 p, uiVec2 a, uiVec2 b, uiVec2 c)
{
    float d1 = cross(p - a, b - a);
    float d2 = cross(p - b, c - b);
    float d3 = cross(p - c, a - c);

    bool neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(neg && pos);
}

// 将大小等比例缩放到矩形内
inline uiVec2 scale_to_rect(const uiVec2& size, float width, float height, bool stretch)
{
    if (size.x <= 0 || size.x <= 0 || width <= 0 || height <= 0) {
        return size;
    }

    // 计算宽高缩放比
    float scale = std::min(width / size.x, height / size.y);

    if (1.0f < scale) { // 放大
        if (stretch) {  // 伸展
            return uiVec2(size.x * scale, size.y * scale);
        }
        else { // 不放大
            return size;
        }
    }

    return uiVec2(size.x * scale, size.y * scale);
}

} // namespace minimgui

#endif // UI_MATH_H_20261031174406
