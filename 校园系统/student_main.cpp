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
#define CARD_WIDTH    330      // 稍微缩小卡片宽度
#define CARD_HEIGHT   180

// 页面状态枚举
typedef enum {
    PAGE_HOME,
    PAGE_DETAIL
} StudentPageState;

// 活动卡片数据结构
typedef struct {
    int id;
    wchar_t title[50];
    wchar_t location[50];
    wchar_t tag[20];
    wchar_t count[30];
    int current;
    int max;
    // 详情页额外信息
    wchar_t time[50];
    wchar_t organizer[50];
    wchar_t requirement[200];
    wchar_t description[300];
} ActivityCard;

// 全局活动卡片数据（模拟数据）
ActivityCard activities[] = {
    {
        1, L"运动会", L"操场", L"体育", L"报名人数：", 25, 50,
        L"2025年10月20日 9:00-17:00", L"体育部", L"身体健康，热爱运动",
        L"一年一度的校园运动会，包含田径、球类等多个项目，欢迎同学们踊跃报名！"
    },
    {
        2, L"班级活动", L"教室A101", L"班级", L"报名人数：", 8, 30,
        L"2025年10月25日 14:00-16:00", L"班长", L"本班同学",
        L"班级团建活动，增进同学感情，有游戏和零食。"
    },
    {
        3, L"社团活动", L"活动中心", L"社团", L"报名人数：", 15, 40,
        L"2025年11月1日 15:00-18:00", L"社团联合会", L"对社团活动感兴趣",
        L"各社团联合举办的大型活动，有表演、互动游戏等精彩内容。"
    },
    {
        4, L"志愿者活动", L"社区中心", L"公益", L"报名人数：", 12, 25,
        L"2025年11月10日 8:00-12:00", L"志愿者协会", L"有爱心，乐于助人",
        L"社区志愿服务，帮助老人和儿童，获得志愿时长认证。"
    }
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
static StudentPageState currentPage = PAGE_HOME;
static int currentActivityId = 0;  // 当前查看的活动ID
static bool isEnrolled = false;     // 当前用户是否已报名（模拟）

// 获取当前活动
ActivityCard* getCurrentActivity() {
    for (int i = 0; i < activityCount; i++) {
        if (activities[i].id == currentActivityId) {
            return &activities[i];
        }
    }
    return NULL;
}

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

// 画信息行
void drawInfoRow(int x, int y, const wchar_t* label, const wchar_t* value, COLORREF labelColor, COLORREF valueColor) {
    settextcolor(labelColor);
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(x, y, label);
    settextcolor(valueColor);
    outtextxy(x + 100, y, value);
}

// 画活动卡片（首页用）
void drawActivityCard(int x, int y, ActivityCard* card, bool isHover) {
    // 卡片背景
    COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
    drawRoundRect(x, y, CARD_WIDTH, CARD_HEIGHT, 10, RGB(200, 200, 200), bgColor);

    // 活动标题
    settextcolor(RGB(30, 60, 120));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + 15, y + 15, card->title);

    // 地点信息
    settextcolor(RGB(100, 100, 100));
    settextstyle(14, 0, L"微软雅黑");
    wchar_t locationText[100];
    swprintf(locationText, 100, L"地点：%s", card->location);
    outtextxy(x + 15, y + 55, locationText);

    // 标签（圆角小标签）
    drawRoundRect(x + 15, y + 85, 60, 25, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(12, 0, L"微软雅黑");
    outtextxy(x + 22, y + 90, card->tag);

    // 报名人数
    wchar_t countText[100];
    swprintf(countText, 100, L"%s %d/%d", card->count, card->current, card->max);
    settextcolor(RGB(80, 80, 80));
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(x + 15, y + 125, countText);

    // 详情按钮
    bool btnHover = (mouseX >= x + CARD_WIDTH - 70 && mouseX <= x + CARD_WIDTH - 15 &&
        mouseY >= y + CARD_HEIGHT - 40 && mouseY <= y + CARD_HEIGHT - 15);
    COLORREF btnColor = btnHover ? RGB(100, 180, 255) : RGB(70, 150, 230);
    drawRoundRect(x + CARD_WIDTH - 70, y + CARD_HEIGHT - 40, 55, 28, 5, RGB(50, 120, 200), btnColor);
    settextcolor(WHITE);
    settextstyle(12, 0, L"微软雅黑");
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
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(x + 40, y + 8, name);

    // 分割线
    setlinecolor(RGB(230, 230, 230));
    line(x, y + 32, x + 250, y + 32);
}

// ==================== 首页绘制 ====================
void drawStudentHomePage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // ===== 顶部标题栏 =====
    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    settextstyle(18, 0, L"微软雅黑");
    settextcolor(RGB(80, 80, 80));
    outtextxy(40, 70, L"欢迎，同学！");

    // ===== 左侧：热门活动推荐 =====
    settextstyle(20, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(40, 120, L"热门活动推荐");

    drawRoundRect(40, 155, 260, 320, 10, RGB(220, 220, 220), RGB(255, 255, 255));

    for (int i = 0; i < hotCount && i < 6; i++) {
        drawHotActivityItem(50, 165 + i * 48, hotActivities[i].name, i);
    }

    // ===== 中间：活动卡片列表（调整位置，避免被右侧挡住）=====
    settextstyle(20, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(330, 120, L"推荐活动");

    // 调整活动卡片起始位置（左移，避免被右侧个人中心挡住）
    int cardStartX = 330;
    int cardStartY = 160;
    int cardGap = 25;

    for (int i = 0; i < activityCount && i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int x = cardStartX + col * (CARD_WIDTH + cardGap);
        int y = cardStartY + row * (CARD_HEIGHT + cardGap);

        bool isHover = (mouseX >= x && mouseX <= x + CARD_WIDTH && mouseY >= y && mouseY <= y + CARD_HEIGHT);
        drawActivityCard(x, y, &activities[i], isHover);
    }

    // ===== 右侧：个人快捷入口（调整宽度和位置，不再遮挡）=====
    int rightX = WIN_WIDTH - 210;  // 移到更右边
    int rightW = 190;              // 宽度缩小

    drawRoundRect(rightX, 120, rightW, 370, 10, RGB(220, 220, 220), RGB(255, 255, 255));

    settextcolor(RGB(30, 60, 120));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy(rightX + (rightW - textwidth(L"个人中心")) / 2, 140, L"个人中心");

    setlinecolor(RGB(230, 230, 230));
    line(rightX + 10, 165, rightX + rightW - 10, 165);

    // 个人头像（圆形）
    setfillcolor(RGB(70, 150, 230));
    setlinecolor(RGB(70, 150, 230));
    fillcircle(rightX + rightW / 2, 205, 30);
    settextcolor(WHITE);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(rightX + rightW / 2 - 10, 197, L"我");

    // 用户名
    settextcolor(RGB(50, 50, 50));
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(rightX + (rightW - textwidth(L"学生用户")) / 2, 250, L"学生用户");

    // 我的活动按钮
    bool myActHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 280 && mouseY <= 315);
    drawBtn(rightX + 20, 280, rightW - 40, 35, L"我的活动", myActHover, RGB(70, 150, 230));

    // 我的勋章按钮
    bool medalHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 325 && mouseY <= 360);
    drawBtn(rightX + 20, 325, rightW - 40, 35, L"我的勋章", medalHover, RGB(100, 180, 100));

    // 个人设置按钮
    bool settingHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 370 && mouseY <= 405);
    drawBtn(rightX + 20, 370, rightW - 40, 35, L"个人设置", settingHover, RGB(150, 150, 150));

    // ===== 底部：查看更多活动按钮 =====
    bool moreHover = (mouseX >= WIN_WIDTH / 2 - 100 && mouseX <= WIN_WIDTH / 2 + 100 &&
        mouseY >= WIN_HEIGHT - 60 && mouseY <= WIN_HEIGHT - 20);
    drawBtn(WIN_WIDTH / 2 - 100, WIN_HEIGHT - 60, 200, 40, L"查看更多活动", moreHover, RGB(70, 150, 230));
}

// ==================== 活动详情页绘制 ====================
void drawActivityDetailPage() {
    ActivityCard* activity = getCurrentActivity();
    if (activity == NULL) {
        currentPage = PAGE_HOME;
        return;
    }

    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // ===== 顶部标题栏 =====
    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    // 返回按钮（左上角）
    bool backHover = (mouseX >= 40 && mouseX <= 120 && mouseY >= 70 && mouseY <= 105);
    drawBtn(40, 70, 80, 35, L"← 返回", backHover, RGB(100, 100, 100));

    // ===== 活动标题 =====
    settextcolor(RGB(30, 60, 120));
    settextstyle(36, 0, L"微软雅黑");
    wchar_t titleText[100];
    swprintf(titleText, 100, L"%s", activity->title);
    outtextxy((WIN_WIDTH - textwidth(titleText)) / 2, 80, titleText);

    // 活动标签
    drawRoundRect((WIN_WIDTH - 80) / 2, 130, 80, 30, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy((WIN_WIDTH - textwidth(activity->tag)) / 2, 137, activity->tag);

    // ===== 活动信息卡片 =====
    int infoCardX = 100;
    int infoCardY = 180;
    int infoCardW = 1000;
    int infoCardH = 500;

    drawRoundRect(infoCardX, infoCardY, infoCardW, infoCardH, 10, RGB(200, 200, 200), RGB(255, 255, 255));

    int lineY = infoCardY + 50;
    int leftX = infoCardX + 50;
    int rightX = infoCardX + 550;

    // 活动时间
    wchar_t timeLabel[50] = L"活动时间：";
    wchar_t timeValue[100];
    swprintf(timeValue, 100, L"%s", activity->time);
    drawInfoRow(leftX, lineY, timeLabel, timeValue, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    // 活动地点
    wchar_t locationLabel[50] = L"活动地点：";
    wchar_t locationValue[100];
    swprintf(locationValue, 100, L"%s", activity->location);
    drawInfoRow(leftX, lineY, locationLabel, locationValue, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    // 组织者
    wchar_t organizerLabel[50] = L"组织者：";
    wchar_t organizerValue[100];
    swprintf(organizerValue, 100, L"%s", activity->organizer);
    drawInfoRow(leftX, lineY, organizerLabel, organizerValue, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    // 要求
    wchar_t requirementLabel[50] = L"要求：";
    wchar_t requirementValue[200];
    swprintf(requirementValue, 200, L"%s", activity->requirement);
    drawInfoRow(leftX, lineY, requirementLabel, requirementValue, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 55;

    // 报名情况
    wchar_t enrollLabel[50] = L"报名情况：";
    wchar_t enrollValue[100];
    swprintf(enrollValue, 100, L"%d / %d 人", activity->current, activity->max);
    drawInfoRow(leftX, lineY, enrollLabel, enrollValue, RGB(80, 80, 80), RGB(50, 150, 50));
    lineY += 45;

    // 活动描述标题
    settextcolor(RGB(30, 60, 120));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(leftX, lineY, L"活动描述：");
    lineY += 35;

    // 活动描述内容（自动换行）
    settextcolor(RGB(80, 80, 80));
    settextstyle(16, 0, L"微软雅黑");
    wchar_t* desc = activity->description;
    int descLen = wcslen(desc);
    int maxCharsPerLine = 45;
    int lineCount = (descLen + maxCharsPerLine - 1) / maxCharsPerLine;
    for (int i = 0; i < lineCount && i < 5; i++) {
        wchar_t lineText[100] = { 0 };
        int start = i * maxCharsPerLine;
        int len = (start + maxCharsPerLine < descLen) ? maxCharsPerLine : descLen - start;
        wcsncpy(lineText, desc + start, len);
        outtextxy(leftX, lineY + i * 28, lineText);
    }

    // ===== 报名状态和按钮 =====
    int btnY = infoCardY + infoCardH - 60;
    int btnX = (WIN_WIDTH - 200) / 2;

    // 显示报名状态
    settextstyle(18, 0, L"微软雅黑");
    if (isEnrolled) {
        settextcolor(RGB(50, 150, 50));
        outtextxy(btnX - 120, btnY + 12, L"? 您已报名此活动");
    }
    else {
        if (activity->current >= activity->max) {
            settextcolor(RGB(200, 50, 50));
            outtextxy(btnX - 120, btnY + 12, L"? 活动已满，不可报名");
        }
        else {
            settextcolor(RGB(150, 150, 150));
            outtextxy(btnX - 120, btnY + 12, L"● 未报名");
        }
    }

    // 报名/取消报名按钮
    bool actionHover = (mouseX >= btnX && mouseX <= btnX + 200 && mouseY >= btnY && mouseY <= btnY + 45);

    if (isEnrolled) {
        // 已报名状态：显示"取消报名"
        drawBtn(btnX, btnY, 200, 45, L"取消报名", actionHover, RGB(200, 100, 100));
    }
    else {
        // 未报名状态：显示"报名"
        if (activity->current >= activity->max) {
            // 活动已满，按钮置灰
            COLORREF grayColor = RGB(180, 180, 180);
            drawRoundRect(btnX, btnY, 200, 45, 8, RGB(150, 150, 150), grayColor);
            settextcolor(WHITE);
            setbkmode(TRANSPARENT);
            settextstyle(18, 0, L"微软雅黑");
            outtextxy(btnX + (200 - textwidth(L"报名")) / 2, btnY + 12, L"报名");
        }
        else {
            drawBtn(btnX, btnY, 200, 45, L"报名", actionHover, RGB(70, 150, 230));
        }
    }
}

// ==================== 详情页鼠标点击处理 ====================
void handleDetailMouseClick(int x, int y) {
    ActivityCard* activity = getCurrentActivity();

    // 返回按钮
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }

    // 报名/取消报名按钮
    int btnX = (WIN_WIDTH - 200) / 2;
    int infoCardY = 180;
    int infoCardH = 500;
    int btnY = infoCardY + infoCardH - 60;

    if (x >= btnX && x <= btnX + 200 && y >= btnY && y <= btnY + 45) {
        if (isEnrolled) {
            // 取消报名
            isEnrolled = false;
            // 更新报名人数
            if (activity != NULL && activity->current > 0) {
                activity->current--;
            }
            MessageBoxW(GetHWnd(), L"已取消报名", L"提示", MB_OK);
        }
        else {
            // 报名
            if (activity != NULL && activity->current < activity->max) {
                isEnrolled = true;
                activity->current++;
                MessageBoxW(GetHWnd(), L"报名成功！", L"提示", MB_OK);
            }
            else {
                MessageBoxW(GetHWnd(), L"活动已满，无法报名", L"提示", MB_OK);
            }
        }
    }
}

// ==================== 首页鼠标点击处理 ====================
void handleHomeMouseClick(int x, int y) {
    // 检查底部"查看更多活动"按钮
    if (x >= WIN_WIDTH / 2 - 100 && x <= WIN_WIDTH / 2 + 100 &&
        y >= WIN_HEIGHT - 60 && y <= WIN_HEIGHT - 20) {
        MessageBoxW(GetHWnd(), L"更多活动功能开发中...", L"提示", MB_OK);
        return;
    }

    // 检查右侧个人中心按钮（调整后的位置）
    int rightX = WIN_WIDTH - 210;
    int rightW = 190;

    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 280 && y <= 315) {
        MessageBoxW(GetHWnd(), L"我的活动功能开发中...", L"提示", MB_OK);
        return;
    }

    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 325 && y <= 360) {
        MessageBoxW(GetHWnd(), L"我的勋章功能开发中...", L"提示", MB_OK);
        return;
    }

    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 370 && y <= 405) {
        MessageBoxW(GetHWnd(), L"个人设置功能开发中...", L"提示", MB_OK);
        return;
    }

    // 检查活动卡片的详情按钮
    int cardStartX = 330;
    int cardStartY = 160;
    int cardGap = 25;

    for (int i = 0; i < activityCount && i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int cardX = cardStartX + col * (CARD_WIDTH + cardGap);
        int cardY = cardStartY + row * (CARD_HEIGHT + cardGap);

        int btnX = cardX + CARD_WIDTH - 70;
        int btnY = cardY + CARD_HEIGHT - 40;
        if (x >= btnX && x <= btnX + 55 && y >= btnY && y <= btnY + 28) {
            // 跳转到详情页
            currentActivityId = activities[i].id;
            // 检查当前用户是否已报名此活动（模拟，实际应从数据库读取）
            isEnrolled = false;
            currentPage = PAGE_DETAIL;
            return;
        }
    }
}

// ==================== 学生主界面入口 ====================
void runStudentMainPage() {
    ExMessage msg;
    bool running = true;

    // 初始化
    mouseX = 0;
    mouseY = 0;
    currentPage = PAGE_HOME;
    currentActivityId = 0;
    isEnrolled = false;

    while (running) {
        // 处理消息
        while (peekmessage(&msg)) {
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                if (currentPage == PAGE_HOME) {
                    handleHomeMouseClick(msg.x, msg.y);
                }
                else if (currentPage == PAGE_DETAIL) {
                    handleDetailMouseClick(msg.x, msg.y);
                }
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    if (currentPage == PAGE_DETAIL) {
                        currentPage = PAGE_HOME;  // ESC返回首页
                    }
                    else {
                        running = false;  // 在首页按ESC返回登录页
                    }
                }
            }
        }


        // 根据当前页面绘制不同界面
        if (currentPage == PAGE_HOME) {
            drawStudentHomePage();
        }
        else if (currentPage == PAGE_DETAIL) {
            drawActivityDetailPage();
        }

        FlushBatchDraw();
        Sleep(16);
    }
}