#include <graphics.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <easyx.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "head.h"

// ==================== 常量定义 ====================
#define WIN_WIDTH     1200
#define WIN_HEIGHT    800
#define BTN_WIDTH     200
#define BTN_HEIGHT    50
#define CARD_WIDTH    350
#define CARD_HEIGHT   180

// 活动卡片数据结构
typedef struct {
    wchar_t title[50];
    wchar_t location[50];
    wchar_t tag[20];
    wchar_t count[30];
    int current;
    int max;
} ActivityCard;

// 全局活动卡片数据（模拟数据）
ActivityCard activities[] = {
    {L"运动会", L"操场", L"体育", L"报名人数：", 25, 50},
    {L"班级活动", L"教室A101", L"班级", L"报名人数：", 8, 30},
    {L"社团活动", L"活动中心", L"社团", L"报名人数：", 15, 40},
    {L"志愿者活动", L"社区中心", L"公益", L"报名人数：", 12, 25}
};
int activityCount = sizeof(activities) / sizeof(ActivityCard);

// 热门活动数据
typedef struct {
    wchar_t name[30];
} HotActivity;

HotActivity hotActivities[] = {
    {L"运动会"},
    {L"班级活动"},
    {L"社团活动"},
    {L"志愿者活动"}
};
int hotCount = sizeof(hotActivities) / sizeof(HotActivity);

// 全局变量
static int mouseX = 0, mouseY = 0;

// ==================== 工具函数 ====================
// 画圆角矩形
void drawRoundRect(int x, int y, int w, int h, int radius, COLORREF lineColor, COLORREF fillColor) {
    setfillcolor(fillColor);
    setlinecolor(lineColor);
    fillroundrect(x, y, x + w, y + h, radius, radius);
}

// 画按钮
void drawBtn(int x, int y, int w, int h, const wchar_t* text, bool isHover, COLORREF color) {
    COLORREF btnColor = isHover ? RGB(100, 180, 255) : color;
    drawRoundRect(x, y, w, h, 8, RGB(150, 150, 150), btnColor);
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(18, 0, L"微软雅黑");
    int tx = x + (w - textwidth(text)) / 2;
    int ty = y + (h - textheight(text)) / 2;
    outtextxy(tx, ty, text);
}

// 画活动卡片
void drawActivityCard(int x, int y, ActivityCard* card, bool isHover) {
    // 卡片背景
    COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
    drawRoundRect(x, y, CARD_WIDTH, CARD_HEIGHT, 10, RGB(200, 200, 200), bgColor);

    // 活动标题
    settextcolor(RGB(30, 60, 120));
    settextstyle(22, 0, L"微软雅黑");
    outtextxy(x + 15, y + 15, card->title);

    // 地点信息
    settextcolor(RGB(100, 100, 100));
    settextstyle(16, 0, L"微软雅黑");
    wchar_t locationText[100];
    swprintf(locationText, 100, L"地点：%s", card->location);
    outtextxy(x + 15, y + 55, locationText);

    // 标签（圆角小标签）
    drawRoundRect(x + 15, y + 85, 60, 25, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(x + 22, y + 90, card->tag);

    // 报名人数
    wchar_t countText[100];
    swprintf(countText, 100, L"%s %d/%d", card->count, card->current, card->max);
    settextcolor(RGB(80, 80, 80));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy(x + 15, y + 125, countText);

    // 详情按钮
    bool btnHover = (mouseX >= x + CARD_WIDTH - 70 && mouseX <= x + CARD_WIDTH - 15 &&
        mouseY >= y + CARD_HEIGHT - 40 && mouseY <= y + CARD_HEIGHT - 15);
    COLORREF btnColor = btnHover ? RGB(100, 180, 255) : RGB(70, 150, 230);
    drawRoundRect(x + CARD_WIDTH - 70, y + CARD_HEIGHT - 40, 55, 28, 5, RGB(50, 120, 200), btnColor);
    settextcolor(WHITE);
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(x + CARD_WIDTH - 65, y + CARD_HEIGHT - 36, L"详情");
}

// 画热门活动项
void drawHotActivityItem(int x, int y, const wchar_t* name, int index) {
    // 序号圆圈
    setfillcolor(RGB(70, 150, 230));
    setlinecolor(RGB(70, 150, 230));
    fillcircle(x + 15, y + 15, 12);

    settextcolor(WHITE);
    settextstyle(14, 0, L"微软雅黑");
    wchar_t num[5];
    swprintf(num, 5, L"%d", index + 1);
    outtextxy(x + 15 - textwidth(num) / 2, y + 15 - textheight(num) / 2, num);

    // 活动名称
    settextcolor(RGB(50, 50, 50));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy(x + 40, y + 8, name);

    // 分割线
    setlinecolor(RGB(230, 230, 230));
    line(x, y + 32, x + 250, y + 32);
}

// ==================== 主界面绘制 ====================
void drawStudentHomePage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // ===== 顶部标题栏 =====
    // 标题
    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    // 欢迎语
    settextstyle(20, 0, L"微软雅黑");
    settextcolor(RGB(80, 80, 80));
    outtextxy(40, 70, L"欢迎，同学！");

    // ===== 左侧：热门活动推荐 =====
    settextstyle(22, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(40, 130, L"热门活动推荐");

    // 热门活动列表背景
    drawRoundRect(40, 165, 280, 320, 10, RGB(220, 220, 220), RGB(255, 255, 255));

    // 绘制热门活动项
    for (int i = 0; i < hotCount && i < 6; i++) {
        drawHotActivityItem(50, 175 + i * 48, hotActivities[i].name, i);
    }

    // ===== 中间：活动卡片列表 =====
    settextstyle(22, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(350, 130, L"推荐活动");

    // 绘制活动卡片（一行两个）
    int cardStartX = 350;
    int cardStartY = 170;
    int cardGap = 30;

    for (int i = 0; i < activityCount && i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int x = cardStartX + col * (CARD_WIDTH + cardGap);
        int y = cardStartY + row * (CARD_HEIGHT + cardGap);

        bool isHover = (mouseX >= x && mouseX <= x + CARD_WIDTH && mouseY >= y && mouseY <= y + CARD_HEIGHT);
        drawActivityCard(x, y, &activities[i], isHover);
    }

    // ===== 右侧：个人快捷入口 =====
    int rightX = WIN_WIDTH - 230;

    // 个人中心卡片
    drawRoundRect(rightX, 130, 200, 350, 10, RGB(220, 220, 220), RGB(255, 255, 255));

    settextcolor(RGB(30, 60, 120));
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(rightX + 50, 150, L"个人中心");

    // 分隔线
    setlinecolor(RGB(230, 230, 230));
    line(rightX + 15, 175, rightX + 185, 175);

    // 个人头像（圆形）
    setfillcolor(RGB(70, 150, 230));
    setlinecolor(RGB(70, 150, 230));
    fillcircle(rightX + 100, 215, 35);
    settextcolor(WHITE);
    settextstyle(24, 0, L"微软雅黑");
    outtextxy(rightX + 92, 205, L"我");

    // 用户名
    settextcolor(RGB(50, 50, 50));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy(rightX + 60, 265, L"学生用户");

    // 我的活动按钮
    bool myActHover = (mouseX >= rightX + 25 && mouseX <= rightX + 175 && mouseY >= 300 && mouseY <= 340);
    drawBtn(rightX + 25, 300, 150, 40, L"我的活动", myActHover, RGB(70, 150, 230));

    // 我的勋章按钮
    bool medalHover = (mouseX >= rightX + 25 && mouseX <= rightX + 175 && mouseY >= 355 && mouseY <= 395);
    drawBtn(rightX + 25, 355, 150, 40, L"我的勋章", medalHover, RGB(100, 180, 100));

    // 个人设置按钮
    bool settingHover = (mouseX >= rightX + 25 && mouseX <= rightX + 175 && mouseY >= 410 && mouseY <= 450);
    drawBtn(rightX + 25, 410, 150, 40, L"个人设置", settingHover, RGB(150, 150, 150));

    // ===== 底部：查看更多活动按钮 =====
    bool moreHover = (mouseX >= WIN_WIDTH / 2 - 100 && mouseX <= WIN_WIDTH / 2 + 100 &&
        mouseY >= WIN_HEIGHT - 60 && mouseY <= WIN_HEIGHT - 20);
    drawBtn(WIN_WIDTH / 2 - 100, WIN_HEIGHT - 60, 200, 40, L"查看更多活动", moreHover, RGB(70, 150, 230));
}

// ==================== 鼠标点击处理 ====================
void handleMouseClick(int x, int y) {
    // 检查底部"查看更多活动"按钮
    if (x >= WIN_WIDTH / 2 - 100 && x <= WIN_WIDTH / 2 + 100 &&
        y >= WIN_HEIGHT - 60 && y <= WIN_HEIGHT - 20) {
        MessageBoxW(GetHWnd(), L"更多活动功能开发中...", L"提示", MB_OK);
        return;
    }

    // 检查右侧个人中心按钮
    int rightX = WIN_WIDTH - 230;

    // 我的活动按钮
    if (x >= rightX + 25 && x <= rightX + 175 && y >= 300 && y <= 340) {
        MessageBoxW(GetHWnd(), L"我的活动功能开发中...", L"提示", MB_OK);
        return;
    }

    // 我的勋章按钮
    if (x >= rightX + 25 && x <= rightX + 175 && y >= 355 && y <= 395) {
        MessageBoxW(GetHWnd(), L"我的勋章功能开发中...", L"提示", MB_OK);
        return;
    }

    // 个人设置按钮
    if (x >= rightX + 25 && x <= rightX + 175 && y >= 410 && y <= 450) {
        MessageBoxW(GetHWnd(), L"个人设置功能开发中...", L"提示", MB_OK);
        return;
    }

    // 检查活动卡片的详情按钮
    int cardStartX = 350;
    int cardStartY = 170;
    int cardGap = 30;

    for (int i = 0; i < activityCount && i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int cardX = cardStartX + col * (CARD_WIDTH + cardGap);
        int cardY = cardStartY + row * (CARD_HEIGHT + cardGap);

        // 详情按钮区域
        int btnX = cardX + CARD_WIDTH - 70;
        int btnY = cardY + CARD_HEIGHT - 40;
        if (x >= btnX && x <= btnX + 55 && y >= btnY && y <= btnY + 28) {
            wchar_t msg[100];
            swprintf(msg, 100, L"查看活动详情：%s", activities[i].title);
            MessageBoxW(GetHWnd(), msg, L"活动详情", MB_OK);
            return;
        }
    }
}

// ==================== 学生主界面入口 ====================
void runStudentMainPage() {
    ExMessage msg;
    bool running = true;

    // 初始化鼠标位置
    mouseX = 0;
    mouseY = 0;

    while (running) {
        // 处理消息
        while (peekmessage(&msg)) {
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                handleMouseClick(msg.x, msg.y);
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    running = false;  // 按ESC返回登录页
                }
            }
        }

        // 绘制界面
        drawStudentHomePage();

        FlushBatchDraw();
        Sleep(16);  // 约60帧
    }
}