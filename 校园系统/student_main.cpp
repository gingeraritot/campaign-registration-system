#include <graphics.h> // EasyX 头文件
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>
#include "head.h"
#include <stdio.h>

using namespace std;

// 全局样式定义
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;
COLORREF COLOR_PRIMARY = RGB(66, 135, 245); // 蓝色主题
COLORREF COLOR_BG = RGB(240, 240, 240);

// 简单的按钮结构
struct Button {
    int x, y, w, h;
    const wchar_t* text;
};

// 绘制按钮
void drawButton(Button btn, bool hover) {
    setfillcolor(hover ? RGB(40, 100, 200) : COLOR_PRIMARY);
    fillroundrect(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, 10, 10);
    settextcolor(WHITE);
    settextstyle(20, 0, _T("微软雅黑"));
    int tx = btn.x + (btn.w - textwidth(btn.text)) / 2;
    int ty = btn.y + (btn.h - textheight(btn.text)) / 2;
    outtextxy(tx, ty, btn.text);
}

// 检查鼠标是否在按钮上
bool isHover(int x, int y, Button btn) {
    MOUSEMSG m = GetMouseMsg();
    return (m.x >= btn.x && m.x <= btn.x + btn.w && m.y >= btn.y && m.y <= btn.y + btn.h);
}

// ==========================================
// 页面 1: 学生 - 活动列表
// ==========================================
void showStudentListPage() {
    cleardevice();
    setbkcolor(COLOR_BG);
    cleardevice();

    // 绘制标题
    settextcolor(BLACK);
    settextstyle(30, 0, _T("微软雅黑"));
    outtextxy(50, 20, _T("校园活动管理系统 - 学生端"));

    // 绘制列表头
    line(50, 70, 750, 70);
    outtextxy(60, 80, _T("活动名称"));
    outtextxy(250, 80, _T("时间"));
    outtextxy(400, 80, _T("地点"));
    outtextxy(600, 80, _T("操作"));

    // 从链表读取数据并绘制（只显示已发布的活动）
    int startY = 110;
    int rowHeight = 60;
    int index = 0;

    Activity* cur = activity_head;
    char buffer[512];
    wchar_t wbuffer[256];
    size_t converted = 0;

    while (cur) {
        if (cur->status == PUBLISHED) {
            int cy = startY + index * rowHeight;

            // 活动名称
            sprintf(buffer, "%s", cur->name);
            outtextxy(60, cy + 10, buffer);

            // 时间
            sprintf(buffer, "%s", cur->start_time);
            outtextxy(250, cy + 10, buffer);

            // 地点
            sprintf(buffer, "%s", cur->location);
            outtextxy(400, cy + 10, buffer);

            // 绘制“详情”按钮（这里只绘制，交互可以在循环里处理）
            Button detailBtn = { 600, cy + 10, 100, 30, "详情" };
            drawButton(detailBtn, false);

            index++;
        }
        cur = cur->next;
    }
}

// ==========================================
// 页面 2: 活动详情页 (对应你的手稿图2)
// ==========================================
void showDetailPage(int actIndex) {
    if (actIndex == -1) return;
    Activity* act = activity_head;

    cleardevice();
    setbkcolor(WHITE);
    cleardevice();

    settextcolor(BLACK);
    settextstyle(24, 0, _T("微软雅黑"));

    // 绘制返回按钮
    Button backBtn = { 20, 20, 80, 30, "返回" };
    drawButton(backBtn, false);

    // 绘制详情内容
    outtextxy(100, 100, _T("活动名称: "));
    outtextxy(250, 100, act->name);

    outtextxy(100, 150, _T("活动时间: "));
    outtextxy(250, 150, act->start_time);

    outtextxy(100, 200, _T("活动地点: "));
    outtextxy(250, 200, act->location);

    outtextxy(100, 250, _T("人数限制: "));
    char pCount[20];
    sprintf(pCount, "%d / %d", act->current_count, act->max_participants);
    outtextxy(250, 250, pCount);

    outtextxy(100, 300, _T("活动描述: "));
    outtextxy(250, 300, act->description);

    // 报名按钮
    Button applyBtn = { 250, 400, 120, 40, "我要报名" };

    // 如果满员，显示不可报名
    if (act->current_count >= act->max_participants) {
        setfillcolor(LIGHTGRAY);
        fillroundrect(applyBtn.x, applyBtn.y, applyBtn.x + applyBtn.w, applyBtn.y + applyBtn.h, 10, 10);
        settextcolor(DARKGRAY);
        outtextxy(applyBtn.x + 10, applyBtn.y + 10, _T("名额已满"));
    }
    else {
        drawButton(applyBtn, false);
    }

    // 这里需要写一个循环来监听鼠标点击“返回”或“报名”
    // 为了代码简洁，这里仅做静态展示
    while (true) {
        MOUSEMSG m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            // 简单的点击检测逻辑
            if (m.x > backBtn.x && m.x < backBtn.x + backBtn.w && m.y > backBtn.y && m.y < backBtn.y + backBtn.h) {
                showStudentListPage(); // 返回列表
                break;
            }
            // 报名逻辑
            if (act->current_count < act->max_participants) {
                if (m.x > applyBtn.x && m.x < applyBtn.x + applyBtn.w && m.y > applyBtn.y && m.y < applyBtn.y + applyBtn.h) {
                    // 调用逻辑层更新人数
					act = act->next; // 这里需要找到对应的活动节点，实际项目中应有更好的数据结构支持
                    save_activities(ACTIVITIES_FILE);// 保存到文件
                    MessageBox(GetHWnd(), _T("报名成功！"), _T("提示"), MB_OK);
                    showDetailPage(actIndex); // 刷新页面
                    break;
                }
            }
        }
    }
}


// ==========================================
// 主程序入口
// ==========================================
int main() {
    // 1. 初始化图形窗口
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    setrendermode(RENDER_MANUAL); // 开启双缓冲，防止闪烁

    // 2. 加载数据（从文件读取）
    load_activities(ACTIVITIES_FILE);

    // 3. 简单的菜单循环
    while (true) {
        cleardevice();
        setbkcolor(WHITE);
        cleardevice();

        outtextxy(300, 100, _T("1. 学生端 (查看活动)"));
        outtextxy(300, 200, _T("2. 管理员端 (发布活动)"));
        outtextxy(300, 300, _T("3. 退出"));

        MOUSEMSG m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            if (m.y > 100 && m.y < 130) {
                showStudentListPage(); // 进入学生列表
            }
            else if (m.y > 200 && m.y < 230) {
                // 管理员逻辑...
            }
            else if (m.y > 300 && m.y < 330) {
                break;
            }
        }
    }

    closegraph();
    return 0;
}

