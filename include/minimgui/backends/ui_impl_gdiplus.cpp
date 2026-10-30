#include "ui_impl_gdi.h"
#include "../ui_gfx.h"
#include "../ui_user.h"

#define PROPID ULONG

#include <windows.h>
#include <propidl.h>
#include <gdiplus.h>

#if defined(_MSC_VER)
    #pragma comment(lib, "gdiplus.lib")
#endif

namespace minimgui {

#if defined(UI_USE_UTF8)
const UINT CODE_PAGE = CP_UTF8;
#else
const UINT CODE_PAGE = CP_ACP;
#endif

// HDC _m_hdc                  = nullptr;
Gdiplus::Graphics* _m_graph = nullptr;
Gdiplus::Font* _m_font      = nullptr;

//------------------------------------------------------------------------------

static std::wstring to_wstring(const char* str, int length)
{
    int wlen = MultiByteToWideChar(CODE_PAGE, 0, str, length, NULL, 0);
    if (wlen <= 0)
        return 0;

    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CODE_PAGE, 0, str, length, &wstr[0], wlen);

    return wstr;
}

static std::wstring to_wstring(const std::string& str)
{
    return to_wstring(str.c_str(), str.length());
}

static Gdiplus::Color toColor(const uiColor& color)
{
    return Gdiplus::Color(color.a, color.r, color.g, color.b);
}

//------------------------------------------------------------------------------

// 初始化界面环境
void on_begin_context(void* instance)
{
    HDC hdc  = (HDC) instance;
    _m_graph = new Gdiplus::Graphics(hdc);
    _m_font  = new Gdiplus::Font(L"Microsoft YaHei", 16.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    _m_graph->SetCompositingMode(Gdiplus::CompositingModeSourceOver);      // 混合模式
    _m_graph->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed); // 混合质量
    _m_graph->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);           // 反锯齿
    _m_graph->SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);            // 像素偏移模式
    _m_graph->SetInterpolationMode(Gdiplus::InterpolationModeBilinear);    // 图形缩放质量

    // gdiplus 默认效果，总是重新调整前面的字符间距，导致绘制有误差。
    _m_graph->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
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

    if (_m_graph) {
        delete _m_graph;
        _m_graph = nullptr;
    }

    if (_m_font) {
        delete _m_font;
        _m_font = nullptr;
    }
}

//------------------------------------------------------------------------------

// 绘制命令列表
void draw_cmd(const uiCommandList& cmdlist, void* instance)
{
    (void) instance;

    Gdiplus::Pen pen(Gdiplus::Color::Black, 1.0f);
    Gdiplus::SolidBrush brush(Gdiplus::Color::White);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color::Black);
    std::vector<Gdiplus::RectF> clipstack;
    Gdiplus::RectF rect;

    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces); // 防止忽略末尾空格

    const uiVec2* points = cmdlist.point_data();
    for (size_t cmdID = 0; cmdID < cmdlist.command_count(); ++cmdID) {
        const uiCommandList::command& cmd = cmdlist.commands(cmdID);
        switch (cmd.inst) {
        case uiCommandList::STROKE:
            pen.SetColor(RedBlueSwap(cmd.stroke.color));
            pen.SetWidth(cmd.stroke.width);
            break;
        case uiCommandList::FILL_COLOR:
            brush.SetColor(RedBlueSwap(cmd.fill.color));
            break;
        case uiCommandList::FILL_IMAGE:
            break;
        case uiCommandList::LINE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                _m_graph->DrawLine(&pen, points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                _m_graph->DrawRectangle(&pen, points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::RECT | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                _m_graph->FillRectangle(&brush, points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::ROUND_RECT:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                //                RoundRect(
                //                    hdc,
                //                    points[i - 2].x, points[i - 2].y,
                //                    points[i - 1].x, points[i - 1].y,
                //                    points[i - 0].x, points[i - 0].y
                //                );
            }

            break;
        case uiCommandList::ROUND_RECT | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                //                RoundRect(
                //                    hdc,
                //                    points[i - 2].x, points[i - 2].y,
                //                    points[i - 1].x, points[i - 1].y,
                //                    points[i - 0].x, points[i - 0].y
                //                );
            }

            break;
        case uiCommandList::ELLIPSE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                _m_graph->DrawEllipse(&pen, points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::ELLIPSE | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                _m_graph->FillEllipse(&brush, points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y);
            }
            break;
        case uiCommandList::TRIANGLE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                _m_graph->DrawPolygon(&pen, (Gdiplus::PointF*) &points[i - 2], 3);
            }
            break;
        case uiCommandList::TRIANGLE | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                _m_graph->FillPolygon(&brush, (Gdiplus::PointF*) &points[i - 2], 3);
            }
            break;
        case uiCommandList::POLYGON:
            points = cmdlist.point_data() + cmd.geom.index;
            _m_graph->DrawPolygon(&pen, (Gdiplus::PointF*) points, cmd.geom.count);
            break;
        case uiCommandList::POLYGON | uiCommandList::FILL_MODE:
            points = cmdlist.point_data() + cmd.geom.index;
            _m_graph->FillPolygon(&brush, (Gdiplus::PointF*) points, cmd.geom.count);
            break;
        case uiCommandList::STRING: // 绘制字符串
        {
            textBrush.SetColor(RedBlueSwap(cmd.text.color));
            points              = cmdlist.point_data() + cmd.text.index;
            const uiString& str = cmdlist.strings(cmd.text.string);
            #if defined(UNICODE)
            _m_graph->DrawString(
                str.c_str(), (int) str.length(),
                _m_font,
                Gdiplus::PointF(points[0].x, points[0].y),
                &format,
                &textBrush
            );
            #else
            std::wstring wstr = to_wstring(cmdlist.strings[cmd.count]);
            _m_graph->DrawString(
                wstr.c_str(), (int) wstr.length(),
                _m_font,
                Gdiplus::PointF(points[pos].x, points[pos].y),
                &format,
                &brush
            );
            #endif
            break;
        }
        case uiCommandList::PUSH_CLIP_RESET:
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
            {
                if (_m_graph->IsClipEmpty()) {
                    // 如果是空则不添加
                }
                else {
                    _m_graph->GetClipBounds(&rect);
                    clipstack.push_back(rect);
                }
                rect = Gdiplus::RectF(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
                _m_graph->SetClip(rect, Gdiplus::CombineModeReplace);
            }
            break;
        case uiCommandList::PUSH_CLIP_INTERSECT:
            // printf("push clip [%0.2f, %0.2f, %0.2f, %0.2f]\n", cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
            {
                if (_m_graph->IsClipEmpty()) {
                    // 如果是空则不添加
                }
                else {
                    _m_graph->GetClipBounds(&rect);
                    clipstack.push_back(rect);
                }
                rect = Gdiplus::RectF(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h);
                _m_graph->SetClip(rect, Gdiplus::CombineModeIntersect);
            }
            break;
        case uiCommandList::POP_CLIP:
            // printf("pop clip\n");
            {
                if (!clipstack.empty()) {
                    Gdiplus::RectF rect = clipstack.back();
                    clipstack.pop_back();
                    _m_graph->SetClip(rect);
                }
                else {
                    // 如果是空，则重置剪裁
                    _m_graph->ResetClip();
                }
            }
            break;
        default:
            break;
        }
    }
}

// 获取字符串的像素宽高
static uiVec2 measure_text(const wchar_t* text, size_t length)
{
    if (_m_graph) {
        Gdiplus::SizeF layoutSize(FLT_MAX, FLT_MAX);
        Gdiplus::SizeF size;
        Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
        format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
        _m_graph->MeasureString(
            text, length,
            _m_font,
            layoutSize,
            // Gdiplus::StringFormat::GenericTypographic(), // 会把末尾空格忽略
            &format,
            &size
        );

        return uiVec2(size.Width, size.Height);
    }
    return uiVec2();
}

// 返回字符宽度
float char_width(int ch)
{
    wchar_t w = ch;
    return measure_text(&w, 1).x;
}

// 返回字符高度
float char_height()
{
    return measure_text(L"A", 1).y;
}

// 返回文字宽度
float text_width(const char* text, size_t length)
{
    std::wstring wstr = to_wstring(text, length);
    return measure_text(wstr.c_str(), wstr.length()).x;
}

float text_width(const wchar_t* text, size_t length)
{
    return measure_text(text, length).x;
}

#if 0

// 统计字符宽度列表
int count_char_widths(std::vector<float>& widths, const char* text, size_t length)
{
    if (!_m_font) {
        return 0;
    }

    std::wstring wstr = to_wstring(text, length);

    // 将 GDI+ Font 转换为 HFONT
    LOGFONTW lf;
    _m_font->GetLogFontW(_m_graph, &lf);
    HFONT hFont = CreateFontIndirectW(&lf);

    HDC hdc = CreateCompatibleDC(nullptr);
    SelectObject(hdc, hFont);

    // 批量获取字符宽度
    widths.resize(wstr.length());
    for (size_t i = 0; i < length; ++i) {
        /*
        int n;
        GetCharWidth32W(hdc, wstr[i], wstr[i], &n);
        widths[i] = static_cast<float>(n);
        /*/
        ABCFLOAT abc;
        GetCharABCWidthsFloatW(hdc, wstr[i], wstr[i], &abc);
        // widths[i] = abc.abcfA + abc.abcfB + abc.abcfC;
        widths[i] = abc.abcfB;
        //*/
    }

    DeleteDC(hdc);
    DeleteObject(hFont);

    return 0;
}

// 统计字符宽度列表
int count_char_widths(std::vector<float>& widths, const wchar_t* text, size_t length)
{
    if (!_m_font) {
        return 0;
    }

    // 将 GDI+ Font 转换为 HFONT
    LOGFONTW lf;
    _m_font->GetLogFontW(_m_graph, &lf);
    HFONT hFont = CreateFontIndirectW(&lf);

    HDC hdc = CreateCompatibleDC(nullptr);
    SelectObject(hdc, hFont);

    // 批量获取字符宽度
    widths.resize(length);
    for (size_t i = 0; i < length; ++i) {
        /*
        int n;
        GetCharWidth32W(hdc, text[i], text[i], &n);
        widths[i] = static_cast<float>(n);
        /*/
        ABCFLOAT abc;
        GetCharABCWidthsFloatW(hdc, text[i], text[i], &abc);
        // widths[i] = abc.abcfA + abc.abcfB + abc.abcfC;
        widths[i] = abc.abcfB;
        //*/
    }

    DeleteDC(hdc);
    DeleteObject(hFont);

    return 0;
}
#endif

} // namespace minimgui
