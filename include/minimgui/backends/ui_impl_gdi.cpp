#include "ui_impl_gdi.h"
#include "../ui_gfx.h"

#define PROPID ULONG

#include <windows.h>

namespace minimgui {

#if defined(UI_USE_UTF8)
const UINT CODE_PAGE = CP_UTF8;
#else
const UINT CODE_PAGE = CP_ACP;
#endif

static HDC _m_hdc     = nullptr;
static HFONT _m_hFont = nullptr;

#define TO_RECT(x, y, w, h) (int) x, (int) y, (int) (x + w), (int) (y + h)

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

static COLORREF toCOLORREF(const uiColor& color)
{
    COLORREF c = RGB(color.r, color.g, color.b);
    if (c == 0 && color.a != 255) {
        c = RGB(color.a, color.a, color.a);
    }
    return c;
}

//------------------------------------------------------------------------------

// 初始化界面环境
void on_begin_context(void* instance)
{
    HDC hdc  = (HDC) instance;
    _m_hdc   = hdc;
    _m_hFont = CreateFont(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, GB2312_CHARSET, 0, 0, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Microsoft YaHei"));

    SelectObject(hdc, _m_hFont);
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

    _m_hdc = nullptr;
    if (_m_hFont) {
        DeleteObject(_m_hFont);
        _m_hFont = nullptr;
    }
}

//------------------------------------------------------------------------------

// 用 AlphaBlend 填充半透明矩形
static BOOL FillRectAlpha(HDC hdc, int x, int y, int w, int h, uiColor color)
{
    if (!hdc || color.a == 0) {
        return TRUE; // 完全透明，无需绘制
    }

    if (w <= 0 || h <= 0) {
        return TRUE; // 空矩形
    }

    HDC hMemDC = CreateCompatibleDC(hdc);
    if (!hMemDC) {
        return FALSE;
    }

    // 创建 32 位 DIB，自带 Alpha 通道
    BITMAPINFO bmi              = { };
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = 1;
    bmi.bmiHeader.biHeight      = 1;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    COLORREF* pBits = nullptr;
    HBITMAP hBmp    = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**) &pBits, nullptr, 0);
    if (!hBmp || !pBits) {
        DeleteDC(hMemDC);
        return FALSE;
    }

    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);

    // 对像素进行 Alpha 预乘
    BYTE r   = (BYTE) ((color.b * color.a + 127) / 255);
    BYTE g   = (BYTE) ((color.g * color.a + 127) / 255);
    BYTE b   = (BYTE) ((color.r * color.a + 127) / 255);
    pBits[0] = RGB(r, g, b) | (color.a << 24);

    // 绘制
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    BOOL ok          = AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, 1, 1, bf);

    // 清理
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return ok;
}

static void draw_polygon(HDC hdc, const uiVec2* points, size_t size)
{
    if (!points || size == 0) {
        return;
    }

    MoveToEx(hdc, (int) points[0].x, (int) points[0].y, nullptr);
    for (size_t i = 1; i < size; ++i) {
        LineTo(hdc, (int) points[i].x, (int) points[i].y);
    }
    LineTo(hdc, (int) points[0].x, (int) points[0].y);
}

static void fill_polygon(HDC hdc, const uiVec2* points, size_t size)
{
    if (!points || size == 0) {
        return;
    }

    BeginPath(hdc);
    MoveToEx(hdc, (int) points[0].x, (int) points[0].y, nullptr);
    for (size_t i = 1; i < size; ++i) {
        LineTo(hdc, (int) points[i].x, (int) points[i].y);
    }
    EndPath(hdc);
    FillPath(hdc);
}

//------------------------------------------------------------------------------

// 绘制命令列表
void draw_cmd(const uiCommandList& cmdlist, void* instance)
{
    (void) instance;
    HDC hdc          = _m_hdc;
    HPEN hPen        = nullptr;
    HBRUSH hBrush    = nullptr;
    HPEN hOldPen     = (HPEN) GetCurrentObject(hdc, OBJ_PEN);
    HBRUSH hOldBrush = (HBRUSH) GetCurrentObject(hdc, OBJ_BRUSH);
    std::vector<RECT> clipstack;
    RECT rect;
    uiColor brushColor;

    HFONT hFont    = _m_hFont;
    HFONT hOldFont = (HFONT) SelectObject(hdc, hFont);

    const uiVec2* points = cmdlist.point_data();
    for (size_t cmdID = 0; cmdID < cmdlist.command_count(); ++cmdID) {
        const uiCommandList::command& cmd = cmdlist.commands(cmdID);

        switch (cmd.inst) {
        case uiCommandList::STROKE:
            if (hPen) {
                DeleteObject(hPen);
            }
            hPen = CreatePen(PS_SOLID, (int) cmd.stroke.width, cmd.stroke.color & 0x00FFFFFF); // toCOLORREF(cmd.stroke.color));
            break;
        case uiCommandList::FILL_COLOR:
            if (hBrush) {
                DeleteObject(hBrush);
            }
            hBrush     = CreateSolidBrush(cmd.fill.color & 0x00FFFFFF);
            brushColor = uiColor(
                RGBA_RED(cmd.fill.color),
                RGBA_GREEN(cmd.fill.color),
                RGBA_BLUE(cmd.fill.color),
                RGBA_ALPHA(cmd.fill.color)
            );
            break;
        case uiCommandList::FILL_IMAGE:
            break;
        case uiCommandList::LINE:
            SelectObject(hdc, hPen);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                MoveToEx(hdc, (int) points[i - 1].x, (int) points[i - 1].y, nullptr);
                LineTo(hdc, (int) points[i - 0].x, (int) points[i - 0].y);
            }
            break;
        case uiCommandList::RECT:
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                Rectangle(hdc, TO_RECT(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y));
            }
            break;
        case uiCommandList::RECT | uiCommandList::FILL_MODE:
            //*
            SelectObject(hdc, GetStockObject(NULL_PEN));
            SelectObject(hdc, hBrush);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                Rectangle(hdc, TO_RECT(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y));
            }
            /*/
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                FillRectAlpha(hdc, points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y, brushColor);
            }
            //*/

            break;
        case uiCommandList::ROUND_RECT:
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                RoundRect(
                    hdc,
                    TO_RECT(points[i - 2].x, points[i - 2].y, points[i - 1].x, points[i - 1].y),
                    points[i - 0].x, points[i - 0].y
                );
            }
            break;
        case uiCommandList::ROUND_RECT | uiCommandList::FILL_MODE:
            SelectObject(hdc, GetStockObject(NULL_PEN));
            SelectObject(hdc, hBrush);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                RoundRect(
                    hdc,
                    TO_RECT(points[i - 2].x, points[i - 2].y, points[i - 1].x, points[i - 1].y),
                    points[i - 0].x, points[i - 0].y
                );
            }
            break;
        case uiCommandList::ELLIPSE:
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                Ellipse(hdc, TO_RECT(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y));
            }
            break;
        case uiCommandList::ELLIPSE | uiCommandList::FILL_MODE:
            SelectObject(hdc, GetStockObject(NULL_PEN));
            SelectObject(hdc, hBrush);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 1; i < cmd.geom.count; i += 2) {
                Ellipse(hdc, TO_RECT(points[i - 1].x, points[i - 1].y, points[i - 0].x, points[i - 0].y));
            }
            break;
        case uiCommandList::TRIANGLE:
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                draw_polygon(hdc, points, 3);
                points += 3;
            }
            draw_polygon(hdc, points, 3);
            break;
        case uiCommandList::TRIANGLE | uiCommandList::FILL_MODE:
            SelectObject(hdc, GetStockObject(NULL_PEN));
            SelectObject(hdc, hBrush);
            points = cmdlist.point_data() + cmd.geom.index;
            for (size_t i = 2; i < cmd.geom.count; i += 3) {
                fill_polygon(hdc, points, 3);
                points += 3;
            }
            break;
        case uiCommandList::POLYGON:
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            points = cmdlist.point_data() + cmd.geom.index;
            draw_polygon(hdc, points, cmd.geom.count);
            break;
        case uiCommandList::POLYGON | uiCommandList::FILL_MODE:
            SelectObject(hdc, GetStockObject(NULL_PEN));
            SelectObject(hdc, hBrush);
            points = cmdlist.point_data() + cmd.geom.index;
            fill_polygon(hdc, points, cmd.geom.count);
            break;
        case uiCommandList::STRING: {
            SetTextColor(hdc, cmd.text.color);
            points              = cmdlist.point_data() + cmd.text.index;
            const uiString& str = cmdlist.strings(cmd.text.string);
            TextOutW(hdc, (int) points->x, (int) points->y, str.c_str(), str.length());
            break;
        }
        case uiCommandList::PUSH_CLIP_RESET:
            GetClipBox(hdc, &rect);
            clipstack.push_back(rect);
            {
                HRGN hRgn = CreateRectRgn(TO_RECT(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h));
                SelectClipRgn(hdc, hRgn);
                DeleteObject(hRgn);
            }
            break;
        case uiCommandList::PUSH_CLIP_INTERSECT:
            GetClipBox(hdc, &rect);
            clipstack.push_back(rect);
            {
                points     = cmdlist.point_data() + cmd.text.index;
                HRGN hRgn1 = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
                HRGN hRgn2 = CreateRectRgn(TO_RECT(cmd.clip.x, cmd.clip.y, cmd.clip.w, cmd.clip.h));
                CombineRgn(hRgn1, hRgn1, hRgn2, RGN_AND);
                SelectClipRgn(hdc, hRgn1);
                DeleteObject(hRgn1);
                DeleteObject(hRgn2);
            }
            break;
        case uiCommandList::POP_CLIP:
            if (!clipstack.empty()) {
                rect = clipstack.back();
                clipstack.pop_back();

                HRGN hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
                SelectClipRgn(hdc, hRgn);
                DeleteObject(hRgn);
            }
            else {
                SelectClipRgn(hdc, nullptr);
            }
            break;
        default:
            break;
        }
    }

    // 释放
    if (hPen) {
        DeleteObject(hPen);
    }
    if (hBrush) {
        DeleteObject(hBrush);
    }
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldFont);
}

// to keys
uiKey to_uiKey(uint32_t vkey)
{
    switch (vkey) {
    case VK_NEXT:
        break;
    case VK_NUMPAD0:
    case VK_PRINT:
    case VK_CLEAR:
    case VK_NUMPAD7:
        // VK_RETURN;
        break;
    }
}

// 返回字符宽度
float char_width(int ch)
{
    wchar_t w = ch;
    SIZE size;
    if (GetTextExtentPoint32W(_m_hdc, &w, 1, &size)) {
        return static_cast<float>(size.cx);
    }
    return 0;
}

// 返回字符高度
float char_height()
{
    SIZE size;
    if (GetTextExtentPoint32W(_m_hdc, L"A", 1, &size)) {
        return static_cast<float>(size.cy);
    }
    return 0;
}

// 返回文字宽度
float text_width(const char* str, size_t length)
{
    if (length == SIZE_MAX)
        length = strlen(str);
    SIZE size;
    std::wstring wstr = to_wstring(str, length);
    if (GetTextExtentPoint32W(_m_hdc, wstr.c_str(), wstr.length(), &size)) {
        return static_cast<float>(size.cx);
    }
    return 0;
}

float text_width(const wchar_t* str, size_t length)
{
    if (length == SIZE_MAX)
        length = wcslen(str);
    SIZE size;
    if (GetTextExtentPoint32W(_m_hdc, str, length, &size)) {
        return static_cast<float>(size.cx);
    }
    return 0;
}

#if 0

// 统计字符宽度列表
int count_char_widths(std::vector<float>& widths, const char* text, size_t length)
{
    if (!_m_hFont) {
        return 0;
    }

    std::wstring wstr = to_wstring(text, length);

    HDC hdc = CreateCompatibleDC(nullptr);
    SelectObject(hdc, _m_hFont);

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

    return 0;
}

// 统计字符宽度列表
int count_char_widths(std::vector<float>& widths, const wchar_t* text, size_t length)
{
    if (!_m_hFont) {
        return 0;
    }

    HDC hdc = CreateCompatibleDC(nullptr);
    SelectObject(hdc, _m_hFont);

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

    return 0;
}
#endif

} // namespace minimgui
