#include <graphics.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "head.h"
#include <algorithm>
#include <vector>
#include <string>
#include <shellapi.h>

using namespace std;

static int g_currentStudentId = 0;   // 当前登录的学生ID
static bool g_studentShouldExit = false;
static int currentActivityPage = 0; // 当前活动列表页码（0开始）
static const int ACTIVITIES_PER_PAGE = 8; // 每页显示8个活动

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
    PAGE_MY_ACTIVITIES,
    PAGE_HISTORY,
    PAGE_SEARCH_RESULT,
    PAGE_ALL_ACTIVITIES
} StudentPageState;

// 活动卡片数据结构（用于显示）
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

// 全局变量
static int mouseX = 0, mouseY = 0;
static StudentPageState currentPage = PAGE_HOME;
static int currentActivityId = 0;

// 搜索相关变量
static wchar_t searchKeyword[100] = { 0 };
static bool isSearchFocus = false;
static ActivityCard* searchResults[100];
static int searchResultCount = 0;



// ==================== 辅助函数 ====================
static wstring StringToWstring(const string& s) {
    int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);
    wstring ws(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, &ws[0], len);
    return ws;
}

// 将底层 Activity 转换为显示用的 ActivityCard
static void FillActivityCard(Activity* act, ActivityCard* card) {
    card->id = act->id;
    wstring wName = StringToWstring(act->name);
    wcscpy(card->title, wName.c_str());
    wstring wLoc = StringToWstring(act->location);
    wcscpy(card->location, wLoc.c_str());

    Category* cat = find_category_by_id(act->category_id);
    if (cat) {
        wstring wTag = StringToWstring(cat->name);
        wcscpy(card->tag, wTag.c_str());
    }
    else {
        wcscpy(card->tag, L"未分类");
    }

    wcscpy(card->count, L"报名人数：");
    card->current = act->current_count;
    card->max = act->max_participants;

    wstring wTime = StringToWstring(act->start_time);
    wcscpy(card->time, wTime.c_str());

    wstring wOrganizer = StringToWstring(act->reward);
    wcscpy(card->organizer, wOrganizer.c_str());

    wcscpy(card->requirement, L"无");

    wstring wDesc = StringToWstring(act->description);
    wcscpy(card->description, wDesc.c_str());

    wstring wItems = StringToWstring(act->items_to_bring);
    wcscpy(card->items, wItems.c_str());

    // 模拟坐标（实际可从活动地点解析，这里简化）
    card->lat = 39.9042;
    card->lng = 116.4074;
}

// 获取热门活动（按报名人数排序，取前4个）
static void GetHotActivities(ActivityCard* outCards, int* outCount) {
    vector<Activity*> publishedActs;
    Activity* act = activity_head;
    while (act) {
        if (act->status == PUBLISHED) {
            publishedActs.push_back(act);
        }
        act = act->next;
    }
    sort(publishedActs.begin(), publishedActs.end(), [](Activity* a, Activity* b) {
        return a->current_count > b->current_count;
        });
    int limit = min(4, (int)publishedActs.size());
    *outCount = limit;
    for (int i = 0; i < limit; i++) {
        FillActivityCard(publishedActs[i], &outCards[i]);
    }
}

// 获取当前活动（用于详情页）
static Activity* getCurrentActivityPtr() {
    return find_activity_by_id(currentActivityId);
}

// 称号系统（基于真实报名记录）
static int getRealEnrollmentCount() {
    int cnt = 0;
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == g_currentStudentId) cnt++;
        e = e->next;
    }
    return cnt;
}

static int getRealTotalDuration() {
    // 简化：假设每个活动2小时
    return getRealEnrollmentCount() * 2;
}

static void getRealTitle(wchar_t* title, int titleSize) {
    int count = getRealEnrollmentCount();
    int duration = getRealTotalDuration();

    if (count >= 4 && duration >= 8) wcscpy(title, L"⭐ 活动大师 ⭐");
    else if (count >= 3 && duration >= 6) wcscpy(title, L"🏆 资深参与者 🏆");
    else if (count >= 2 && duration >= 4) wcscpy(title, L"🌟 活跃成员 🌟");
    else if (count >= 1 && duration >= 2) wcscpy(title, L"🌱 新手上路 🌱");
    else wcscpy(title, L"📖 萌新小白 📖");
}

static COLORREF getRealTitleColor() {
    int count = getRealEnrollmentCount();
    if (count >= 4) return RGB(255, 200, 0);
    if (count >= 3) return RGB(255, 150, 50);
    if (count >= 2) return RGB(100, 180, 100);
    if (count >= 1) return RGB(100, 150, 200);
    return RGB(150, 150, 150);
}

// 报名相关函数（直接调用底层接口）
static bool isActivityEnrolled(int activityId) {
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == g_currentStudentId && e->activity_id == activityId) return true;
        e = e->next;
    }
    return false;
}

static void addEnrollment(int activityId) {
    int result = enroll_student(g_currentStudentId, activityId);
    if (result == 0) MessageBoxW(GetHWnd(), L"报名成功！", L"提示", MB_OK);
    else if (result == -4) MessageBoxW(GetHWnd(), L"报名失败，活动已满！", L"提示", MB_OK);
    else if (result == -5) MessageBoxW(GetHWnd(), L"您已经报名过该活动！", L"提示", MB_OK);
    else MessageBoxW(GetHWnd(), L"报名失败！", L"错误", MB_OK);
}

static void cancelEnrollment(int activityId) {
    int result = cancel_enrollment(g_currentStudentId, activityId);
    if (result == 0) MessageBoxW(GetHWnd(), L"取消报名成功！", L"提示", MB_OK);
    else MessageBoxW(GetHWnd(), L"取消失败！", L"错误", MB_OK);
}

// ==================== 绘图工具函数 ====================
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

void drawSearchBox(int x, int y, int w, int h) {
    drawRoundRect(x, y, w, h, 8, isSearchFocus ? RGB(0, 120, 255) : RGB(180, 180, 180), WHITE);
    settextcolor(RGB(150, 150, 150));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + 10, y + 8, L"🔍");
    settextcolor(BLACK);
    settextstyle(18, 0, L"微软雅黑");
    if (wcslen(searchKeyword) > 0) outtextxy(x + 40, y + 8, searchKeyword);
    else if (!isSearchFocus) {
        settextcolor(RGB(180, 180, 180));
        outtextxy(x + 40, y + 8, L"搜索活动名称或标签...");
    }
    if (isSearchFocus) {
        int cursorX = x + 40 + textwidth(searchKeyword);
        setlinecolor(BLACK);
        line(cursorX, y + 10, cursorX, y + h - 10);
    }
}

void drawInfoRow(int x, int y, const wchar_t* label, const wchar_t* value, COLORREF labelColor, COLORREF valueColor) {
    settextcolor(labelColor);
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(x, y, label);
    settextcolor(valueColor);
    outtextxy(x + 100, y, value);
}

void drawActivityCard(int x, int y, ActivityCard* card, bool isHover) {
    COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
    drawRoundRect(x, y, CARD_WIDTH, CARD_HEIGHT, 10, RGB(200, 200, 200), bgColor);
    settextcolor(RGB(30, 60, 120));
    settextstyle(26, 0, L"微软雅黑");
    outtextxy(x + 15, y + 15, card->title);
    settextcolor(RGB(100, 100, 100));
    settextstyle(20, 0, L"微软雅黑");
    wchar_t locationText[100];
    swprintf(locationText, 100, L"地点：%s", card->location);
    outtextxy(x + 15, y + 50, locationText);
    drawRoundRect(x + 15, y + 85, 60, 25, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy(x + 22, y + 90, card->tag);
    wchar_t countText[100];
    swprintf(countText, 100, L"%s %d/%d", card->count, card->current, card->max);
    settextcolor(RGB(80, 80, 80));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + 15, y + 125, countText);
    bool btnHover = (mouseX >= x + CARD_WIDTH - 70 && mouseX <= x + CARD_WIDTH - 15 &&
        mouseY >= y + CARD_HEIGHT - 40 && mouseY <= y + CARD_HEIGHT - 15);
    COLORREF btnColor = btnHover ? RGB(100, 180, 255) : RGB(70, 150, 230);
    drawRoundRect(x + CARD_WIDTH - 70, y + CARD_HEIGHT - 40, 55, 28, 5, RGB(50, 120, 200), btnColor);
    settextcolor(WHITE);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + CARD_WIDTH - 65, y + CARD_HEIGHT - 36, L"详情");
}

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
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + 40, y + 8, name);
    setlinecolor(RGB(230, 230, 230));
    line(x - 20, y + 32, x + 230, y + 32);
}

// 判断活动是否已结束
bool isActivityEnded(Activity* act) {
    int year, month, day;
    if (sscanf(act->start_time, "%d-%d-%d", &year, &month, &day) == 3) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        int curYear = t->tm_year + 1900, curMonth = t->tm_mon + 1, curDay = t->tm_mday;
        if (year < curYear) return true;
        if (year > curYear) return false;
        if (month < curMonth) return true;
        if (month > curMonth) return false;
        if (day < curDay) return true;
    }
    return false;
}

// ==================== 各页面绘制 ====================
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

    bool logoutHover = (mouseX >= WIN_WIDTH - 100 && mouseX <= WIN_WIDTH - 20 && mouseY >= 30 && mouseY <= 70);
    drawBtn(WIN_WIDTH - 100, 30, 80, 40, L"退出", logoutHover, RGB(200, 100, 100));

    int searchX = WIN_WIDTH - 380, searchY = 32, searchW = 260, searchH = 36;
    drawSearchBox(searchX, searchY, searchW, searchH);

    settextstyle(24, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(20, 120, L"热门活动推荐");
    drawRoundRect(20, 155, 260, 320, 10, RGB(220, 220, 220), RGB(255, 255, 255));
    ActivityCard hotCards[4];
    int hotCount = 0;
    GetHotActivities(hotCards, &hotCount);
    for (int i = 0; i < hotCount; i++) {
        drawHotActivityItem(50, 165 + i * 48, hotCards[i].title, i);
    }
    if (hotCount == 0) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(14, 0, L"微软雅黑");
        outtextxy(50, 200, L"暂无已发布活动");
    }

    settextstyle(24, 0, L"微软雅黑");
    settextcolor(RGB(30, 60, 120));
    outtextxy(330, 120, L"推荐活动");
    int cardStartX = 300, cardStartY = 160, cardGap = 25;
    vector<Activity*> publishedActs;
    Activity* act = activity_head;
    while (act) {
        if (act->status == PUBLISHED) publishedActs.push_back(act);
        act = act->next;
    }
    sort(publishedActs.begin(), publishedActs.end(), [](Activity* a, Activity* b) { return a->id > b->id; });
    int displayCount = min(4, (int)publishedActs.size());
    for (int i = 0; i < displayCount; i++) {
        int col = i % 2, row = i / 2;
        int x = cardStartX + col * (CARD_WIDTH + cardGap);
        int y = cardStartY + row * (CARD_HEIGHT + cardGap);
        ActivityCard card;
        FillActivityCard(publishedActs[i], &card);
        bool isHover = (mouseX >= x && mouseX <= x + CARD_WIDTH && mouseY >= y && mouseY <= y + CARD_HEIGHT);
        drawActivityCard(x, y, &card, isHover);
    }
    if (displayCount == 0) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(18, 0, L"微软雅黑");
        outtextxy(330, 200, L"暂无已发布活动");
    }

    int rightX = WIN_WIDTH - 200, rightW = 190;
    drawRoundRect(rightX, 120, rightW, 420, 10, RGB(220, 220, 220), RGB(255, 255, 255));
    settextcolor(RGB(30, 60, 120));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(rightX + (rightW - textwidth(L"个人中心")) / 2, 140, L"个人中心");
    line(rightX + 10, 165, rightX + rightW - 10, 165);
    setfillcolor(RGB(70, 150, 230));
    setlinecolor(RGB(70, 150, 230));
    fillcircle(rightX + rightW / 2, 205, 30);
    settextcolor(WHITE);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(rightX + rightW / 2 - 10, 197, L"我");
    wchar_t titleText[50];
    getRealTitle(titleText, 50);
    settextcolor(getRealTitleColor());
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(rightX + (rightW - textwidth(titleText)) / 2, 250, titleText);
    settextcolor(RGB(120, 120, 120));
    settextstyle(16, 0, L"微软雅黑");
    wchar_t infoText[100];
    int cnt = getRealEnrollmentCount();
    int dur = getRealTotalDuration();
    swprintf(infoText, 100, L"报名%d项 | 累计%d小时", cnt, dur);
    outtextxy(rightX + (rightW - textwidth(infoText)) / 2, 275, infoText);

    bool titleHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 305 && mouseY <= 340);
    drawBtn(rightX + 20, 305, rightW - 40, 35, L"称号详情", titleHover, RGB(100, 180, 100));
    bool myActHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 350 && mouseY <= 385);
    drawBtn(rightX + 20, 350, rightW - 40, 35, L"我的活动", myActHover, RGB(70, 150, 230));
    bool suggestionHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 395 && mouseY <= 430);
    drawBtn(rightX + 20, 395, rightW - 40, 35, L"提交建议", suggestionHover, RGB(150, 150, 50));
    bool historyHover = (mouseX >= rightX + 20 && mouseX <= rightX + rightW - 20 && mouseY >= 440 && mouseY <= 475);
    drawBtn(rightX + 20, 440, rightW - 40, 35, L"历史记录", historyHover, RGB(100, 100, 200));
    bool moreHover = (mouseX >= WIN_WIDTH / 2 - 100 && mouseX <= WIN_WIDTH / 2 + 100 &&
        mouseY >= WIN_HEIGHT - 60 && mouseY <= WIN_HEIGHT - 20);
    drawBtn(WIN_WIDTH / 2 - 100, WIN_HEIGHT - 60, 200, 40, L"查看更多活动", moreHover, RGB(70, 150, 230));
}

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

    int listStartY = 150, itemHeight = 140, itemGap = 15, itemWidth = 1000;
    int itemX = (WIN_WIDTH - itemWidth) / 2;
    int y = listStartY;
    Enrollment* e = enrollment_head;
    bool hasAny = false;
    while (e && y + itemHeight < WIN_HEIGHT - 50) {
        if (e->student_id == g_currentStudentId) {
            Activity* act = find_activity_by_id(e->activity_id);
            if (act && act->status == PUBLISHED) {
                hasAny = true;
                ActivityCard card;
                FillActivityCard(act, &card);
                bool isHover = (mouseX >= itemX && mouseX <= itemX + itemWidth && mouseY >= y && mouseY <= y + itemHeight);
                COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
                drawRoundRect(itemX, y, itemWidth, itemHeight, 10, RGB(200, 200, 200), bgColor);
                settextcolor(RGB(30, 60, 120));
                settextstyle(22, 0, L"微软雅黑");
                outtextxy(itemX + 20, y + 15, card.title);
                settextcolor(RGB(100, 100, 100));
                settextstyle(16, 0, L"微软雅黑");
                wchar_t timeText[100];
                swprintf(timeText, 100, L"活动时间：%s", card.time);
                outtextxy(itemX + 20, y + 50, timeText);
                wchar_t locationText[100];
                swprintf(locationText, 100, L"活动地点：%s", card.location);
                outtextxy(itemX + 20, y + 78, locationText);
                settextcolor(RGB(80, 80, 80));
                wchar_t itemsText[200];
                swprintf(itemsText, 200, L"需携带：%s", card.items);
                outtextxy(itemX + 20, y + 106, itemsText);
                bool mapHover = (mouseX >= itemX + itemWidth - 210 && mouseX <= itemX + itemWidth - 110 &&
                    mouseY >= y + 50 && mouseY <= y + 90);
                drawBtn(itemX + itemWidth - 210, y + 50, 90, 40, L"查看地图", mapHover, RGB(50, 150, 200));
                bool cancelHover = (mouseX >= itemX + itemWidth - 110 && mouseX <= itemX + itemWidth - 20 &&
                    mouseY >= y + 50 && mouseY <= y + 90);
                drawBtn(itemX + itemWidth - 110, y + 50, 90, 40, L"取消报名", cancelHover, RGB(200, 100, 100));
                y += itemHeight + itemGap;
            }
        }
        e = e->next;
    }
    if (!hasAny) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(24, 0, L"微软雅黑");
        const wchar_t* emptyMsg = L"暂无报名活动\n\n点击首页活动卡片上的\"详情\"按钮报名吧！";
        outtextxy((WIN_WIDTH - textwidth(emptyMsg)) / 2, 350, emptyMsg);
    }
}

void drawActivityDetailPage() {
    Activity* act = getCurrentActivityPtr();
    if (!act || act->status != PUBLISHED) {
        currentPage = PAGE_HOME;
        return;
    }
    ActivityCard card;
    FillActivityCard(act, &card);

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
    swprintf(titleText, 100, L"%s", card.title);
    outtextxy((WIN_WIDTH - textwidth(titleText)) / 2, 80, titleText);
    drawRoundRect((WIN_WIDTH - 80) / 2, 130, 80, 30, 5, RGB(150, 200, 255), RGB(200, 225, 255));
    settextcolor(RGB(50, 100, 150));
    settextstyle(16, 0, L"微软雅黑");
    outtextxy((WIN_WIDTH - textwidth(card.tag)) / 2, 137, card.tag);

    int infoCardX = 100, infoCardY = 180, infoCardW = 1000, infoCardH = 530;
    drawRoundRect(infoCardX, infoCardY, infoCardW, infoCardH, 10, RGB(200, 200, 200), RGB(255, 255, 255));
    int lineY = infoCardY + 50, leftX = infoCardX + 50;
    drawInfoRow(leftX, lineY, L"活动时间：", card.time, RGB(80, 80, 80), RGB(50, 50, 50)); lineY += 45;
    drawInfoRow(leftX, lineY, L"活动地点：", card.location, RGB(80, 80, 80), RGB(50, 50, 50)); lineY += 45;
    drawInfoRow(leftX, lineY, L"组织者：", card.organizer, RGB(80, 80, 80), RGB(50, 50, 50)); lineY += 45;
    drawInfoRow(leftX, lineY, L"要求：", card.requirement, RGB(80, 80, 80), RGB(50, 50, 50)); lineY += 55;
    drawInfoRow(leftX, lineY, L"需携带：", card.items, RGB(80, 80, 80), RGB(50, 50, 50)); lineY += 45;
    wchar_t enrollValue[100];
    swprintf(enrollValue, 100, L"%d / %d 人", card.current, card.max);
    drawInfoRow(leftX, lineY, L"报名情况：", enrollValue, RGB(80, 80, 80), RGB(50, 150, 50)); lineY += 45;
    settextcolor(RGB(30, 60, 120));
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(leftX, lineY, L"活动描述："); lineY += 35;
    settextcolor(RGB(80, 80, 80));
    settextstyle(16, 0, L"微软雅黑");
    wchar_t* desc = card.description;
    int descLen = wcslen(desc), maxCharsPerLine = 45;
    int lineCount = (descLen + maxCharsPerLine - 1) / maxCharsPerLine;
    for (int i = 0; i < lineCount && i < 5; i++) {
        wchar_t lineText[100] = { 0 };
        int start = i * maxCharsPerLine;
        int len = (start + maxCharsPerLine < descLen) ? maxCharsPerLine : descLen - start;
        wcsncpy(lineText, desc + start, len);
        outtextxy(leftX, lineY + i * 28, lineText);
    }

    int btnY = infoCardY + infoCardH - 60, btnX = (WIN_WIDTH - 200) / 2;
    bool alreadyEnrolled = isActivityEnrolled(act->id);
    settextstyle(18, 0, L"微软雅黑");
    if (alreadyEnrolled) {
        settextcolor(RGB(50, 150, 50));
        outtextxy(btnX - 120, btnY + 12, L"✓ 您已报名此活动");
    }
    else {
        if (card.current >= card.max) {
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
        if (card.current >= card.max) {
            drawRoundRect(btnX, btnY, 200, 45, 8, RGB(150, 150, 150), RGB(180, 180, 180));
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

void drawHistoryPage() {
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
    outtextxy((WIN_WIDTH - textwidth(L"历史记录")) / 2, 80, L"历史记录");
    settextstyle(18, 0, L"微软雅黑");
    settextcolor(RGB(100, 100, 100));
    outtextxy((WIN_WIDTH - textwidth(L"已结束的活动")) / 2, 130, L"已结束的活动");

    vector<Activity*> ended;
    Activity* act = activity_head;
    while (act) {
        if (act->status == PUBLISHED && isActivityEnded(act)) ended.push_back(act);
        act = act->next;
    }
    int listStartY = 180, itemHeight = 120, itemGap = 15, itemWidth = 900;
    int itemX = (WIN_WIDTH - itemWidth) / 2;
    if (ended.empty()) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(24, 0, L"微软雅黑");
        outtextxy((WIN_WIDTH - textwidth(L"暂无已结束的活动记录")) / 2, 350, L"暂无已结束的活动记录");
    }
    else {
        int y = listStartY;
        for (size_t i = 0; i < ended.size() && y + itemHeight < WIN_HEIGHT - 50; i++) {
            ActivityCard card;
            FillActivityCard(ended[i], &card);
            bool isHover = (mouseX >= itemX && mouseX <= itemX + itemWidth && mouseY >= y && mouseY <= y + itemHeight);
            COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
            drawRoundRect(itemX, y, itemWidth, itemHeight, 10, RGB(200, 200, 200), bgColor);
            settextcolor(RGB(30, 60, 120));
            settextstyle(20, 0, L"微软雅黑");
            outtextxy(itemX + 20, y + 15, card.title);
            drawRoundRect(itemX + 180, y + 12, 60, 25, 5, RGB(150, 200, 255), RGB(200, 225, 255));
            settextcolor(RGB(50, 100, 150));
            settextstyle(12, 0, L"微软雅黑");
            outtextxy(itemX + 190, y + 17, card.tag);
            settextcolor(RGB(100, 100, 100));
            settextstyle(16, 0, L"微软雅黑");
            wchar_t timeText[150];
            swprintf(timeText, 150, L"活动时间：%s", card.time);
            outtextxy(itemX + 20, y + 50, timeText);
            wchar_t locationText[150];
            swprintf(locationText, 150, L"活动地点：%s", card.location);
            outtextxy(itemX + 20, y + 80, locationText);
            setfillcolor(RGB(180, 180, 180));
            setlinecolor(RGB(150, 150, 150));
            drawRoundRect(itemX + itemWidth - 100, y + 35, 80, 35, 8, RGB(150, 150, 150), RGB(180, 180, 180));
            settextcolor(WHITE);
            settextstyle(16, 0, L"微软雅黑");
            outtextxy(itemX + itemWidth - 85, y + 45, L"已结束");
            y += itemHeight + itemGap;
        }
    }
}

void drawSearchResultPage() {
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
    wchar_t titleText[150];
    swprintf(titleText, 150, L"搜索结果：%s", searchKeyword);
    outtextxy((WIN_WIDTH - textwidth(titleText)) / 2, 80, titleText);
    settextstyle(18, 0, L"微软雅黑");
    settextcolor(RGB(100, 100, 100));
    wchar_t countText[100];
    swprintf(countText, 100, L"共找到 %d 个活动", searchResultCount);
    outtextxy((WIN_WIDTH - textwidth(countText)) / 2, 130, countText);

    int listStartY = 180, itemHeight = 120, itemGap = 15, itemWidth = 900;
    int itemX = (WIN_WIDTH - itemWidth) / 2;
    if (searchResultCount == 0) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(24, 0, L"微软雅黑");
        outtextxy((WIN_WIDTH - textwidth(L"未找到相关活动\n\n试试其他关键词吧！")) / 2, 350, L"未找到相关活动\n\n试试其他关键词吧！");
    }
    else {
        int y = listStartY;
        for (int i = 0; i < searchResultCount && y + itemHeight < WIN_HEIGHT - 50; i++) {
            ActivityCard* activity = searchResults[i];
            bool isHover = (mouseX >= itemX && mouseX <= itemX + itemWidth && mouseY >= y && mouseY <= y + itemHeight);
            COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
            drawRoundRect(itemX, y, itemWidth, itemHeight, 10, RGB(200, 200, 200), bgColor);
            settextcolor(RGB(30, 60, 120));
            settextstyle(20, 0, L"微软雅黑");
            outtextxy(itemX + 20, y + 15, activity->title);
            drawRoundRect(itemX + 180, y + 12, 60, 25, 5, RGB(150, 200, 255), RGB(200, 225, 255));
            settextcolor(RGB(50, 100, 150));
            settextstyle(12, 0, L"微软雅黑");
            outtextxy(itemX + 190, y + 17, activity->tag);
            settextcolor(RGB(100, 100, 100));
            settextstyle(16, 0, L"微软雅黑");
            wchar_t timeText[150];
            swprintf(timeText, 150, L"活动时间：%s", activity->time);
            outtextxy(itemX + 20, y + 50, timeText);
            wchar_t locationText[150];
            swprintf(locationText, 150, L"活动地点：%s", activity->location);
            outtextxy(itemX + 20, y + 80, locationText);
            bool detailHover = (mouseX >= itemX + itemWidth - 100 && mouseX <= itemX + itemWidth - 20 &&
                mouseY >= y + 40 && mouseY <= y + 75);
            drawBtn(itemX + itemWidth - 100, y + 40, 80, 35, L"查看详情", detailHover, RGB(70, 150, 230));
            y += itemHeight + itemGap;
        }
    }
}

// ==================== 新增：所有活动列表页面 ====================
void drawAllActivitiesPage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // 标题栏
    settextcolor(RGB(30, 60, 120));
    settextstyle(32, 0, L"微软雅黑");
    outtextxy(40, 25, L"校园活动管理系统");

    // 返回按钮
    bool backHover = (mouseX >= 40 && mouseX <= 120 && mouseY >= 70 && mouseY <= 105);
    drawBtn(40, 70, 80, 35, L"← 返回", backHover, RGB(100, 100, 100));

    // 页面标题
    settextcolor(RGB(30, 60, 120));
    settextstyle(36, 0, L"微软雅黑");
    outtextxy((WIN_WIDTH - textwidth(L"所有活动")) / 2, 80, L"所有活动");

    // 计算总页数
    int totalPublished = 0;
    Activity* temp = activity_head;
    while (temp) {
        if (temp->status == PUBLISHED) totalPublished++;
        temp = temp->next;
    }
    int totalPages = (totalPublished + ACTIVITIES_PER_PAGE - 1) / ACTIVITIES_PER_PAGE;

    // 修正页码范围
    if (currentActivityPage >= totalPages) currentActivityPage = max(0, totalPages - 1);

    // 提示信息
    settextcolor(RGB(100, 100, 100));
    settextstyle(18, 0, L"微软雅黑");
    wchar_t pageInfo[100];
    swprintf(pageInfo, 100, L"共 %d 页，当前第 %d 页 (已发布活动总数: %d)", totalPages, currentActivityPage + 1, totalPublished);
    outtextxy((WIN_WIDTH - textwidth(pageInfo)) / 2, 130, pageInfo);

    // 计算起始位置
    int listStartX = (WIN_WIDTH - 1100) / 2; // 居中
    int listStartY = 180;
    int itemWidth = 1100;
    int itemHeight = 80;
    int itemGap = 15;

    // 遍历链表，跳过前面页数的活动
    Activity* act = activity_head;
    int skipCount = currentActivityPage * ACTIVITIES_PER_PAGE;
    int displayedCount = 0;

    // 先跳过
    while (act && skipCount > 0) {
        if (act->status == PUBLISHED) skipCount--;
        act = act->next;
    }

    // 绘制当前页的活动
    while (act && displayedCount < ACTIVITIES_PER_PAGE) {
        if (act->status == PUBLISHED) {
            int y = listStartY + displayedCount * (itemHeight + itemGap);

            // 悬停效果
            bool isHover = (mouseX >= listStartX && mouseX <= listStartX + itemWidth &&
                mouseY >= y && mouseY <= y + itemHeight);
            COLORREF bgColor = isHover ? RGB(250, 250, 255) : RGB(255, 255, 255);
            drawRoundRect(listStartX, y, itemWidth, itemHeight, 10, RGB(200, 200, 200), bgColor);

            // 活动信息
            settextcolor(RGB(30, 60, 120));
            settextstyle(20, 0, L"微软雅黑");
            wchar_t title[100];
            swprintf(title, 100, L"【%d】%S", act->id, StringToWstring(act->name).c_str());
            outtextxy(listStartX + 20, y + 15, title);

            settextcolor(RGB(100, 100, 100));
            settextstyle(16, 0, L"微软雅黑");
            wchar_t info[200];
            swprintf(info, 200, L"时间: %S | 地点: %S | 报名: %d/%d",
                StringToWstring(act->start_time).c_str(),
                StringToWstring(act->location).c_str(),
                act->current_count, act->max_participants);
            outtextxy(listStartX + 20, y + 50, info);

            // 操作按钮
            bool btnHover = (mouseX >= listStartX + itemWidth - 100 && mouseX <= listStartX + itemWidth - 20 &&
                mouseY >= y + 25 && mouseY <= y + 55);
            drawBtn(listStartX + itemWidth - 100, y + 25, 80, 30, L"查看详情", btnHover, RGB(70, 150, 230));

            displayedCount++;
        }
        act = act->next;
    }

    // 如果没有活动
    if (displayedCount == 0) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(24, 0, L"微软雅黑");
        outtextxy((WIN_WIDTH - textwidth(L"暂无更多活动")) / 2, 350, L"暂无更多活动");
    }

    // 翻页按钮 (底部)
    int btnY = WIN_HEIGHT - 100;
    bool prevHover = (mouseX >= WIN_WIDTH / 2 - 150 && mouseX <= WIN_WIDTH / 2 - 50 && mouseY >= btnY && mouseY <= btnY + 40);
    bool nextHover = (mouseX >= WIN_WIDTH / 2 + 50 && mouseX <= WIN_WIDTH / 2 + 150 && mouseY >= btnY && mouseY <= btnY + 40);

    // 上一页
    if (currentActivityPage > 0) {
        drawBtn(WIN_WIDTH / 2 - 150, btnY, 100, 40, L"← 上一页", prevHover, RGB(100, 180, 100));
    }
    else {
        drawRoundRect(WIN_WIDTH / 2 - 150, btnY, 100, 40, 8, RGB(200, 200, 200), RGB(240, 240, 240));
        settextcolor(RGB(150, 150, 150));
        outtextxy(WIN_WIDTH / 2 - 110, btnY + 12, L"← 上一页");
    }

    // 下一页
    if (currentActivityPage < totalPages - 1) {
        drawBtn(WIN_WIDTH / 2 + 50, btnY, 100, 40, L"下一页 →", nextHover, RGB(70, 150, 230));
    }
    else {
        drawRoundRect(WIN_WIDTH / 2 + 50, btnY, 100, 40, 8, RGB(200, 200, 200), RGB(240, 240, 240));
        settextcolor(RGB(150, 150, 150));
        outtextxy(WIN_WIDTH / 2 + 90, btnY + 12, L"下一页 →");
    }
}
// ==================== 搜索功能 ====================
void performSearch() {
    searchResultCount = 0;
    if (wcslen(searchKeyword) == 0) return;
    wchar_t keywordLower[100];
    wcscpy(keywordLower, searchKeyword);
    _wcslwr(keywordLower);
    Activity* act = activity_head;
    while (act) {
        if (act->status == PUBLISHED) {
            wstring wName = StringToWstring(act->name);
            wchar_t titleLower[50];
            wcscpy(titleLower, wName.c_str());
            _wcslwr(titleLower);
            if (wcsstr(titleLower, keywordLower) != NULL) {
                ActivityCard* card = new ActivityCard();
                FillActivityCard(act, card);
                searchResults[searchResultCount++] = card;
            }
        }
        act = act->next;
    }
}

// ==================== 鼠标点击处理 ====================
void handleHomeMouseClick(int x, int y) {
    static bool shouldExit = false;
    if (x >= WIN_WIDTH - 100 && x <= WIN_WIDTH - 20 && y >= 30 && y <= 70) {
        g_studentShouldExit = true;
        return;
    }
    int searchX = WIN_WIDTH - 380, searchY = 32, searchW = 260, searchH = 36;
    if (x >= searchX && x <= searchX + searchW && y >= searchY && y <= searchY + searchH) {
        isSearchFocus = true;
        return;
    }
    else {
        if (isSearchFocus) isSearchFocus = false;
    }
    if (x >= WIN_WIDTH / 2 - 100 && x <= WIN_WIDTH / 2 + 100 && y >= WIN_HEIGHT - 60 && y <= WIN_HEIGHT - 20) {
        currentPage = PAGE_ALL_ACTIVITIES; // 跳转页面
        currentActivityPage = 0; // 重置页码
        return;
    }
    int rightX = WIN_WIDTH - 260, rightW = 190;
    if (x >= rightX + 20 && x <= rightX + rightW - 20) {
        if (y >= 305 && y <= 340) {
            // 称号详情弹窗
            wchar_t msg[800];
            int cnt = getRealEnrollmentCount(), dur = getRealTotalDuration();
            wchar_t tit[50];
            getRealTitle(tit, 50);
            swprintf(msg, 800, L"【当前称号】\n%s\n\n━━━━━━━━━━━━━━━━━━━━\n\n📊 活动统计\n\n• 报名活动数量：%d 项\n• 累计参与时长：%d 小时\n\n━━━━━━━━━━━━━━━━━━━━\n\n🏅 称号等级\n\n⭐ 活动大师：报名≥4项 且 时长≥8小时\n🏆 资深参与者：报名≥3项 且 时长≥6小时\n🌟 活跃成员：报名≥2项 且 时长≥4小时\n🌱 新手上路：报名≥1项 且 时长≥2小时\n📖 萌新小白：初始称号\n\n━━━━━━━━━━━━━━━━━━━━\n\n💡 提示：参加更多活动可提升称号！", tit, cnt, dur);
            MessageBoxW(GetHWnd(), msg, L"称号详情", MB_OK);
            return;
        }
        else if (y >= 350 && y <= 385) {
            currentPage = PAGE_MY_ACTIVITIES;
            return;
        }
        else if (y >= 395 && y <= 430) {
            wchar_t input[500] = { 0 };
            InputBox(input, 500, L"请输入您的建议：", L"提交建议", NULL, 0, 0, false);
            if (wcslen(input) > 0) {
                char contentA[500];
                WideCharToMultiByte(CP_ACP, 0, input, -1, contentA, 500, NULL, NULL);
                add_suggestion(g_currentStudentId, contentA);
                MessageBoxW(GetHWnd(), L"建议提交成功！感谢您的反馈。", L"提示", MB_OK);
            }
            return;
        }
        else if (y >= 440 && y <= 475) {
            currentPage = PAGE_HISTORY;
            return;
        }
    }
    // 活动卡片详情按钮
    int cardStartX = 330, cardStartY = 160, cardGap = 25;
    int idx = 0;
    Activity* act = activity_head;
    while (act && idx < 4) {
        if (act->status == PUBLISHED) {
            int col = idx % 2, row = idx / 2;
            int cardX = cardStartX + col * (CARD_WIDTH + cardGap);
            int cardY = cardStartY + row * (CARD_HEIGHT + cardGap);
            int btnX = cardX + CARD_WIDTH - 70, btnY = cardY + CARD_HEIGHT - 40;
            if (x >= btnX && x <= btnX + 55 && y >= btnY && y <= btnY + 28) {
                currentActivityId = act->id;
                currentPage = PAGE_DETAIL;
                return;
            }
            idx++;
        }
        act = act->next;
    }
}

void handleDetailMouseClick(int x, int y) {
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }
    Activity* act = getCurrentActivityPtr();
    if (!act) return;

    int btnX = (WIN_WIDTH - 200) / 2, infoCardY = 180, infoCardH = 530, btnY = infoCardY + infoCardH - 60;
    if (x >= btnX && x <= btnX + 200 && y >= btnY && y <= btnY + 45) {
        if (isActivityEnrolled(act->id)) {
            // ✅ 修复：将 char* 转换为 wchar_t*
            wstring wName = StringToWstring(act->name);
            wchar_t msg[200];
            swprintf(msg, 200, L"确定要取消报名【%s】吗？", wName.c_str());
            if (MessageBoxW(GetHWnd(), msg, L"确认取消", MB_YESNO) == IDYES)
                cancelEnrollment(act->id);
        }
        else {
            if (act->current_count < act->max_participants) addEnrollment(act->id);
            else MessageBoxW(GetHWnd(), L"活动已满，无法报名", L"提示", MB_OK);
        }
    }
}

void handleMyActivitiesMouseClick(int x, int y) {
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }
    int listStartY = 150, itemHeight = 140, itemGap = 15, itemWidth = 1000;
    int itemX = (WIN_WIDTH - itemWidth) / 2;
    int yPos = listStartY;
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == g_currentStudentId) {
            Activity* act = find_activity_by_id(e->activity_id);
            if (act && act->status == PUBLISHED) {
                if (x >= itemX + itemWidth - 210 && x <= itemX + itemWidth - 110 &&
                    y >= yPos + 50 && y <= yPos + 90) {
                    // 查看地图
                    wstring wLoc = StringToWstring(act->location);
                    wchar_t msg[500];
                    swprintf(msg, 500, L"【地图功能】\n\n活动地点：%s\n\n坐标信息：纬度 39.9042，经度 116.4074\n\n提示：实际应用中这里会调用地图API显示具体位置", wLoc.c_str());
                    MessageBoxW(GetHWnd(), msg, L"活动地图", MB_OK);
                    return;
                }
                else if (x >= itemX + itemWidth - 110 && x <= itemX + itemWidth - 20 &&
                    y >= yPos + 50 && y <= yPos + 90) {
                    wchar_t msg[200];
                    swprintf(msg, 200, L"确定要取消报名【%s】吗？", act->name);
                    if (MessageBoxW(GetHWnd(), msg, L"确认取消", MB_YESNO) == IDYES) cancelEnrollment(act->id);
                    return;
                }
                yPos += itemHeight + itemGap;
            }
        }
        e = e->next;
    }
}

void handleHistoryMouseClick(int x, int y) {
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) currentPage = PAGE_HOME;
}

void handleSearchResultMouseClick(int x, int y) {
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }
    int listStartY = 180, itemHeight = 120, itemGap = 15, itemWidth = 900;
    int itemX = (WIN_WIDTH - itemWidth) / 2;
    int yPos = listStartY;
    for (int i = 0; i < searchResultCount && yPos + itemHeight < WIN_HEIGHT - 50; i++) {
        if (x >= itemX + itemWidth - 100 && x <= itemX + itemWidth - 20 &&
            y >= yPos + 40 && y <= yPos + 75) {
            currentActivityId = searchResults[i]->id;
            currentPage = PAGE_DETAIL;
            return;
        }
        yPos += itemHeight + itemGap;
    }
}
// ==================== 新增：所有活动页面交互 ====================
void handleAllActivitiesMouseClick(int x, int y) {
    // 返回首页
    if (x >= 40 && x <= 120 && y >= 70 && y <= 105) {
        currentPage = PAGE_HOME;
        return;
    }

    // 翻页逻辑
    int btnY = WIN_HEIGHT - 100;
    int totalPages = (count_published_activities() + ACTIVITIES_PER_PAGE - 1) / ACTIVITIES_PER_PAGE; // 需要辅助函数计算总数

    if (x >= WIN_WIDTH / 2 - 150 && x <= WIN_WIDTH / 2 - 50 && y >= btnY && y <= btnY + 40) {
        if (currentActivityPage > 0) {
            currentActivityPage--;
        }
        return;
    }
    if (x >= WIN_WIDTH / 2 + 50 && x <= WIN_WIDTH / 2 + 150 && y >= btnY && y <= btnY + 40) {
        if (currentActivityPage < totalPages - 1) {
            currentActivityPage++;
        }
        return;
    }

    // 点击查看详情
    int listStartX = (WIN_WIDTH - 1100) / 2;
    int listStartY = 180;
    int itemWidth = 1100;
    int itemHeight = 80;
    int itemGap = 15;

    // 重新遍历以确定点击了哪个活动（简化版：只计算Y轴位置）
    // 注意：这里为了简化逻辑，我们直接通过坐标反推页码内的索引，实际链表遍历需同步跳过逻辑
    int rowIndex = (y - listStartY) / (itemHeight + itemGap);
    if (rowIndex >= 0 && rowIndex < ACTIVITIES_PER_PAGE) {
        // 这里需要重新遍历链表找到第 (currentActivityPage * 8 + rowIndex) 个发布的活动
        int targetIndex = currentActivityPage * ACTIVITIES_PER_PAGE + rowIndex;
        int currentIndex = 0;
        Activity* act = activity_head;
        while (act) {
            if (act->status == PUBLISHED) {
                if (currentIndex == targetIndex) {
                    // 检查是否点击了“查看详情”按钮区域
                    int btnX = listStartX + itemWidth - 100;
                    int btnY = listStartY + rowIndex * (itemHeight + itemGap) + 25;
                    if (x >= btnX && x <= btnX + 80 && y >= btnY && y <= btnY + 30) {
                        currentActivityId = act->id;
                        currentPage = PAGE_DETAIL;
                        return;
                    }
                    break;
                }
                currentIndex++;
            }
            act = act->next;
        }
    }
}

// 辅助函数：计算已发布活动总数 (放在文件任意位置)
int count_published_activities() {
    int count = 0;
    Activity* a = activity_head;
    while (a) {
        if (a->status == PUBLISHED) count++;
        a = a->next;
    }
    return count;
}
void handleKeyInput(wchar_t ch) {
    if (!isSearchFocus) return;
    if (ch == VK_BACK) {
        int len = wcslen(searchKeyword);
        if (len > 0) searchKeyword[len - 1] = L'\0';
    }
    else if (ch == VK_RETURN) {
        performSearch();
        if (searchResultCount > 0 || wcslen(searchKeyword) > 0) currentPage = PAGE_SEARCH_RESULT;
        isSearchFocus = false;
    }
    else {
        if (wcslen(searchKeyword) < 98) searchKeyword[wcslen(searchKeyword)] = ch;
    }
}

// ==================== 学生主界面入口 ====================
void runStudentMainPage(int studentId) {
    g_currentStudentId = studentId;   // 保存学生ID
    ExMessage msg;
    bool running = true;
    mouseX = mouseY = 0;
    currentPage = PAGE_HOME;
    currentActivityId = 0;
    isSearchFocus = false;
    wmemset(searchKeyword, 0, 100);
    g_studentShouldExit = false;      // 重置退出标志

    BeginBatchDraw();
    while (running) {
        while (peekmessage(&msg)) {
            if (msg.message == WM_MOUSEMOVE) {
                mouseX = msg.x;
                mouseY = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                if (currentPage == PAGE_HOME) handleHomeMouseClick(msg.x, msg.y);
                else if (currentPage == PAGE_DETAIL) handleDetailMouseClick(msg.x, msg.y);
                else if (currentPage == PAGE_MY_ACTIVITIES) handleMyActivitiesMouseClick(msg.x, msg.y);
                else if (currentPage == PAGE_HISTORY) handleHistoryMouseClick(msg.x, msg.y);
                else if (currentPage == PAGE_SEARCH_RESULT) handleSearchResultMouseClick(msg.x, msg.y);
            }
            else if (msg.message == WM_CHAR) {
                handleKeyInput((wchar_t)msg.ch);
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    if (currentPage != PAGE_HOME) {
                        currentPage = PAGE_HOME;
                        wmemset(searchKeyword, 0, 100);
                        searchResultCount = 0;
                    }
                    else {
                        running = false;
                    }
                }
            }
        }
        if (g_studentShouldExit) running = false;   // 检查退出标志

        switch (currentPage) {
        case PAGE_HOME: drawStudentHomePage(); break;
        case PAGE_DETAIL: drawActivityDetailPage(); break;
        case PAGE_MY_ACTIVITIES: drawMyActivitiesPage(); break;
        case PAGE_HISTORY: drawHistoryPage(); break;
        case PAGE_SEARCH_RESULT: drawSearchResultPage(); break;
        case PAGE_ALL_ACTIVITIES:drawAllActivitiesPage(); break;
        }
        FlushBatchDraw();
        Sleep(16);
    }
    EndBatchDraw();
}