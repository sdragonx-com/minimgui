#include "ui_font.h"
#include "ui_imagelist.h"

#include <stb/stb_truetype.h>

namespace minimgui {

// 字形矩阵
struct char_info
{
    tile_image* image;

    float bearingX; // character offset
    float bearingY;
    float advanceX; // the offset of the next position
    float advanceY;
};

struct uiFont
{
    std::string m_name;              // 字体名称
    std::vector<uint8_t> m_fontData; // 字体数据
    stbtt_fontinfo m_font;           // 字体
    float m_scale;                   // 字体缩放
    float m_size;                    // 字体大小

    uiImageList<uint32_t> m_imagelist;
};

static std::map<std::string, uiFont> _m_fonts;


// 加载字体
uiFont* load_font(const char* fontFile, float size)
{
}

// 从内存加载字体
uiFont* load_font(const char* fontName, const void* fontData, float size)
{
}

// 获取当前字体
uiFont* get_font()
{
}

// 设置当前字体
void set_font(uiFont* font)
{
}

// 获得字符宽度
float char_width1(wchar_t ch)
{
}

// 获取字符高度
float char_height1()
{
}

// 获取字符串宽度 (UTF-8)
float text_width1(const unsigned char* text, size_t length)
{
}

// 输出单行文字
void textout1(float x, float y, const char* str, size_t length)
{
}

} // namespace minimgui
