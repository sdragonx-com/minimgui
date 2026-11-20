/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 imagelist.hpp

 2015-04-05 01:46:12

*/
#ifndef GL_IMAGELIST_HPP_20150405014612
#define GL_IMAGELIST_HPP_20150405014612

#include "../ui_std.h"

#include <map>
#include <vector>

#include <glad/gl4.6.h>

namespace minimgui {

//
// 图像列表，管理等高高度的小图像图集
//

template<typename T, typename USERDATA>
class glimagelist
{
public:
    // 瓦片图片
    struct tile_image
    {
        GLuint image;
        int16_t x, y, width, height;
        float u1, v1, u2, v2;
    };

    // 图标
    struct icon_type
    {
        tile_image image; // 瓦片图片
        USERDATA data;    // 用户数据
    };

    typedef T index_type;
    typedef const icon_type* const_pointer;

private:
    typedef typename std::map<index_type, icon_type> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;

    std::vector<GLuint> m_pages; // 纹理缓存列表
    container m_images;          // 图标信息列表
    GLsizei m_width;             // 纹理宽度
    GLsizei m_height;            // 纹理高度
    GLenum m_format;             // 纹理格式
    GLenum m_filter;             // 纹理过滤方式
    GLint m_iconHeight;          // 图标高度
    GLint m_page;                // 当前操作页面
    GLint m_u, m_v;              // 添加位置

public:
    glimagelist() :
        m_pages(),
        m_images(),
        m_width(1024),
        m_height(1024),
        m_format(GL_RGBA),
        m_filter(GL_LINEAR),
        m_iconHeight(16),
        m_page(-1),
        m_u(),
        m_v() { }

    glimagelist(int width, int height, int size, int format, int filter = GL_LINEAR) :
        m_pages(), m_images(), m_u(), m_v()
    {
        this->create(width, height, size, format, filter);
    }

    ~glimagelist()
    {
        this->dispose();
    }

    // 初始化
    int init(int width, int height, int iconHeight, GLenum format, GLenum filter = GL_LINEAR)
    {
        this->dispose();
        m_width      = width;
        m_height     = height;
        m_format     = format,
        m_iconHeight = iconHeight;
        m_filter     = filter;
        return 0;
    }

    // 重置图标大小，清空图标
    void reset(int iconHeight)
    {
        m_iconHeight = iconHeight;
        m_images.clear();
        m_page = m_pages.empty() ? -1 : 0;
        m_u = m_v = 0;

        // 清理纹理
        /*
        for (size_t i = 0; i < m_pages.size(); ++i) {
            glBindTexture(GL_TEXTURE_2D, m_pages[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        */
    }

    // 释放
    void dispose()
    {
        if (!m_pages.empty()) {
            glDeleteTextures(GLsizei(m_pages.size()), &m_pages[0]);
        }
        m_pages.clear();

        m_images.clear();
        m_page = -1;
        m_u = m_v = 0;
    }

    // 返回纹理宽度
    int width() const
    {
        return m_width;
    }

    // 返回纹理高度
    int height() const
    {
        return m_height;
    }

    // 添加图标
    int insert(const T& index, int width, int height, GLenum format, const void* data, const USERDATA& userdata)
    {
        check_move(width);

        glBindTexture(GL_TEXTURE_2D, m_pages[m_page]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, m_u, m_v, width, height, format, GL_UNSIGNED_BYTE, data);

        icon_type icon;
        icon.image.image  = m_pages[m_page];
        icon.image.x      = uint16_t(m_u);
        icon.image.y      = uint16_t(m_v);
        icon.image.width  = uint16_t(width);
        icon.image.height = uint16_t(height);
        icon.image.u1     = float(m_u) / float(m_width);
        icon.image.v1     = float(m_v) / float(m_height);
        icon.image.u2     = float(m_u + width) / float(m_width);
        icon.image.v2     = float(m_v + height) / float(m_height);
        icon.data         = userdata;

        m_images[index] = icon;
        m_u += width;
        ++m_u; // 做一个像素的间距

        return index;
    }

    // 返回图标数量
    size_t size() const
    {
        return m_images.size();
    }

    // 返回图标
    const_pointer icons(const T& index) const
    {
        const_iterator itr = m_images.find(index);
        if (itr != m_images.end()) {
            return &itr->second;
        }
        else {
            return nullptr;
        }
    }

    // 判断是索引否存在
    bool exists(const T& index) const
    {
        return m_images.find(index) != m_images.end();
    }

    // 测试
    /*
    void draw(float x, float y)
    {
        const float w = float(m_width) * 0.25f;
        const float h = float(m_height) * 0.25f;
        for (size_t i = 0; i < m_pages.size(); ++i) {
            GL110::draw_image(m_pages[i], x, y, w, h);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            GL110::draw_rect(x, y, w, h);
            y += h;
        }
    }
    */

private:
    // 测试当前页是否能容纳下一个图标，如果容纳不下则新建一页
    void check_move(int width)
    {
        if (m_u + width > m_width) {
            m_u = 0;
            m_v += m_iconHeight;
            ++m_v; // 行距一个像素
        }

        // 添加纹理页
        if (m_pages.empty() || m_v + m_iconHeight > m_height) {
            ++m_page;
            if (static_cast<size_t>(m_page) >= m_pages.size()) {
                GLuint texID = create_texture(m_width, m_height, m_format, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                // CGL_LOG("add page %p", this);
                m_pages.push_back(texID);
            }
            m_u = 0;
            m_v = 0;
        }
    }

    // 创建纹理
    GLuint create_texture(int width, int height, GLenum format, GLenum source_format, GLenum source_type, const GLubyte* pixels)
    {
        GLuint texID = 0;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, source_format, source_type, pixels);

        // 设置纹理缩放的插值方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter);

        // 设置纹理环绕模式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        return texID;
    }

    /*
    int paint(const T& index, int x, int y, int width, int height, const vec4ub& color)
    {
        imageblock* image = BindTexture(index);
        assert(image);
        gl.draw_rect(x, y, x + width, y + height, image->u1, image->v1, image->u2, image->v2, color);
        return 0;
    }

    int paint(draw_context& dc, const T& index, int x, int y, int width, int height, const vec4ub& color)
    {
        imageblock* image = BindTexture(index);
        assert(image);
        // glxRectf(x, y, x+width, y+height, image->u1, image->v1, image->u2, image->v2);
        dc.draw_image(image->image, x, y, width, height, image->u1, image->v1, image->u2, image->v2);
        return 0;
    }

    int paint(const T& index, int x, int y, const vec4ub& color)
    {
        teximage* icon = BindTexture(index);
        assert(icon);
        // m_gl->color(color.red/255.0f, color.green/255.0f, color.blue/255.0f, color.alpha/255.0f);
        // m_gl->draw_rect(x, y, x+icon->width, y+icon->height, color, icon->u1, icon->v1, icon->u2, icon->v2);
        return 0;
    }

    int paint(draw_context& dc, const T& index, int x, int y)
    {
        teximage* image = BindTexture(index);
        assert(image);
        dc.draw_image(image->image, x, y, image->width, image->height, image->u1, image->v1, image->u2, image->v2);
        return 0;
    }

    teximage* BindTexture(int index)
    {
        iterator itr = m_images.find(index);
        if (itr != m_images.end()) {
            glBindTexture(GL_TEXTURE_2D, itr->second.image.image);
            return &itr->second.image;
        }
        return nullptr;
    }
    */
};

} // namespace minimgui

#endif // GL_IMAGELIST_HPP_20150405014612
