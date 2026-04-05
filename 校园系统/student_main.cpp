#include <graphics.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <easyx.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "head.h"

#define WIN_WIDTH     1200
#define WIN_HEIGHT    800
#define BTN_WIDTH     200
#define BTN_HEIGHT    50
#define INPUT_WIDTH   300
#define INPUT_HEIGHT  40
#define ITEM_WIDTH    1100
#define ITEM_HEIGHT   120

typedef enum {
    PAGE_STUDENT_HOME,
    PAGE_STUDENT_SEARCH
} StudentPageType;

static StudentPageType currentPage = PAGE_STUDENT_HOME;
static wchar_t searchText[100] = { 0 };
static bool isSearchFocus = false;
static int mouseX = 0, mouseY = 0;

// ------------------------------
// 画圆角矩形（和管理员风格统一）
// ------------------------------
void drawRoundRect(int x, int y, int w, int h, int radius, COLORREF lineColor, COLORREF fillColor) {
    setfillcolor(fillColor);
    setlinecolor(lineColor);
    fillroundrect(x, y, x + w, y + h, radius, radius);
}

// ------------------------------
// 画按钮（统一风格）
// ------------------------------
void drawStudentButton(int x, int y, int w, int h, const wchar_t* text, bool isHover) {
    COLORREF btnColor = isHover ? RGB(100, 180, 255) : RGB(70, 150, 230);
    drawRoundRect(x, y, w, h, 10, RGB(30, 100, 180), btnColor);
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, L"微软雅黑");
    int tx = x + (w - textwidth(text)) / 2;
    int ty = y + (h - textheight(text)) / 2;
    outtextxy(tx, ty, text);
}

// ------------------------------
// 搜索框（和管理员风格统一）
// ------------------------------
void drawSearchBox(int x, int y) {
    drawRoundRect(x, y, INPUT_WIDTH, INPUT_HEIGHT, 8, RGB(180, 180, 180), WHITE);
    settextcolor(RGB(100, 100, 100));
    settextstyle(24, 0, L"微软雅黑");
    outtextxy(x + 15, y + 10, L"🔍");

    if (wcslen(searchText) > 0) {
        settextcolor(BLACK);
        outtextxy(x + 50, y + 10, searchText);
    }
    else {
        settextcolor(RGB(150, 150, 150));
        outtextxy(x + 50, y + 10, L"输入活动名称搜索");
    }

    if (isSearchFocus) {
        int cx = x + 50 + textwidth(searchText);
        setlinecolor(BLACK);
        line(cx, y + 15, cx, y + INPUT_HEIGHT - 15);
    }
}

// ------------------------------
// 画活动卡片（按你的草图）
// ------------------------------
void drawActivityCard(int x, int y, const wchar_t* title, const wchar_t* info, const wchar_t* btnText) {
    drawRoundRect(x, y, ITEM_WIDTH, ITEM_HEIGHT, 8, RGB(200, 200, 200), WHITE);

    settextcolor(RGB(30, 60, 120));
    settextstyle(24, 0, L"微软雅黑");
    outtextxy(x + 20, y + 20, title);

    settextcolor(RGB(80, 80, 80));
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(x + 20, y + 60, info);

    bool hover = (mouseX >= x + ITEM_WIDTH - 120 && mouseX <= x + ITEM_WIDTH - 20 &&
        mouseY >= y + 40 && mouseY <= y + 90);
    drawStudentButton(x + ITEM_WIDTH - 120, y + 40, 100, 40, btnText, hover);
}

// ------------------------------
// 学生首页（按你的草图绘制）
// ------------------------------
void drawStudentHomePage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // 标题
    settextcolor(RGB(30, 60, 120));
    settextstyle(36, 0, L"微软雅黑");
    outtextxy(50, 30, L"校园活动管理系统");

    // 欢迎语
    settextstyle(48, 0, L"微软雅黑");
    outtextxy(50, 100, L"欢迎！");

    // 最新活动
    settextstyle(24, 0, L"微软雅黑");
    outtextxy(50, 180, L"最新活动");

    // 搜索框
    drawSearchBox(WIN_WIDTH - INPUT_WIDTH - 50, 175);

    // 活动卡片（按你草图）
    drawActivityCard(50, 230,
        L"学生 · 运动会志愿者",
        L"名称：运动会志愿者招募 | 时间：2025-10-20 | 名额：1/50",
        L"详情");

    drawActivityCard(50, 370,
        L"通用活动",
        L"名称：校园文化节 | 时间：2025-11-01 | 名额：1/20",
        L"报名");

    drawActivityCard(50, 510,
        L"管理员发布",
        L"名称：讲座活动 | 时间：2025-12-10 | 名额：1/100",
        L"查看");

    // 查看更多按钮
    bool moreHover = (mouseX >= 500 && mouseX <= 700 && mouseY >= 650 && mouseY <= 700);
    drawStudentButton(500, 650, 200, 50, L"查看更多活动", moreHover);
}

// ------------------------------
// 鼠标点击处理
// ------------------------------
void handleStudentMouseClick(int x, int y) {
    if (currentPage == PAGE_STUDENT_HOME) {
        // 搜索框
        if (x >= WIN_WIDTH - INPUT_WIDTH - 50 && x <= WIN_WIDTH - 50 && y >= 175 && y <= 215) {
            isSearchFocus = true;
            return;
        }
        // 查看更多
        if (x >= 500 && x <= 700 && y >= 650 && y <= 700) {
            MessageBoxW(GetHWnd(), L"进入活动列表", L"提示", MB_OK);
        }
        isSearchFocus = false;
    }
}

// ------------------------------
// 键盘输入
// ------------------------------
void handleStudentKeyInput(wchar_t ch) {
    if (!isSearchFocus) return;
    if (ch == VK_BACK) {
        int len = wcslen(searchText);
        if (len > 0) searchText[len - 1] = L'\0';
    }
    else if (wcslen(searchText) < 98) {
        searchText[wcslen(searchText)] = ch;
    }
}

// ------------------------------
// 学生主界面入口（和管理员 runAdminMainPage 完全一样）
// ------------------------------
void runStudentMainPage() {
    currentPage = PAGE_STUDENT_HOME;
    isSearchFocus = false;
    memset(searchText, 0, sizeof(searchText));

    ExMessage msg;
    bool running = true;

    while (running) {
        while (peekmessage(&msg)) {
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                handleStudentMouseClick(msg.x, msg.y);
            }
            else if (msg.message == WM_CHAR) {
                handleStudentKeyInput((wchar_t)msg.ch);
            }
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
                running = false;
            }
        }

        if (currentPage == PAGE_STUDENT_HOME) {
            drawStudentHomePage();
        }

        FlushBatchDraw();
        Sleep(10);
    }
}