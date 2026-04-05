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

// 简单的测试页面 - 用来验证 main.cpp 和 studentman.cpp 是否连接成功。
void runStudentMainPage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // 显示一个大大的成功提示
    settextcolor(RGB(0, 150, 0));
    settextstyle(48, 0, L"微软雅黑");
    const wchar_t* successMsg = L"学生主界面连接成功！";
    outtextxy((WIN_WIDTH - textwidth(successMsg)) / 2, 200, successMsg);

    settextcolor(RGB(30, 60, 120));
    settextstyle(28, 0, L"微软雅黑");
    const wchar_t* tipMsg = L"这是从 studentman.cpp 中调用的页面";
    outtextxy((WIN_WIDTH - textwidth(tipMsg)) / 2, 300, tipMsg);

    settextstyle(24, 0, L"微软雅黑");
    settextcolor(RGB(100, 100, 100));
    const wchar_t* testMsg = L"证明 main.cpp 和 studentman.cpp 连接成功！";
    outtextxy((WIN_WIDTH - textwidth(testMsg)) / 2, 400, testMsg);

    settextstyle(20, 0, L"微软雅黑");
    const wchar_t* backMsg = L"点击任意位置返回登录页";
    outtextxy((WIN_WIDTH - textwidth(backMsg)) / 2, 550, backMsg);

    // 等待鼠标点击，然后返回
    ExMessage msg;
    while (true) {
        while (peekmessage(&msg)) {
            if (msg.message == WM_LBUTTONDOWN) {
                return;  // 点击后返回登录页
            }
        }
        Sleep(10);
    }
}