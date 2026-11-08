/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 ui_imagelist.h

 2026-11-05 22:35:53

*/
#ifndef UI_IMAGELIST_H_20261105223553
#define UI_IMAGELIST_H_20261105223553

#include "ui_std.h"
#include "ui_user.h"

#include <map>

namespace minimgui {

// 瓦片图片
struct tile_image
{
    uiHandle image;
    int16_t x, y, width, height;
    float u1, v1, u2, v2;
};

//
// 图像列表，管理等高高度的小图像图集
//

template<typename T>
class uiImageList
{
public:
    typedef T index_type;

private:
    typedef typename std::map<index_type, tile_image> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;

    std::vector<uiHandle> m_pages; // 纹理缓存列表
    container m_images;            // 图标信息列表
    int m_width;                   // 纹理宽度
    int m_height;                  // 纹理高度
    int m_format;                  // 纹理格式
    int m_iconHeight;              // 图标高度
    int m_page;                    // 当前操作页面
    int m_u, m_v;                  // 添加位置

public:
    uiImageList() :
        m_pages(),
        m_images(),
        m_width(1024),
        m_height(1024),
        m_format(),
        m_iconHeight(16),
        m_page(-1),
        m_u(),
        m_v() { }

    uiImageList(int width, int height, int size, int format) :
        m_pages(), m_images(), m_u(), m_v()
    {
        this->create(width, height, size, format);
    }

    ~uiImageList()
    {
        this->dispose();
    }

    // 初始化
    int init(int width, int height, int iconHeight, int format)
    {
        this->dispose();
        m_width      = width;
        m_height     = height;
        m_format     = format,
        m_iconHeight = iconHeight;
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
            for (size_t i = 0; i < m_pages.size(); ++i) {
                delete_image(m_pages[i]);
            }
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
    int insert(const T& index, int width, int height, const void* data)
    {
        check_move(width);

        update_image(m_pages[m_page], m_u, m_v, width, height, data);

        tile_image icon;
        icon.image  = m_pages[m_page];
        icon.x      = uint16_t(m_u);
        icon.y      = uint16_t(m_v);
        icon.width  = uint16_t(width);
        icon.height = uint16_t(height);
        icon.u1     = float(m_u) / float(m_width);
        icon.v1     = float(m_v) / float(m_height);
        icon.u2     = float(m_u + width) / float(m_width);
        icon.v2     = float(m_v + height) / float(m_height);

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
    tile_image* icons(const T& index) const
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
                uiHandle texID = create_image(m_width, m_height);
                // CGL_LOG("add page %p", this);
                m_pages.push_back(texID);
            }
            m_u = 0;
            m_v = 0;
        }
    }
};

} // namespace minimgui

#endif // UI_IMAGELIST_H_20261105223553
