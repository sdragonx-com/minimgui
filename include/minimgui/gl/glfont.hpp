/*
 Copyright (c) 2005-2020 sdragonx (mail:sdragonx@foxmail.com)

 glfont.hpp

 sdragonx 2015-04-05 01:47:10

*/
#ifndef GLFONT_HPP_20150405014710_STB
#define GLFONT_HPP_20150405014710_STB

#include <cgl/public.h>
#include <cgl/graphics/gl/glimagelist.hpp>
#include <cgl/graphics/gl/gl110/draw.hpp>
#include <cgl/io/utility.hpp>

#include <stb/stb_truetype.h>


namespace cgl {
namespace graphics {

class glfont_stb
{
public:
    // 字形矩阵
    struct char_info    // glyph_metrics
    {
        float bearingX; // character offset
        float bearingY;
        float advanceX; // the offset of the next position
        float advanceY;
    };

    static const int DEFAULT_TEXTURE_SIZE = 1024; // 默认纹理大小

    typedef glimagelist<wchar_t, char_info>::const_pointer glyph;

private:
    std::vector<byte_t> m_fontData;
    mutable stbtt_fontinfo m_font;
    float m_scale; // 字体缩放
    float m_size;  // 字体大小

public:
    typedef glimagelist<wchar_t, char_info> imagelist_type;
    imagelist_type m_imagelist;

public:
    vec4ub color;

public:
    glfont_stb() : m_font(), m_imagelist() { }
    glfont_stb(const char* filename, float fontSize, size_t textureSize = DEFAULT_TEXTURE_SIZE) :
        m_font(), m_imagelist()
    {
        this->load(filename, fontSize, textureSize);
    }
    ~glfont_stb()
    {
        this->dispose();
    }

    // 判断是否为空
    bool is_null() const
    {
        return m_font.data == nullptr;
    }

    // 返回字体句柄
    stbtt_fontinfo* handle() const
    {
        return &m_font;
    }

    // 加载字体
    int load(const char* filename, float fontSize, size_t textureSize = DEFAULT_TEXTURE_SIZE)
    {
        // 读取字体
        if (io::load_file(m_fontData, filename) != CGL_OK) {
            printf("freetype> load font failed: %s.\n", filename);
            return -1;
        }

        // 初始化字体
        if (stbtt_InitFont(&m_font, &m_fontData[0], stbtt_GetFontOffsetForIndex(&m_fontData[0], 0)) == 0) {
            printf("freetype> init font error.\n");
            return -1;
        }

        // 设置字体大小
        this->set_size(fontSize);

        // 初始化图标列表
        m_imagelist.init(textureSize, textureSize, static_cast<int>(fontSize + 0.5f), GL_RGBA, GL_NEAREST);

        return 0;
    }

    // 从内存加载字体
    int load(const void* data, size_t size, float fontSize, size_t textureSize = DEFAULT_TEXTURE_SIZE)
    {
        if (!data || size == 0) {
            return -1;
        }

        this->dispose();

        // 复制字体数据
        m_fontData.assign(static_cast<const byte_t*>(data), static_cast<const byte_t*>(data) + size);

        // 初始化字体
        if (stbtt_InitFont(&m_font, &m_fontData[0], stbtt_GetFontOffsetForIndex(&m_fontData[0], 0)) == 0) {
            printf("freetype> init font error.\n");
            return -1;
        }

        // 设置字体大小
        this->set_size(fontSize);

        // 初始化图标列表
        m_imagelist.init(textureSize, textureSize, static_cast<int>(fontSize + 0.5f), GL_RGBA, GL_NEAREST);

        return 0;
    }

    // 释放
    void dispose()
    {
        m_fontData.clear();
        m_font.data = nullptr;
        m_size      = 0.0f;
        m_scale     = 0.0f;
        m_imagelist.dispose();
    }

    // 字体名称
    const char* name() const
    {
        int length = 0;
        return stbtt_GetFontNameString(
            this->handle(),
            &length,
            STBTT_PLATFORM_ID_MICROSOFT, STBTT_MS_EID_UNICODE_BMP, STBTT_MS_LANG_ENGLISH,
            0
        );
    }

    // 返回字体大小
    float size() const
    {
        return m_size;
    }

    // 设置字体大小
    void set_size(float size, int dpi = 72)
    {
        if (m_size != size) {
            m_size = size;
            // 2022-08-21 02:54:04
            m_scale = stbtt_ScaleForMappingEmToPixels(this->handle(), size * static_cast<float>(dpi) / 72.0f);

            // 重置图像列表
            // m_imagelist.init(m_imagelist.width(), m_imagelist.height(), static_cast<int>(m_size + 0.5f), GL_RGBA, GL_NEAREST);
            m_imagelist.reset(m_size);
        }
    }

    // 获得字符宽度
    float char_width(wchar_t ch)
    {
        if (iscntrl(ch)) { // 控制字符
            return 0.0f;
        }
        if (!m_imagelist.exists(ch)) {
            make_char(ch);
        }
        glyph icon = m_imagelist.icons(ch);
        return icon ? icon->data.advanceX : 0.0f;
    }

    // 获取字符高度
    float char_height()
    {
        return m_size;
    }

    // 获取字符串宽度
    float text_width(const wchar_t* text, size_t length)
    {
        float width = 0.0f;
        for (size_t i = 0; i < length; ++i) {
            wchar_t ch = text[i];
            if (iscntrl(ch)) { // 控制字符
                continue;
            }
            glyph icon = get_char(ch);
            width += icon->data.advanceX;
        }
        return width;
    }

    // 返回字符
    glyph get_char(wchar_t ch)
    {
        if (!m_imagelist.exists(ch)) {
            make_char(ch);
        }
        return m_imagelist.icons(ch);
    }

    // 输出一个字符
    float put_char(float x, float y, wchar_t ch)
    {
        if (iscntrl(ch)) { // 控制字符
            return 0.0f;
        }
        if (!m_imagelist.exists(ch)) {
            make_char(ch);
        }
        glyph icon = m_imagelist.icons(ch);
        if (icon) {
            GL110::draw_image(
                icon->image.image,
                x + icon->data.bearingX,
                y + (m_size - icon->data.bearingY),
                static_cast<float>(icon->image.width),
                static_cast<float>(icon->image.height),
                icon->image.u1, icon->image.v1, icon->image.u2, icon->image.v2
            );

            /*
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            GL110::draw_rect(
                x + icon->data.bearingX,
                y + (m_size - icon->data.bearingY),
                static_cast<float>(icon->image.width),
                static_cast<float>(icon->image.height)
            );
            */

            return icon->data.advanceX;
        }

        return 0.0f;
    }

    // 输出字符串
    void textout(float x, float y, const wchar_t* text, size_t length)
    {
        for (size_t i = 0; i < length; ++i) {
            wchar_t ch = text[i];
            x += this->put_char(x, y, ch);
        }
    }

private:
    // 渲染字符
    byte_t* render_glyph(char_info& info, int ch, int& w, int& h)
    {
        int x, y;
        int advance, lsb;

        byte_t* image = stbtt_GetCodepointBitmap(handle(), 0, m_scale, ch, &w, &h, &x, &y);
        // int id        = stbtt_FindGlyphIndex(this->handle(), ch);
        // byte_t* image = stbtt_GetGlyphBitmapSubpixel(this->handle(), 0, m_scale, 0.0f, 0.0f, id, &w, &h, &x, &y);

        if (!image) {
            printf("freetype> rander character faild.\n");
            return nullptr;
        }

        stbtt_GetCodepointHMetrics(this->handle(), ch, &advance, &lsb);

        info.bearingX = static_cast<float>(round(static_cast<double>(x + lsb) * m_scale));
        info.bearingY = static_cast<float>(-y);
        info.advanceX = static_cast<float>(round(static_cast<double>(advance) * m_scale));
        info.advanceY = 0.0f; // 忽略

        return image;
    }

    double round(double value)
    {
        // return value + 0.5f;
        return value < 0.0f ? std::ceil(value - 0.5f) : std::floor(value + 0.5f);
    }

    // 生成一个字符
    int make_char(wchar_t ch)
    {
        byte_t* image;
        int w, h;
        char_info info;

        if (ch == 0x20) { // space
            image = render_glyph(info, 'A', w, h);
            if (!image) {
                return -1;
            }

            m_imagelist.insert(ch, w, h, GL_RGBA, nullptr, info);
        }
        else {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            image = render_glyph(info, ch, w, h);
            if (!image) {
                return -1;
            }

            // 转成 RGBA 数据
            std::vector<vec4ub> data(w * h);
            for (int i = 0; i < w * h; ++i) {
                data[i] = vec4ub(255, 255, 255, image[i]);
            }

            m_imagelist.insert(ch, w, h, GL_RGBA, &data[0], info);
        }

        stbtt_FreeBitmap(image, 0);

        return 0;
    }
};

} // end namespace graphics
} // end namespace cgl

#endif // GLFONT_HPP_20150405014710_STB
