#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include "head.h"

// 全局基础变量（底层必备）
int screenWidth = 800;
int screenHeight = 600;
User currentUser;
int currentPage = 0;

// 绘制欢迎页（底层简单界面）
void DrawWelcomePage() {
    cleardevice();
    setbkcolor(WHITE);
    setbkmode(TRANSPARENT);

    settextcolor(BLACK);
    settextstyle(30, 0, _T("微软雅黑"));
    outtextxy(250, 250, _T("志愿者活动管理系统"));
    outtextxy(300, 300, _T("底层框架已启动"));
}

int main() {
    // 1. 初始化图形界面
    initgraph(screenWidth, screenHeight);

    // 2. 调用团队初始化函数
    init_system();

    // 3. 主循环，保持界面运行
    while (1) {
        if (currentPage == 0) {
            DrawWelcomePage();
        }
        Sleep(30);
    }

    closegraph();
    return 0;
}