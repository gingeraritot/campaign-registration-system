#include <graphics.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "head.h"
#include <regex>

// ==================== 全局常量 ====================
const int WIN_W = 1200;
const int WIN_H = 800;
const int CARD_W = 280;
const int CARD_H = 130;
const int BTN_W = 200;
const int BTN_H = 50;
const int INPUT_W = 300;
const int INPUT_H = 45;

// 颜色定义
const COLORREF BG_MAIN = RGB(240, 245, 250);
const COLORREF CARD_BG = RGB(255, 255, 255);
const COLORREF BTN_BLUE = RGB(70, 150, 230);
const COLORREF BTN_BLUE_HOVER = RGB(100, 180, 255);
const COLORREF BTN_GREEN = RGB(46, 204, 113);
const COLORREF BTN_GREEN_HOVER = RGB(39, 174, 96);
const COLORREF BTN_RED = RGB(231, 76, 60);
const COLORREF BTN_RED_HOVER = RGB(192, 57, 43);
const COLORREF BTN_ORANGE = RGB(241, 196, 15);
const COLORREF BTN_ORANGE_HOVER = RGB(243, 156, 18);
const COLORREF TEXT_DARK = RGB(44, 62, 80);
const COLORREF TEXT_LIGHT = RGB(127, 140, 141);
const COLORREF BORDER = RGB(220, 220, 220);
const COLORREF FOCUS = RGB(70, 150, 230);

// 页面枚举
enum AdminPage {
    PAGE_HOME,
    PAGE_ACT_LIST,
    PAGE_ACT_ADD,
    PAGE_DRAFT,
    PAGE_CAT_MANAGE,
    PAGE_ENROLL_VIEW,
    PAGE_SUGGEST_VIEW
};

// 全局变量
static enum AdminPage currentPage = PAGE_HOME;
static bool g_adminShouldExit = false;
static int mouseX = 0, mouseY = 0;
static int focusIdx = -1;
static int cursorPos = 0;

// 表单数据
static wchar_t actName[50] = {0};
static wchar_t actTime[20] = {0};
static wchar_t actLoc[100] = {0};
static wchar_t actDesc[500] = {0};
static wchar_t actReward[50] = {0};
static wchar_t actItems[200] = {0};
static int actLimit = 0, actDuration = 0, actCatId = 0;
static int editingActId = 0;

static wchar_t catName[20] = {0};
static wchar_t searchId[20] = {0};
static Enrollment* enrollList[100];
static int enrollCount = 0;


// ==================== 辅助函数 ====================
static char* WcharToChar(const wchar_t* ws) {
    int len = WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL);
    char* s = (char*)malloc(len);
    if (s) {
        WideCharToMultiByte(CP_ACP, 0, ws, -1, s, len, NULL, NULL);
    }
    return s;
}

static wchar_t* CharToWchar(const char* s) {
    int len = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
    wchar_t* ws = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (ws) {
        MultiByteToWideChar(CP_ACP, 0, s, -1, ws, len);
    }
    return ws;
}

static void ShowMsg(const wchar_t* msg) {
    MessageBoxW(GetHWnd(), msg, L"提示", MB_OK);
}

static void ClearActForm() {
    actName[0] = 0;
    actTime[0] = 0;
    actLoc[0] = 0;
    actDesc[0] = 0;
    actReward[0] = 0;
    actItems[0] = 0;
    actLimit = actDuration = actCatId = 0;
    editingActId = 0;
    focusIdx = -1;
    cursorPos = 0;
}

static void ClearCatForm() {
    catName[0] = 0;
    focusIdx = -1;
    cursorPos = 0;
}

// ==================== 绘图函数 ====================
static void drawRoundRect(int x, int y, int w, int h, int r, COLORREF fill, COLORREF border) {
    setfillcolor(fill);
    setlinecolor(border);
    fillroundrect(x, y, x + w, y + h, r, r);
}

static void drawBtn(int x, int y, int w, int h, const wchar_t* text, bool hover, COLORREF color) {
    COLORREF btnColor = hover ? BTN_BLUE_HOVER : color;
    drawRoundRect(x, y, w, h, 8, btnColor, RGB(150, 150, 150));
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, L"微软雅黑");
    int tw = textwidth(text);
    int th = textheight(text);
    outtextxy(x + (w - tw) / 2, y + (h - th) / 2, text);
}

static void drawCard(int x, int y, int w, int h, const wchar_t* line1, const wchar_t* line2) {
    drawRoundRect(x, y, w, h, 10, CARD_BG, BORDER);
    settextcolor(TEXT_DARK);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(x + 15, y + 20, line1);
    if (line2) {
        settextcolor(TEXT_LIGHT);
        settextstyle(16, 0, L"微软雅黑");
        outtextxy(x + 15, y + 55, line2);
    }
}

static void drawMultilineText(int x, int y, int w, int h, const wchar_t* text, int lineHeight) {
    settextcolor(TEXT_DARK);
    settextstyle(18, 0, L"微软雅黑");
    int maxCharsPerLine = w / 18;
    if (maxCharsPerLine <= 0) maxCharsPerLine = 1;
    
    int startY = y + 12;
    int line = 0;
    int textLen = wcslen(text);
    int start = 0;
    
    while (start < textLen && startY + line * lineHeight < y + h - 10) {
        int end = start + maxCharsPerLine;
        if (end >= textLen) end = textLen;
        
        wchar_t lineText[512];
        wcsncpy(lineText, text + start, end - start);
        lineText[end - start] = 0;
        
        outtextxy(x + 10, startY + line * lineHeight, lineText);
        start = end;
        line++;
    }
}

static void drawSinglelineBox(int x, int y, int w, int h, const wchar_t* label, wchar_t* content, bool focus, int* cursor) {
    settextcolor(TEXT_DARK);
    settextstyle(21, 0, L"微软雅黑");
    outtextxy(x - 90, y + 12, label);
    setlinecolor(focus ? FOCUS : BORDER);
    setfillcolor(WHITE);
    drawRoundRect(x, y, w, h, 6, WHITE, focus ? FOCUS : BORDER);
    
    settextcolor(TEXT_DARK);
    outtextxy(x + 10, y + 12, content);
    
    if (focus && cursor) {
        wchar_t sub[512];
        wcsncpy(sub, content, *cursor);
        sub[*cursor] = 0;
        int textW = textwidth(sub);
        int cursorX = x + 10 + textW;
        setlinecolor(TEXT_DARK);
        line(cursorX, y + 8, cursorX, y + h - 8);
    }
}

static void drawNumberBox(int x, int y, int w, int h, const wchar_t* label, int* value, bool focus, int* cursor) {
    wchar_t buf[20];
    swprintf(buf, 20, L"%d", *value);
    drawSinglelineBox(x, y, w, h, label, buf, focus, cursor);
    if (!focus) {
        *value = _wtoi(buf);
    }
}

// ==================== 页面绘制 ====================
static void drawTopBar() {
    setfillcolor(RGB(30, 60, 120));
    solidrectangle(0, 0, WIN_W, 80);
    settextcolor(WHITE);
    settextstyle(36, 0, L"微软雅黑");
    outtextxy(40, 20, L"校园活动管理系统");
    settextstyle(22, 0, L"微软雅黑");
    outtextxy(40, 55, L"管理员后台");
}

static void drawHome() {
    cleardevice();
    drawTopBar();
    
    settextcolor(TEXT_DARK);
    settextstyle(26, 0, L"微软雅黑");
    outtextxy(40, 120, L"功能列表");
    
    const wchar_t* funcs[6] = { L"活动列表", L"新增活动", L"草稿箱", L"分类管理", L"报名查看", L"建议箱" };
    int startX = 30;
    int startY = 160;
    int gapX = 40;
    int gapY = 30;
    
    for (int i = 0; i < 6; i++) {
        int col = i % 3;
        int row = i / 3;
        int x = startX + col * (CARD_W + gapX);
        int y = startY + row * (CARD_H + gapY);
        bool hover = (mouseX >= x && mouseX <= x + CARD_W && mouseY >= y && mouseY <= y + CARD_H);
        COLORREF bg = hover ? RGB(250, 250, 255) : CARD_BG;
        drawRoundRect(x, y, CARD_W, CARD_H, 10, bg, BORDER);
        settextcolor(TEXT_DARK);
        settextstyle(22, 0, L"微软雅黑");
        outtextxy(x + CARD_W / 2 - textwidth(funcs[i]) / 2, y + CARD_H / 2 - 15, funcs[i]);
    }
    
    bool backHover = (mouseX >= WIN_W - 120 && mouseX <= WIN_W - 30 && mouseY >= 20 && mouseY <= 60);
    drawBtn(WIN_W - 120, 20, 90, 45, L"退出", backHover, BTN_BLUE);
}

static void drawActList() {
    cleardevice();
    drawTopBar();
    settextcolor(TEXT_DARK);
    settextstyle(26, 0, L"微软雅黑");
    outtextxy(40, 110, L"活动列表");
    drawBtn(WIN_W - 120, 110, 90, 40, L"返回", false, BTN_BLUE);
    
    Activity* p = activity_head;
    int y = 170;
    while (p && y < WIN_H - 100) {
        if (p->status == DRAFT) { p = p->next; continue; }
        
        wchar_t* wName = CharToWchar(p->name);
        wchar_t* wTime = CharToWchar(p->start_time);
        wchar_t wStatus[10];
        wcscpy(wStatus, (p->status == PUBLISHED) ? L"已发布" : L"已下架");
        
        wchar_t line1[300];
        swprintf(line1, 300, L"[ID:%d] %s  |  %s  |  %d/%d", p->id, wName, wTime, p->current_count, p->max_participants);
        wchar_t line2[200];
        swprintf(line2, 200, L"状态：%s", wStatus);
        drawCard(60, y, WIN_W - 300, 90, line1, line2);
        
        free(wName);
        free(wTime);
        
        int btnX = 680;
        int btnY = y + 25;
        bool editHover = (mouseX >= btnX && mouseX <= btnX + 70 && mouseY >= btnY && mouseY <= btnY + 35);
        bool offHover = (p->status == PUBLISHED && mouseX >= btnX + 80 && mouseX <= btnX + 150 && mouseY >= btnY && mouseY <= btnY + 35);
        bool delHover = (mouseX >= btnX + 160 && mouseX <= btnX + 230 && mouseY >= btnY && mouseY <= btnY + 35);
        
        drawBtn(btnX, btnY, 70, 35, L"编辑", editHover, BTN_ORANGE);
        if (p->status == PUBLISHED)
            drawBtn(btnX + 80, btnY, 70, 35, L"下架", offHover, BTN_RED);
        drawBtn(btnX + 160, btnY, 70, 35, L"删除", delHover, BTN_RED);
        
        y += 110;
        p = p->next;
    }
}

static void drawActAdd() {
    cleardevice();
    drawTopBar();
    settextcolor(TEXT_DARK);
    settextstyle(26, 0, L"微软雅黑");
    outtextxy(40, 110, editingActId ? L"编辑活动" : L"新增活动");
    drawBtn(WIN_W - 120, 110, 90, 40, L"返回", false, BTN_BLUE);
    
    int x1 = 180, x2 = 600;
    int y1 = 160, y2 = 215, y3 = 270, y4 = 325, y5 = 380, y6 = 435;
    
    drawSinglelineBox(x1, y1, INPUT_W, INPUT_H, L"活动名称", actName, focusIdx == 0, &cursorPos);
    drawSinglelineBox(x1, y2, INPUT_W, INPUT_H, L"开始时间", actTime, focusIdx == 1, &cursorPos);
    drawSinglelineBox(x1, y3, INPUT_W, INPUT_H, L"活动地点", actLoc, focusIdx == 2, &cursorPos);
    drawNumberBox(x1, y4, INPUT_W, INPUT_H, L"最大人数", &actLimit, focusIdx == 3, &cursorPos);
    drawNumberBox(x1, y5, INPUT_W, INPUT_H, L"活动时长", &actDuration, focusIdx == 4, &cursorPos);
    drawNumberBox(x1, y6, INPUT_W, INPUT_H, L"分类ID", &actCatId, focusIdx == 5, &cursorPos);
    
    drawSinglelineBox(x2, y1, INPUT_W, INPUT_H, L"活动奖励", actReward, focusIdx == 6, &cursorPos);
    drawSinglelineBox(x2, y2, INPUT_W, INPUT_H, L"需携带物品", actItems, focusIdx == 7, &cursorPos);
    
    // 描述框
    drawRoundRect(x2, y3, INPUT_W, 120, 6, WHITE, BORDER);
    settextcolor(TEXT_DARK);
    settextstyle(21, 0, L"微软雅黑");
    outtextxy(x2 - 90, y3 + 12, L"活动描述");
    drawMultilineText(x2, y3, INPUT_W, 120, actDesc, 22);
    
    int btnY = 650;
    int btnW = 130;
    int btnSpacing = 30;
    int btnStartX = 250;
    
    bool saveHover = (mouseX >= btnStartX && mouseX <= btnStartX + btnW && mouseY >= btnY && mouseY <= btnY + 45);
    bool pubHover = (mouseX >= btnStartX + btnW + btnSpacing && mouseX <= btnStartX + 2 * btnW + btnSpacing && mouseY >= btnY && mouseY <= btnY + 45);
    bool uploadHover = (mouseX >= btnStartX + 2 * (btnW + btnSpacing) && mouseX <= btnStartX + 3 * btnW + 2 * btnSpacing && mouseY >= btnY && mouseY <= btnY + 45);
    bool aiHover = (mouseX >= btnStartX + 3 * (btnW + btnSpacing) && mouseX <= btnStartX + 4 * btnW + 3 * btnSpacing && mouseY >= btnY && mouseY <= btnY + 45);
    
    drawBtn(btnStartX, btnY, btnW, 45, editingActId ? L"更新并保存" : L"保存草稿", saveHover, BTN_GREEN);
    drawBtn(btnStartX + btnW + btnSpacing, btnY, btnW, 45, editingActId ? L"更新发布" : L"发布", pubHover, BTN_BLUE);
    drawBtn(btnStartX + 2 * (btnW + btnSpacing), btnY, btnW, 45, L"从TXT导入", uploadHover, BTN_ORANGE);
    drawBtn(btnStartX + 3 * (btnW + btnSpacing), btnY, btnW, 45, L"智能填充", aiHover, BTN_ORANGE);
}

// ==================== 事件处理 ====================
static void handleChar(wchar_t ch, wchar_t* target, int maxLen, int* cursor) {
    int len = wcslen(target);
    
    if (ch == VK_LEFT) {
        if (*cursor > 0) (*cursor)--;
    }
    else if (ch == VK_RIGHT) {
        if (*cursor < len) (*cursor)++;
    }
    else if (ch == VK_BACK) {
        if (*cursor > 0 && len > 0) {
            for (int i = *cursor - 1; i < len; i++) {
                target[i] = target[i + 1];
            }
            (*cursor)--;
        }
    }
    else if (ch == VK_DELETE) {
        if (*cursor < len) {
            for (int i = *cursor; i < len; i++) {
                target[i] = target[i + 1];
            }
        }
    }
    else if (ch >= 32 && ch < 0xFFFF && len < maxLen - 1) {
        for (int i = len; i > *cursor; i--) {
            target[i] = target[i - 1];
        }
        target[*cursor] = ch;
        (*cursor)++;
        target[len + 1] = 0;
    }
}

static void handleGlobalChar(wchar_t ch) {
    if (currentPage == PAGE_ACT_ADD && focusIdx != -1) {
        switch (focusIdx) {
        case 0: handleChar(ch, actName, 50, &cursorPos); break;
        case 1: handleChar(ch, actTime, 20, &cursorPos); break;
        case 2: handleChar(ch, actLoc, 100, &cursorPos); break;
        case 6: handleChar(ch, actReward, 50, &cursorPos); break;
        case 7: handleChar(ch, actItems, 200, &cursorPos); break;
        case 3: 
            if (ch >= L'0' && ch <= L'9') actLimit = actLimit * 10 + (ch - L'0');
            else if (ch == VK_BACK) actLimit /= 10;
            break;
        case 4:
            if (ch >= L'0' && ch <= L'9') actDuration = actDuration * 10 + (ch - L'0');
            else if (ch == VK_BACK) actDuration /= 10;
            break;
        case 5:
            if (ch >= L'0' && ch <= L'9') actCatId = actCatId * 10 + (ch - L'0');
            else if (ch == VK_BACK) actCatId /= 10;
            break;
        }
    }
    else if (currentPage == PAGE_CAT_MANAGE && focusIdx == 0) {
        handleChar(ch, catName, 20, &cursorPos);
    }
    else if (currentPage == PAGE_ENROLL_VIEW && focusIdx == 0) {
        int len = wcslen(searchId);
        if (ch >= L'0' && ch <= L'9' && len < 19) {
            searchId[len] = ch;
            searchId[len + 1] = 0;
        }
        else if (ch == VK_BACK && len > 0) {
            searchId[len - 1] = 0;
        }
    }
}

static void handleClick(int x, int y) {
    if (x >= WIN_W - 120 && x <= WIN_W - 30 && y >= 20 && y <= 65) {
        g_adminShouldExit = true;
        return;
    }
    
    if (currentPage != PAGE_HOME) {
        if (x >= WIN_W - 120 && x <= WIN_W - 30 && y >= 110 && y <= 150) {
            currentPage = PAGE_HOME;
            ClearActForm();
            ClearCatForm();
            return;
        }
    }
    
    switch (currentPage) {
    case PAGE_HOME: {
        int startX = 60, startY = 160;
        int gapX = 40, gapY = 30;
        for (int i = 0; i < 6; i++) {
            int col = i % 3;
            int row = i / 3;
            int cx = startX + col * (CARD_W + gapX);
            int cy = startY + row * (CARD_H + gapY);
            if (x >= cx && x <= cx + CARD_W && y >= cy && y <= cy + CARD_H) {
                switch (i) {
                case 0: currentPage = PAGE_ACT_LIST; break;
                case 1: ClearActForm(); currentPage = PAGE_ACT_ADD; break;
                case 2: currentPage = PAGE_DRAFT; break;
                case 3: currentPage = PAGE_CAT_MANAGE; break;
                case 4: currentPage = PAGE_ENROLL_VIEW; break;
                case 5: currentPage = PAGE_SUGGEST_VIEW; break;
                }
                return;
            }
        }
        break;
    }
    
    case PAGE_ACT_ADD: {
        int x1 = 180, x2 = 600;
        int y1 = 160, y2 = 215, y3 = 270, y4 = 325, y5 = 380, y6 = 435;
        focusIdx = -1;
        
        if (x >= x1 && x <= x1 + INPUT_W) {
            if (y >= y1 && y <= y1 + INPUT_H) { focusIdx = 0; cursorPos = wcslen(actName); }
            else if (y >= y2 && y <= y2 + INPUT_H) { focusIdx = 1; cursorPos = wcslen(actTime); }
            else if (y >= y3 && y <= y3 + INPUT_H) { focusIdx = 2; cursorPos = wcslen(actLoc); }
            else if (y >= y4 && y <= y4 + INPUT_H) { focusIdx = 3; cursorPos = 0; }
            else if (y >= y5 && y <= y5 + INPUT_H) { focusIdx = 4; cursorPos = 0; }
            else if (y >= y6 && y <= y6 + INPUT_H) { focusIdx = 5; cursorPos = 0; }
        }
        else if (x >= x2 && x <= x2 + INPUT_W) {
            if (y >= y1 && y <= y1 + INPUT_H) { focusIdx = 6; cursorPos = wcslen(actReward); }
            else if (y >= y2 && y <= y2 + INPUT_H) { focusIdx = 7; cursorPos = wcslen(actItems); }
        }
        break;
    }
    }
}

// ==================== 主循环 ====================
void runAdminMainPage() {
    currentPage = PAGE_HOME;
    focusIdx = -1;
    cursorPos = 0;
    ClearActForm();
    ClearCatForm();
    enrollCount = 0;
    g_adminShouldExit = false;
    
    BeginBatchDraw();
    while (!g_adminShouldExit) {
        ExMessage msg;
        while (peekmessage(&msg)) {
            if (msg.message == WM_MOUSEMOVE) { mouseX = msg.x; mouseY = msg.y; }
            else if (msg.message == WM_LBUTTONDOWN) handleClick(msg.x, msg.y);
            else if (msg.message == WM_CHAR) { 
                if (msg.ch >= 32 && msg.ch < 0xFFFF) 
                    handleGlobalChar((wchar_t)msg.ch); 
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_LEFT) handleGlobalChar(VK_LEFT);
                else if (msg.vkcode == VK_RIGHT) handleGlobalChar(VK_RIGHT);
                else if (msg.vkcode == VK_BACK) handleGlobalChar(VK_BACK);
                else if (msg.vkcode == VK_DELETE) handleGlobalChar(VK_DELETE);
            }
        }
        
        switch (currentPage) {
        case PAGE_HOME: drawHome(); break;
        case PAGE_ACT_LIST: drawActList(); break;
        case PAGE_ACT_ADD: drawActAdd(); break;
        case PAGE_DRAFT: /* 需要实现 */ break;
        case PAGE_CAT_MANAGE: /* 需要实现 */ break;
        case PAGE_ENROLL_VIEW: /* 需要实现 */ break;
        case PAGE_SUGGEST_VIEW: /* 需要实现 */ break;
        }
        FlushBatchDraw();
        Sleep(10);
    }
    EndBatchDraw();
    cleardevice();
}