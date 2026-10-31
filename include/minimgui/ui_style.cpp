#include "ui.h"

namespace minimgui {


//------------------------------------------------------------------------------

uiStyle::uiStyle()
{
    set_white_style(this);
}

void set_alpha_style(uiStyle* style)
{
    style->titleHeight = 32.0f;

    style->windowPadding = uiVec2(8.0f);
    style->itemSpace     = uiVec2(4.0f);

    style->framePadding = uiVec2(2.0f);

    style->sliderSize      = 16.0f;
    style->scrollWidth     = 8.0f;
    style->scrollSliderMin = 16.0f;

    // 标题栏背景色
    style->titleBackground[UI_COLOR_NORMAL]    = uiColor(96, 128, 160, 255);
    style->titleBackground[UI_COLOR_HOTLIGHT]  = uiColor(64, 128, 224, 255);
    style->titleBackground[UI_COLOR_HIGHLIGHT] = uiColor(64, 192, 255, 255);
    style->titleBackground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    //  标题栏前景色
    style->titleForeground[UI_COLOR_NORMAL]    = uiColor(224, 224, 224, 255);
    style->titleForeground[UI_COLOR_HOTLIGHT]  = uiColor(255, 255, 255, 255);
    style->titleForeground[UI_COLOR_HIGHLIGHT] = uiColor(224, 224, 224, 255);
    style->titleForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 窗口背景色
    //    style->windowBackground[UI_COLOR_NORMAL]    = uiColor(255, 255, 255, 192);
    //    style->windowBackground[UI_COLOR_HOTLIGHT]  = uiColor(255, 255, 255, 224);
    //    style->windowBackground[UI_COLOR_HIGHLIGHT] = uiColor(255, 255, 255, 192);
    //    style->windowBackground[UI_COLOR_DISABLE]   = uiColor(255, 255, 255, 255);

    // 窗口前景色
    style->windowForeground[UI_COLOR_NORMAL]    = uiColor(128, 128, 128, 255);
    style->windowForeground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 224, 255);
    style->windowForeground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 255, 255);
    style->windowForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 部件背景色
    //    style->widgetBackground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 32);
    //    style->widgetBackground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 0, 64);
    //    style->widgetBackground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 0, 32);
    //    style->widgetBackground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 部件前景色
    style->widgetForeground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 255);
    style->widgetForeground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 224, 255);
    style->widgetForeground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 128, 255);
    style->widgetForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 背景色 (文字、项...)
    style->background[UI_COLOR_NORMAL]    = uiColor(255, 255, 255, 255);
    style->background[UI_COLOR_HOTLIGHT]  = uiColor(224, 240, 255, 255);
    style->background[UI_COLOR_HIGHLIGHT] = uiColor(128, 192, 255, 255);
    style->background[UI_COLOR_DISABLE]   = uiColor(255, 255, 255, 255);

    // 前景色 (文字、项...)
    style->foreground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 255);
    style->foreground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 224, 255);
    style->foreground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 0, 255);
    style->foreground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);
}

void set_white_style(uiStyle* style)
{
    style->titleHeight  = 20.0f;         // 标题栏高度
    style->borderWidth  = 1.0f;          // 边框宽度
    style->triangleSize = 20.0f;         // 窗口右下角三角形大小

    style->windowPadding = uiVec2(8.0f); // 窗口边界空白
    style->framePadding  = uiVec2(2.0f); // 部件边框空白
    style->itemSpace     = uiVec2(4.0f); // 部件间距

    style->menuHeight  = 20.0f;          // 菜单高度
    style->menuPadding = uiVec2(2.0f);   // 菜单边框空白

    style->sliderSize      = 16.0f;      // 滑块大小
    style->scrollWidth     = 8.0f;       // 滚动条宽度
    style->scrollSliderMin = 16.0f;      // 滚动条滑块最小大小

    // uiColor(248, 248, 255, 255)
    // uiColor(64, 192, 255, 255);
    const uiColor unused = uiColor(255, 0, 255, 255);

    // 标题栏背景色
    style->titleBackground[UI_COLOR_NORMAL]    = uiColor(96, 128, 160, 255);
    style->titleBackground[UI_COLOR_HOTLIGHT]  = unused;
    style->titleBackground[UI_COLOR_HIGHLIGHT] = uiColor(64, 128, 224, 255);
    style->titleBackground[UI_COLOR_DISABLE]   = unused;

    // 标题栏前景色
    style->titleForeground[UI_COLOR_NORMAL]    = uiColor(224, 224, 224, 255);
    style->titleForeground[UI_COLOR_HOTLIGHT]  = unused;
    style->titleForeground[UI_COLOR_HIGHLIGHT] = uiColor(255, 255, 255, 255);
    style->titleForeground[UI_COLOR_DISABLE]   = unused;

    // 窗口背景色
    style->windowBackground = uiColor(255, 255, 255, 255);

    // 窗口前景色
    style->windowForeground[UI_COLOR_NORMAL]    = uiColor(128, 128, 128, 255);
    style->windowForeground[UI_COLOR_HOTLIGHT]  = unused;
    style->windowForeground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 128, 255);
    style->windowForeground[UI_COLOR_DISABLE]   = unused;

    // 部件背景色
    style->widgetBackground[UI_COLOR_NORMAL]    = uiColor(220, 220, 220, 255);
    style->widgetBackground[UI_COLOR_HOTLIGHT]  = uiColor(245, 245, 245, 255);
    style->widgetBackground[UI_COLOR_HIGHLIGHT] = uiColor(192, 192, 192, 255);
    style->widgetBackground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 部件前景色
    style->widgetForeground[UI_COLOR_NORMAL]    = uiColor(32, 32, 32, 255);
    style->widgetForeground[UI_COLOR_HOTLIGHT]  = uiColor(65, 105, 225, 255); // 皇家蓝
    style->widgetForeground[UI_COLOR_HIGHLIGHT] = uiColor(30, 144, 255, 255); // 道奇蓝
    style->widgetForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 轨道，滚动条、滑块背景
    style->trackBackground = uiColor(240, 240, 240, 255);

    // 滑块
    style->thumbForeground[UI_COLOR_NORMAL]    = uiColor(220, 220, 220, 255);
    style->thumbForeground[UI_COLOR_HOTLIGHT]  = uiColor(173, 216, 230, 255); // 浅蓝色 // uiColor(192, 192, 192, 255);
    style->thumbForeground[UI_COLOR_HIGHLIGHT] = uiColor(30, 144, 255, 255);  // 道奇蓝 // uiColor(169, 169, 169, 255);
    style->thumbForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 背景色 (文字、项...)
    style->background[UI_COLOR_NORMAL] = uiColor(255, 255, 255, 255);
    style->background[UI_COLOR_HOTLIGHT] =
        // uiColor(240, 248, 255, 255); // 爱丽丝蓝
        uiColor(135, 206, 250, 255); // 浅天蓝
    style->background[UI_COLOR_HIGHLIGHT] =
        uiColor(30, 144, 255, 255);  // 道奇蓝
    // uiColor(135, 206, 250, 255); // 浅天蓝
    // uiColor(135, 206, 235, 255); // 天蓝色
    style->background[UI_COLOR_DISABLE] = uiColor(255, 255, 255, 255);

    // 前景色 (文字、项...)
    style->foreground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 255);
    style->foreground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 255, 255);
    style->foreground[UI_COLOR_HIGHLIGHT] = uiColor(255, 255, 255, 255);
    style->foreground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);
}

void set_black_style(uiStyle* style)
{
    style->titleHeight = 32.0f;

    style->windowPadding = uiVec2(8.0f);
    style->itemSpace     = uiVec2(4.0f);

    style->framePadding = uiVec2(2.0f);

    style->sliderSize      = 16.0f;
    style->scrollWidth     = 8.0f;
    style->scrollSliderMin = 16.0f;

    // 标题栏背景色
    style->titleBackground[UI_COLOR_NORMAL]    = uiColor(96, 128, 160, 255);
    style->titleBackground[UI_COLOR_HOTLIGHT]  = uiColor(64, 128, 224, 255);
    style->titleBackground[UI_COLOR_HIGHLIGHT] = uiColor(64, 192, 255, 255);
    style->titleBackground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    //  标题栏前景色
    style->titleForeground[UI_COLOR_NORMAL]    = uiColor(224, 224, 224, 255);
    style->titleForeground[UI_COLOR_HOTLIGHT]  = uiColor(255, 255, 255, 255);
    style->titleForeground[UI_COLOR_HIGHLIGHT] = uiColor(224, 224, 224, 255);
    style->titleForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 窗口背景色
    //    style->windowBackground[UI_COLOR_NORMAL]    = uiColor(255, 255, 255, 192);
    //    style->windowBackground[UI_COLOR_HOTLIGHT]  = uiColor(255, 255, 255, 224);
    //    style->windowBackground[UI_COLOR_HIGHLIGHT] = uiColor(255, 255, 255, 192);
    //    style->windowBackground[UI_COLOR_DISABLE]   = uiColor(255, 255, 255, 255);

    // 窗口前景色
    style->windowForeground[UI_COLOR_NORMAL]    = uiColor(128, 128, 128, 255);
    style->windowForeground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 224, 255);
    style->windowForeground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 255, 255);
    style->windowForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 部件背景色
    //    style->widgetBackground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 32);
    //    style->widgetBackground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 0, 64);
    //    style->widgetBackground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 0, 32);
    //    style->widgetBackground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 部件前景色
    style->widgetForeground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 255);
    style->widgetForeground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 224, 255);
    style->widgetForeground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 128, 255);
    style->widgetForeground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);

    // 背景色 (文字、项...)
    style->background[UI_COLOR_NORMAL]    = uiColor(255, 255, 255, 255);
    style->background[UI_COLOR_HOTLIGHT]  = uiColor(224, 240, 255, 255);
    style->background[UI_COLOR_HIGHLIGHT] = uiColor(128, 192, 255, 255);
    style->background[UI_COLOR_DISABLE]   = uiColor(255, 255, 255, 255);

    // 前景色 (文字、项...)
    style->foreground[UI_COLOR_NORMAL]    = uiColor(0, 0, 0, 255);
    style->foreground[UI_COLOR_HOTLIGHT]  = uiColor(0, 0, 224, 255);
    style->foreground[UI_COLOR_HIGHLIGHT] = uiColor(0, 0, 0, 255);
    style->foreground[UI_COLOR_DISABLE]   = uiColor(128, 128, 128, 255);
}

} // namespace minimgui
