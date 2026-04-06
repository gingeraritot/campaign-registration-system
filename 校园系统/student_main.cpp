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
#define CARD_WIDTH    330
#define CARD_HEIGHT   180

// 页面状态枚举
typedef enum {
    PAGE_HOME,
    PAGE_DETAIL,
    PAGE_MY_ACTIVITIES
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
    wchar_t time[50];
    wchar_t organizer[50];
    wchar_t requirement[200];
    wchar_t description[300];
    wchar_t items[200];
    double lat;
    double lng;
} ActivityCard;

// 报名记录结构体
typedef struct EnrollmentRecord {
    int activityId;
    wchar_t enrollTime[50];
    struct EnrollmentRecord* next;
} EnrollmentRecord;

// 全局活动卡片数据（模拟数据）
ActivityCard activities[] = {
    {
        1, L"运动会", L"操场", L"体育", L"报名人数：", 25, 50,
        L"2025年10月20日 9:00-17:00", L"体育部", L"身体健康，热爱运动",
        L"一年一度的校园运动会，包含田径、球类等多个项目，欢迎同学们踊跃报名！",
        L"运动鞋、运动服、饮用水",
        39.9042, 116.4074
    },
    {
        2, L"班级活动", L"教室A101", L"班级", L"报名人数：", 8, 30,
        L"2025年10月25日 14:00-16:00", L"班长", L"本班同学",
        L"班级团建活动，增进同学感情，有游戏和零食。",
        L"无需特殊物品",
        39.9142, 116.4174
    },
    {
        3, L"社团活动", L"活动中心", L"社团", L"报名人数：", 15, 40,
        L"2025年11月1日 15:00-18:00", L"社团联合会", L"对社团活动感兴趣",
        L"各社团联合举办的大型活动，有表演、互动游戏等精彩内容。",
        L"学生证",
        39.8942, 116.3974
    },
    {
        4, L"志愿者活动", L"社区中心", L"公益", L"报名人数：", 12, 25,
        L"2025年11月10日 8:00-12:00", L"志愿者协会", L"有爱心，乐于助人",
        L"社区志愿服务，帮助老人和儿童，获得志愿时长认证。",
        L"志愿者马甲（现场发放）",
        39.8842, 116.3874
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
static int currentActivityId = 0;

// 当前登录的学生ID（模拟，实际应该从登录传递过来）
static int currentStudentId = 1001;

// 模拟当前用户报名的活动列表
static EnrollmentRecord* myEnrollments = NULL;

// 显示地图提示框
void showMapDialog(double lat, double lng, const wchar_t* locationName) {
    wchar_t msg[500];
    swprintf(msg, 500, L"【地图功能】\n\n活动地点：%s\n\n坐标信息：纬度 %.4f，经度 %.4f\n\n提示：实际应用中这里会调用地图API显示具体位置",
        locationName, lat, lng);
    MessageBoxW(GetHWnd(), msg, L"活动地图", MB_OK);
}

// ==================== 提交建议弹窗 ====================
void showSuggestionDialog() {
    // 创建输入框获取建议内容
    wchar_t input[500] = {0};
    InputBox(input, 500, L"请输入您的建议：", L"提交建议", NULL, 0, 0, false);
    
    if (wcslen(input) > 0) {
        // 将宽字符转换为多字节字符
        char contentA[500];
        WideCharToMultiByte(CP_ACP, 0, input, -1, contentA, 500, NULL, NULL);
        
        // 调用 head.h 中声明的 add_suggestion 函数
        int result = add_suggestion(currentStudentId, contentA);
        
        if (result == 1) {
            MessageBoxW(GetHWnd(), L"建议提交成功！感谢您的反馈。", L"提示", MB_OK);
        } else {
            MessageBoxW(GetHWnd(), L"建议提交失败，请稍后重试。", L"错误", MB_OK);
        }
    }
}

// 初始化模拟报名数据
void initMockEnrollments() {
    EnrollmentRecord* p = myEnrollments;
    while (p) {
        EnrollmentRecord* tmp = p;
        p = p->next;
        free(tmp);
    }
    myEnrollments = NULL;

    int activityIds[] = { 1, 2, 4 };
    const wchar_t* enrollTimes[] = { L"2025-10-15 10:30", L"2025-10-20 14:20", L"2025-11-05 09:15" };

    for (int i = 0; i < 3; i++) {
        EnrollmentRecord* newRecord = (EnrollmentRecord*)malloc(sizeof(EnrollmentRecord));
        newRecord->activityId = activityIds[i];
        wcscpy(newRecord->enrollTime, enrollTimes[i]);
        newRecord->next = NULL;

        if (myEnrollments == NULL) {
            myEnrollments = newRecord;
        }
        else {
            EnrollmentRecord* tail = myEnrollments;
            while (tail->next) tail = tail->next;
            tail->next = newRecord;
        }
    }
}

// 检查是否已报名某个活动
bool isActivityEnrolled(int activityId) {
    EnrollmentRecord* p = myEnrollments;
    while (p) {
        if (p->activityId == activityId) {
            return true;
        }
        p = p->next;
    }
    return false;
}

// 取消报名
void cancelEnrollment(int activityId) {
    EnrollmentRecord* prev = NULL;
    EnrollmentRecord* p = myEnrollments;
    while (p) {
        if (p->activityId == activityId) {
            if (prev) {
                prev->next = p->next;
            }
            else {
                myEnrollments = p->next;
            }
            free(p);

            for (int i = 0; i < activityCount; i++) {
                if (activities[i].id == activityId && activities[i].current > 0) {
                    activities[i].current--;
                    break;
                }
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

// 报名活动
void addEnrollment(int activityId) {
    if (isActivityEnrolled(activityId)) return;

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    wchar_t timeStr[50];
    swprintf(timeStr, 50, L"%04d-%02d-%02d %02d:%02d",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min);

    EnrollmentRecord* newRecord = (EnrollmentRecord*)malloc(sizeof(EnrollmentRecord));
    newRecord->activityId = activityId;
    wcscpy(newRecord->enrollTime, timeStr);
    newRecord->next = NULL;

    if (myEnrollments == NULL) {
        myEnrollments = newRecord;
    }
    else if (wcscmp(timeStr, myEnrollments->enrollTime) < 0) {
        newRecord->next = myEnrollments;
        myEnrollments = newRecord;
    }
    else {
        EnrollmentRecord* p = myEnrollments;
        while (p->next && wcscmp(timeStr, p->next->enrollTime) > 0) {
            p = p->next;
        }
        newRecord->next = p->next;
        p->next = newRecord;
    }

    for (int i = 0; i < activityCount; i++) {
        if (activities[i].id == activityId && activities[i].current < activities[i].max) {
            activities[i].current++;
            break;
        }
    }
}

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
void drawRoundRect(int x, int y, int w, int h, int radius, COLORREF lineColor, COLORREF fillColor) {
    setfillcolor(fillColor);
    setlinecolor(lineColor);
    fillroundrect(x, y, x + w, y + h, radius, radius);
}

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

void drawInfoRow(int x, int y, const wchar_t* label, const wchar_t* value, COLORREF labelColor, COLORREF valueColor) {
    settextcolor(labelColor);
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(x, y, label);
    settextcolor(valueColor);
    outtextxy(x + 100, y, value);
}

// 画活动卡片
void drawActivityCard(int x, int y, ActivityCard* card, bool isHover) {
    COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
    drawRoundRect(x, y, CARD_WIDTH, CARD_HEIGHT, 10, RGB(200, 200, 200), bgColor);

    settextcolor(RGB(30, 60, 120));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + 15, y + 15, card->title);

    settextcolor(RGB(100, 100, 100));
    settextstyle(14, 0, L"微软雅黑");
    wchar_t locationText[100];
    swprintf(locationText, 100, L"地点：%s", card->location);
    outtextxy(x + 15, y + 55, locationText);

    drawRoundRect(x + 15, y + 85, 60, 25, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(12, 0, L"微软雅黑");
    outtextxy(x + 22, y + 90, card->tag);

    wchar_t countText[100];
    swprintf(countText, 100, L"%s %d/%d", card->count, card->current, card->max);
    settextcolor(RGB(80, 80, 80));
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(x + 15, y + 125, countText);

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
    setfillcolor(RGB(70, 150, 230));
    setlinecolor(RGB(70, 150, 230));
    fillcircle(x + 15, y + 15, 12);

    settextcolor(WHITE);
    settextstyle(14, 0, L"微软雅黑");
    wchar_t num[5];
    swprintf(num, 5, L"%d", index + 1);
    outtextxy(x + 15 - textwidth(num) / 2, y + 15 - textheight(num) / 2, num);

    settextcolor(RGB(50, 50, 50));
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(x + 40, y + 8, name);

    setlinecolor(RGB(230, 230, 230));
    line(x, y + 32, x + 250, y + 32);
}

// ==================== 我的活动页面绘制 ====================
void drawMyActivitiesPage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    bool backHover = (mouseX >= 40 && mouseX <= 120 && mouseY >= 70 && mouseY <= 105);
    drawBtn(40, 70, 80, 35, L"← 返回", backHover, RGB(100, 100, 100));

    settextcolor(RGB(30, 60, 120));
    settextstyle(36, 0, L"微软雅黑");
    const wchar_t* title = L"我的活动";
    outtextxy((WIN_WIDTH - textwidth(title)) / 2, 80, title);

    int listStartY = 150;
    int listEndY = WIN_HEIGHT - 50;
    int itemHeight = 140;
    int itemGap = 15;
    int itemWidth = 1000;
    int itemX = (WIN_WIDTH - itemWidth) / 2;

    int enrollCount = 0;
    EnrollmentRecord* p = myEnrollments;
    while (p) { enrollCount++; p = p->next; }

    if (enrollCount == 0) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(24, 0, L"微软雅黑");
        const wchar_t* emptyMsg = L"暂无报名活动\n\n点击首页活动卡片上的\"详情\"按钮报名吧！";
        outtextxy((WIN_WIDTH - textwidth(emptyMsg)) / 2, 350, emptyMsg);
    }
    else {
        int y = listStartY;
        p = myEnrollments;

        while (p && y + itemHeight < listEndY) {
            ActivityCard* activity = NULL;
            for (int i = 0; i < activityCount; i++) {
                if (activities[i].id == p->activityId) {
                    activity = &activities[i];
                    break;
                }
            }

            if (activity != NULL) {
                bool isHover = (mouseX >= itemX && mouseX <= itemX + itemWidth &&
                    mouseY >= y && mouseY <= y + itemHeight);

                COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
                drawRoundRect(itemX, y, itemWidth, itemHeight, 10, RGB(200, 200, 200), bgColor);

                settextcolor(RGB(30, 60, 120));
                settextstyle(22, 0, L"微软雅黑");
                outtextxy(itemX + 20, y + 15, activity->title);

                settextcolor(RGB(100, 100, 100));
                settextstyle(16, 0, L"微软雅黑");
                wchar_t timeText[100];
                swprintf(timeText, 100, L"活动时间：%s", activity->time);
                outtextxy(itemX + 20, y + 50, timeText);

                wchar_t locationText[100];
                swprintf(locationText, 100, L"活动地点：%s", activity->location);
                outtextxy(itemX + 20, y + 78, locationText);

                settextcolor(RGB(80, 80, 80));
                wchar_t itemsText[200];
                swprintf(itemsText, 200, L"需携带：%s", activity->items);
                outtextxy(itemX + 20, y + 106, itemsText);

                bool mapHover = (mouseX >= itemX + itemWidth - 210 && mouseX <= itemX + itemWidth - 110 &&
                    mouseY >= y + 50 && mouseY <= y + 90);
                drawBtn(itemX + itemWidth - 210, y + 50, 90, 40, L"查看地图", mapHover, RGB(50, 150, 200));

                bool cancelHover = (mouseX >= itemX + itemWidth - 110 && mouseX <= itemX + itemWidth - 20 &&
                    mouseY >= y + 50 && mouseY <= y + 90);
                drawBtn(itemX + itemWidth - 110, y + 50, 90, 40, L"取消报名", cancelHover, RGB(200, 100, 100));
            }

            y += itemHeight + itemGap;
            p = p->next;
        }
    }
}

// ==================== 首页绘制 ====================
void drawStudentHomePage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    settextstyle(18, 0, L"微软雅黑");
    settextcolor(RGB(80, 80, 80));
    outtextxy(40, 70, L"欢迎，同学！");

    // 右上角退出登录按钮
    bool logoutHover = (mouseX >= WIN_WIDTH - 100 && mouseX <= WIN_WIDTH - 20 && mouseY >= 30 && mouseY <= 70);
    drawBtn(WIN_WIDTH - 100, 30, 80, 40, L"退出", logoutHover, RGB(200, 100, 100));

    settextstyle(20, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(40, 120, L"热门活动推荐");

    drawRoundRect(40, 155, 260, 320, 10, RGB(220, 220, 220), RGB(255, 255, 255));

    for (int i = 0; i < hotCount && i < 6; i++) {
        drawHotActivityItem(50, 165 + i * 48, hotActivities[i].name, i);
    }

    settextstyle(20, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(330, 120, L"推荐活动");

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

    int rightX = WIN_WIDTH - 210;
    int rightW = 190;

    drawRoundRect(rightX, 120, rightW, 420, 10, RGB(220, 220, 220), RGB(255, 255, 255));

    settextcolor(RGB(30, 60, 120));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy(rightX + (rightW - textwidth(L"个人中心")) / 2, 140, L"个人中心");

    setlinecolor(RGB(230, 230, 230));
    line(rightX + 10, 165, rightX + rightW - 10, 165);

    setfillcolor(RGB(70, 150, 230));
    setlinecolor(RGB(70, 150, 230));
    fillcircle(rightX + rightW / 2, 205, 30);
    settextcolor(WHITE);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(rightX + rightW / 2 - 10, 197, L"我");

    settextcolor(RGB(50, 50, 50));
    settextstyle(14, 0, L"微软雅黑");
    outtextxy(rightX + (rightW - textwidth(L"学生用户")) / 2, 250, L"学生用户");

    // 我的活动按钮
    bool myActHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 280 && mouseY <= 315);
    drawBtn(rightX + 20, 280, rightW - 40, 35, L"我的活动", myActHover, RGB(70, 150, 230));

    // 我的勋章按钮（保留，功能开发中）
    bool medalHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 325 && mouseY <= 360);
    drawBtn(rightX + 20, 325, rightW - 40, 35, L"我的勋章", medalHover, RGB(100, 180, 100));

    // 提交建议按钮
    bool suggestionHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 370 && mouseY <= 405);
    drawBtn(rightX + 20, 370, rightW - 40, 35, L"提交建议", suggestionHover, RGB(150, 150, 50));

    // 个人设置按钮
    bool settingHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 415 && mouseY <= 450);
    drawBtn(rightX + 20, 415, rightW - 40, 35, L"个人设置", settingHover, RGB(150, 150, 150));

    // 查看更多活动按钮
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

    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    bool backHover = (mouseX >= 40 && mouseX <= 120 && mouseY >= 70 && mouseY <= 105);
    drawBtn(40, 70, 80, 35, L"← 返回", backHover, RGB(100, 100, 100));

    settextcolor(RGB(30, 60, 120));
    settextstyle(36, 0, L"微软雅黑");
    wchar_t titleText[100];
    swprintf(titleText, 100, L"%s", activity->title);
    outtextxy((WIN_WIDTH - textwidth(titleText)) / 2, 80, titleText);

    drawRoundRect((WIN_WIDTH - 80) / 2, 130, 80, 30, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy((WIN_WIDTH - textwidth(activity->tag)) / 2, 137, activity->tag);

    int infoCardX = 100;
    int infoCardY = 180;
    int infoCardW = 1000;
    int infoCardH = 530;

    drawRoundRect(infoCardX, infoCardY, infoCardW, infoCardH, 10, RGB(200, 200, 200), RGB(255, 255, 255));

    int lineY = infoCardY + 50;
    int leftX = infoCardX + 50;

    wchar_t timeLabel[50] = L"活动时间：";
    drawInfoRow(leftX, lineY, timeLabel, activity->time, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    wchar_t locationLabel[50] = L"活动地点：";
    drawInfoRow(leftX, lineY, locationLabel, activity->location, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    wchar_t organizerLabel[50] = L"组织者：";
    drawInfoRow(leftX, lineY, organizerLabel, activity->organizer, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    wchar_t requirementLabel[50] = L"要求：";
    drawInfoRow(leftX, lineY, requirementLabel, activity->requirement, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 55;

    wchar_t itemsLabel[50] = L"需携带：";
    drawInfoRow(leftX, lineY, itemsLabel, activity->items, RGB(80, 80, 80), RGB(50, 50, 50));
    lineY += 45;

    wchar_t enrollLabel[50] = L"报名情况：";
    wchar_t enrollValue[100];
    swprintf(enrollValue, 100, L"%d / %d 人", activity->current, activity->max);
    drawInfoRow(leftX, lineY, enrollLabel, enrollValue, RGB(80, 80, 80), RGB(50, 150, 50));
    lineY += 45;

    settextcolor(RGB(30, 60, 120));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(leftX, lineY, L"活动描述：");
    lineY += 35;

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

    int btnY = infoCardY + infoCardH - 60;
    int btnX = (WIN_WIDTH - 200) / 2;
    bool alreadyEnrolled = isActivityEnrolled(activity->id);

    settextstyle(18, 0, L"微软雅黑");
    if (alreadyEnrolled) {
        settextcolor(RGB(50, 150, 50));
        outtextxy(btnX - 120, btnY + 12, L"✓ 您已报名此活动");
    }
    else {
        if (activity->current >= activity->max) {
            settextcolor(RGB(200, 50, 50));
            outtextxy(btnX - 120, btnY + 12, L"✗ 活动已满，不可报名");
        }
        else {
            settextcolor(RGB(150, 150, 150));
            outtextxy(btnX - 120, btnY + 12, L"● 未报名");
        }
    }

    bool actionHover = (mouseX >= btnX && mouseX <= btnX + 200 && mouseY >= btnY && mouseY <= btnY + 45);

    if (alreadyEnrolled) {
        drawBtn(btnX, btnY, 200, 45, L"取消报名", actionHover, RGB(200, 100, 100));
    }
    else {
        if (activity->current >= activity->max) {
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

// ==================== 我的活动页鼠标点击处理 ====================
void handleMyActivitiesMouseClick(int x, int y) {
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }

    int listStartY = 150;
    int itemHeight = 140;
    int itemGap = 15;
    int itemWidth = 1000;
    int itemX = (WIN_WIDTH - itemWidth) / 2;

    int yPos = listStartY;
    EnrollmentRecord* p = myEnrollments;

    while (p) {
        ActivityCard* activity = NULL;
        for (int i = 0; i < activityCount; i++) {
            if (activities[i].id == p->activityId) {
                activity = &activities[i];
                break;
            }
        }

        if (activity != NULL) {
            if (x >= itemX + itemWidth - 210 && x <= itemX + itemWidth - 110 &&
                y >= yPos + 50 && y <= yPos + 90) {
                showMapDialog(activity->lat, activity->lng, activity->location);
                return;
            }

            if (x >= itemX + itemWidth - 110 && x <= itemX + itemWidth - 20 &&
                y >= yPos + 50 && y <= yPos + 90) {
                wchar_t msg[200];
                swprintf(msg, 200, L"确定要取消报名【%s】吗？", activity->title);
                if (MessageBoxW(GetHWnd(), msg, L"确认取消", MB_YESNO) == IDYES) {
                    cancelEnrollment(p->activityId);
                }
                return;
            }
        }

        yPos += itemHeight + itemGap;
        p = p->next;
    }
}

// ==================== 详情页鼠标点击处理 ====================
void handleDetailMouseClick(int x, int y) {
    ActivityCard* activity = getCurrentActivity();

    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }

    int btnX = (WIN_WIDTH - 200) / 2;
    int infoCardY = 180;
    int infoCardH = 530;
    int btnY = infoCardY + infoCardH - 60;

    if (x >= btnX && x <= btnX + 200 && y >= btnY && y <= btnY + 45) {
        if (activity == NULL) return;

        if (isActivityEnrolled(activity->id)) {
            wchar_t msg[200];
            swprintf(msg, 200, L"确定要取消报名【%s】吗？", activity->title);
            if (MessageBoxW(GetHWnd(), msg, L"确认取消", MB_YESNO) == IDYES) {
                cancelEnrollment(activity->id);
            }
        }
        else {
            if (activity->current < activity->max) {
                addEnrollment(activity->id);
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
    // 退出登录按钮
    if (x >= WIN_WIDTH - 100 && x <= WIN_WIDTH - 20 && y >= 30 && y <= 70) {
        // 退出登录，返回登录页
        return;  // 返回后会退出 runStudentMainPage 函数
    }
    
    // 查看更多活动按钮
    if (x >= WIN_WIDTH / 2 - 100 && x <= WIN_WIDTH / 2 + 100 &&
        y >= WIN_HEIGHT - 60 && y <= WIN_HEIGHT - 20) {
        MessageBoxW(GetHWnd(), L"更多活动功能开发中...", L"提示", MB_OK);
        return;
    }

    int rightX = WIN_WIDTH - 210;
    int rightW = 190;

    // 我的活动按钮
    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 280 && y <= 315) {
        currentPage = PAGE_MY_ACTIVITIES;
        return;
    }
    
    // 我的勋章按钮（开发中）
    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 325 && y <= 360) {
        MessageBoxW(GetHWnd(), L"我的勋章功能开发中...", L"提示", MB_OK);
        return;
    }
    
    // 提交建议按钮
    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 370 && y <= 405) {
        showSuggestionDialog();
        return;
    }

    // 个人设置按钮
    if (x >= rightX + 20 && x <= rightX + rightW - 20 && y >= 415 && y <= 450) {
        MessageBoxW(GetHWnd(), L"个人设置功能开发中...", L"提示", MB_OK);
        return;
    }

    // 活动卡片详情按钮
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
            currentActivityId = activities[i].id;
            currentPage = PAGE_DETAIL;
            return;
        }
    }
}

// ==================== 学生主界面入口 ====================
void runStudentMainPage() {
    ExMessage msg;
    bool running = true;

    mouseX = 0;
    mouseY = 0;
    currentPage = PAGE_HOME;
    currentActivityId = 0;

    initMockEnrollments();

    BeginBatchDraw();

    while (running) {
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
                else if (currentPage == PAGE_MY_ACTIVITIES) {
                    handleMyActivitiesMouseClick(msg.x, msg.y);
                }
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    if (currentPage == PAGE_DETAIL || currentPage == PAGE_MY_ACTIVITIES) {
                        currentPage = PAGE_HOME;
                    }
                    else {
                        running = false;
                    }
                }
            }
        }

        if (currentPage == PAGE_HOME) {
            drawStudentHomePage();
        }
        else if (currentPage == PAGE_DETAIL) {
            drawActivityDetailPage();
        }
        else if (currentPage == PAGE_MY_ACTIVITIES) {
            drawMyActivitiesPage();
        }

        FlushBatchDraw();
        Sleep(16);
    }

    EndBatchDraw();
}