#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <easyx.h>
#include <windows.h>
#include "head.h"

// ==================== 全局常量定义 ====================
#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800
#define BTN_WIDTH     200
#define BTN_HEIGHT    50
#define BTN_GAP       80
#define INPUT_WIDTH   300
#define INPUT_HEIGHT  40

<<<<<<< HEAD
#define ADMIN_USER    L"admin"
#define ADMIN_PWD     L"123456"
=======
// ==================== 演示用账号密码 ====================
#define ADMIN_USER    "admin"
#define ADMIN_PWD     "123456"
>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6

typedef struct {
    wchar_t username[20];
    wchar_t password[20];
} StudentAccount;

StudentAccount stuAccounts[] = {
    {L"jiang", L"123"},
    {L"yang", L"456"},
    {L"chen", L"111"},
    {L"guo", L"222"}
};
int stuCount = sizeof(stuAccounts) / sizeof(StudentAccount);

typedef enum {
    PAGE_WELCOME,
    PAGE_LOGIN,
    PAGE_REGISTER,
    PAGE_STUDENT_MAIN,
    PAGE_ADMIN_MAIN
} PageType;

// ==================== 工具函数 ====================
void drawButton(int x, int y, int w, int h, const wchar_t* text, bool isHover) {
    COLORREF btnColor = isHover ? RGB(100, 180, 255) : RGB(70, 150, 230);
    setfillcolor(btnColor);
    setlinecolor(RGB(30, 100, 180));
    fillroundrect(x, y, x + w, y + h, 10, 10);
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, L"微软雅黑");
    int textX = x + (w - textwidth(text)) / 2;
    int textY = y + (h - textheight(text)) / 2;
    outtextxy(textX, textY, text);
}

void drawInputBox(int x, int y, int w, int h, const wchar_t* label, const wchar_t* content, bool isFocus) {
    setfillcolor(WHITE);
    setlinecolor(isFocus ? RGB(0, 120, 255) : RGB(180, 180, 180));
    fillroundrect(x, y, x + w, y + h, 5, 5);
    settextcolor(BLACK);
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(x - 120, y + 8, label);

    if (wcslen(content) > 0) {
        outtextxy(x + 10, y + 8, content);
    }

    if (isFocus) {
        int cursorX = x + 10 + textwidth(content);
        setlinecolor(RGB(0, 0, 0));
        line(cursorX, y + 10, cursorX, y + h - 10);
    }
}

<<<<<<< HEAD
// ========== 新增：不带标签的输入框函数 ==========
void drawInputBoxNoLabel(int x, int y, int w, int h, const wchar_t* content, bool isFocus) {
    setfillcolor(WHITE);
    setlinecolor(isFocus ? RGB(0, 120, 255) : RGB(180, 180, 180));
    fillroundrect(x, y, x + w, y + h, 5, 5);
    settextcolor(BLACK);
    settextstyle(18, 0, L"微软雅黑");

    if (wcslen(content) > 0) {
        outtextxy(x + 10, y + 8, content);
=======
// ==================== 登录验证（临时用写死的账号）====================
int loginCheck(const char* username, const char* password, Role role) {
    if (role == ROLE_ADMIN) {
        if (strcmp(username, ADMIN_USER) == 0 && strcmp(password, ADMIN_PWD) == 0)
            return 1;
>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6
    }

    if (isFocus) {
        int cursorX = x + 10 + textwidth(content);
        setlinecolor(RGB(0, 0, 0));
        line(cursorX, y + 10, cursorX, y + h - 10);
    }
}

<<<<<<< HEAD
int checkLogin(const wchar_t* username, const wchar_t* password, Role role) {
    if (role == ROLE_ADMIN) {
        return (wcscmp(username, ADMIN_USER) == 0 && wcscmp(password, ADMIN_PWD) == 0);
    }
    else {
        for (int i = 0; i < stuCount; i++) {
            if (wcscmp(username, stuAccounts[i].username) == 0 &&
                wcscmp(password, stuAccounts[i].password) == 0)
                return 1;
        }
        return 0;
    }
}

// ==================== 欢迎页 ====================
=======
// ==================== 页面1：欢迎页 ====================
>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6
PageType showWelcomePage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();
    settextcolor(RGB(30, 60, 120));
    settextstyle(36, 0, L"微软雅黑");
    const wchar_t* title = L"校园活动管理系统";
    outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 80, title);

    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int studentBtnX = centerX - BTN_WIDTH - BTN_GAP / 2;
    int adminBtnX = centerX + BTN_GAP / 2;
    int btnY = centerY - BTN_HEIGHT / 2;

    ExMessage msg;
    int lastMx = -1, lastMy = -1;
    bool studentHover = false, adminHover = false;

    while (true) {
<<<<<<< HEAD
        while (peekmessage(&msg)) {
=======
        // 处理所有鼠标消息，更新坐标并检测点击
        while (peekmessage(&msg, EM_MOUSE)) {
>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6
            if (msg.message == WM_MOUSEMOVE) {
                if (msg.x != lastMx || msg.y != lastMy) {
                    lastMx = msg.x;
                    lastMy = msg.y;
                    bool newStudentHover = (msg.x >= studentBtnX && msg.x <= studentBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                    bool newAdminHover = (msg.x >= adminBtnX && msg.x <= adminBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                    if (newStudentHover != studentHover || newAdminHover != adminHover) {
                        studentHover = newStudentHover;
                        adminHover = newAdminHover;
                        cleardevice();
                        settextcolor(RGB(30, 60, 120));
                        settextstyle(36, 0, L"微软雅黑");
                        outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 80, title);
                        drawButton(studentBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"学生登录", studentHover);
                        drawButton(adminBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"管理员登录", adminHover);
                    }
                }
            }
            else if (msg.message == WM_LBUTTONDOWN) {
<<<<<<< HEAD
                bool studentClick = (msg.x >= studentBtnX && msg.x <= studentBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                bool adminClick = (msg.x >= adminBtnX && msg.x <= adminBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                if (studentClick) return PAGE_REGISTER;
                if (adminClick) return PAGE_LOGIN;
=======
                bool studentHover = (msg.x >= studentBtnX && msg.x <= studentBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                bool adminHover = (msg.x >= adminBtnX && msg.x <= adminBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                if (studentHover) return PAGE_REGISTER;
                if (adminHover) return PAGE_LOGIN;
>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6
            }
        }
        Sleep(10);
    }
}

// ==================== 登录页 ====================
PageType showLoginPage(Role& currentRole) {
    // ... 登录页代码保持不变
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    int inputX = WINDOW_WIDTH / 2 + 50;
    int userInputY = 200;
    int pwdInputY = 280;
    int btnY = 400;
    int loginBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH - 20;
    int registerBtnX = WINDOW_WIDTH / 2 + 20;
    int backBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH / 2;
    int backBtnY = 520;

    wchar_t username[50] = { 0 };
    wchar_t password[50] = { 0 };
    int focusIndex = 0;

    ExMessage msg;
    int lastMx = -1, lastMy = -1;
    bool needRedraw = true;
    bool loginHover = false, registerHover = false, backHover = false;

    while (true) {
<<<<<<< HEAD
        while (peekmessage(&msg)) {
            if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
                if (msg.message == WM_MOUSEMOVE) {
                    if (msg.x != lastMx || msg.y != lastMy) {
                        lastMx = msg.x;
                        lastMy = msg.y;
                        bool newLoginHover = (msg.x >= loginBtnX && msg.x <= loginBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                        bool newRegisterHover = (currentRole == ROLE_STUDENT) && (msg.x >= registerBtnX && msg.x <= registerBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                        bool newBackHover = (msg.x >= backBtnX && msg.x <= backBtnX + BTN_WIDTH && msg.y >= backBtnY && msg.y <= backBtnY + BTN_HEIGHT);
                        if (newLoginHover != loginHover || newRegisterHover != registerHover || newBackHover != backHover) {
                            loginHover = newLoginHover;
                            registerHover = newRegisterHover;
                            backHover = newBackHover;
                            needRedraw = true;
                        }
=======
        // 处理鼠标消息，更新坐标
        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_MOUSEMOVE) {
                mx = msg.x;
                my = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                // 先判断输入框点击
                if (msg.x >= inputX && msg.x <= inputX + INPUT_WIDTH) {
                    if (msg.y >= userInputY && msg.y <= userInputY + INPUT_HEIGHT)
                        focusIndex = 0;
                    else if (msg.y >= pwdInputY && msg.y <= pwdInputY + INPUT_HEIGHT)
                        focusIndex = 1;
                }
                // 判断按钮点击
                bool loginHover = (msg.x >= loginBtnX && msg.x <= loginBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                bool registerHover = (currentRole == ROLE_STUDENT) && (msg.x >= registerBtnX && msg.x <= registerBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                bool backHover = (msg.x >= backBtnX && msg.x <= backBtnX + BTN_WIDTH && msg.y >= backBtnY && msg.y <= backBtnY + BTN_HEIGHT);

                if (loginHover) {
                    if (loginCheck(username, password, currentRole)) {
                        return (currentRole == ROLE_STUDENT) ? PAGE_STUDENT_MAIN : PAGE_ADMIN_MAIN;
                    }
                    else {
                        MessageBoxW(GetHWnd(), L"用户名或密码错误", L"登录失败", MB_OK | MB_ICONERROR);
                        memset(username, 0, sizeof(username));
                        memset(password, 0, sizeof(password));
                        focusIndex = 0;
>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6
                    }
                }
                else if (msg.message == WM_LBUTTONDOWN) {
                    if (msg.x >= inputX && msg.x <= inputX + INPUT_WIDTH) {
                        if (msg.y >= userInputY && msg.y <= userInputY + INPUT_HEIGHT) {
                            if (focusIndex != 0) {
                                focusIndex = 0;
                                needRedraw = true;
                            }
                        }
                        else if (msg.y >= pwdInputY && msg.y <= pwdInputY + INPUT_HEIGHT) {
                            if (focusIndex != 1) {
                                focusIndex = 1;
                                needRedraw = true;
                            }
                        }
                    }
                    if (msg.x >= loginBtnX && msg.x <= loginBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT) {
                        if (checkLogin(username, password, currentRole)) {
                            return (currentRole == ROLE_STUDENT) ? PAGE_STUDENT_MAIN : PAGE_ADMIN_MAIN;
                        }
                        else {
                            MessageBoxW(GetHWnd(), L"用户名或密码错误", L"登录失败", MB_OK);
                            memset(username, 0, sizeof(username));
                            memset(password, 0, sizeof(password));
                            focusIndex = 0;
                            needRedraw = true;
                        }
                    }
                    else if (currentRole == ROLE_STUDENT && msg.x >= registerBtnX && msg.x <= registerBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT) {
                        return PAGE_REGISTER;
                    }
                    else if (msg.x >= backBtnX && msg.x <= backBtnX + BTN_WIDTH && msg.y >= backBtnY && msg.y <= backBtnY + BTN_HEIGHT) {
                        return PAGE_WELCOME;
                    }
                }
            }
            else if (msg.message == WM_CHAR) {
                wchar_t* target = (focusIndex == 0) ? username : password;
                int maxLen = 49;

                if (msg.ch == VK_BACK) {
                    int len = wcslen(target);
                    if (len > 0) {
                        target[len - 1] = L'\0';
                        needRedraw = true;
                    }
                }
                else if (msg.ch != 0 && msg.ch != VK_RETURN && msg.ch != VK_ESCAPE) {
                    if (wcslen(target) < maxLen) {
                        target[wcslen(target)] = (wchar_t)msg.ch;
                        needRedraw = true;
                    }
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_TAB) {
                focusIndex = 1 - focusIndex;
                needRedraw = true;
            }
        }

        if (needRedraw) {
            cleardevice();

            const wchar_t* title = (currentRole == ROLE_STUDENT) ? L"学生登录" : L"管理员登录";
            settextcolor(RGB(30, 60, 120));
            settextstyle(30, 0, L"微软雅黑");
            outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 80, title);

            drawInputBox(inputX, userInputY, INPUT_WIDTH, INPUT_HEIGHT, L"用户名：", username, focusIndex == 0);
            drawInputBox(inputX, pwdInputY, INPUT_WIDTH, INPUT_HEIGHT, L"密  码：", password, focusIndex == 1);

            drawButton(loginBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"登录", loginHover);
            if (currentRole == ROLE_STUDENT) {
                drawButton(registerBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"注册", registerHover);
            }
            drawButton(backBtnX, backBtnY, BTN_WIDTH, BTN_HEIGHT, L"返回", backHover);

            needRedraw = false;
        }
        Sleep(10);
    }
}

// ==================== 注册页 ==================
PageType showRegisterPage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    // 布局参数
    int labelX = 200;              // 标签起始X
    int inputX = labelX + 80;     // 输入框起始X
    int inputWidth = 300;
    int rowHeight = 48;
    int startY = 100;

    // 第一列
    int col1LabelX = labelX;
    int col1InputX = inputX;

    // 第二列
    int col2LabelX = labelX + 420;
    int col2InputX = col2LabelX + 80;

    // Y坐标
    int userInputY = startY;
    int pwdInputY = startY + rowHeight;
    int idInputY = startY + rowHeight * 2;
    int nameInputY = startY + rowHeight * 3;
    int phoneInputY = startY + rowHeight * 4;

    int genderInputY = startY;
    int ageInputY = startY + rowHeight;
    int clnameInputY = startY + rowHeight * 2;
    int locationInputY = startY + rowHeight * 3;

    int btnY = startY + rowHeight * 5 + 20;
    int registerBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH - 20;
    int backBtnX = WINDOW_WIDTH / 2 + 20;

    wchar_t username[50] = { 0 };
    wchar_t password[50] = { 0 };
    wchar_t studentId[20] = { 0 };
    wchar_t name[20] = { 0 };
    wchar_t phone[20] = { 0 };
    wchar_t gender[10] = { 0 };
    wchar_t clname[30] = { 0 };
    wchar_t location[100] = { 0 };
    int age = 0;
    wchar_t ageStr[10] = { 0 };
    int focusIndex = 0;

    ExMessage msg;
    int lastMx = -1, lastMy = -1;
    bool needRedraw = true;
    bool registerHover = false, backHover = false;

    while (true) {
        while (peekmessage(&msg)) {
            if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
                if (msg.message == WM_MOUSEMOVE) {
                    if (msg.x != lastMx || msg.y != lastMy) {
                        lastMx = msg.x;
                        lastMy = msg.y;
                        bool newRegHover = (msg.x >= registerBtnX && msg.x <= registerBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                        bool newBackHover = (msg.x >= backBtnX && msg.x <= backBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                        if (newRegHover != registerHover || newBackHover != backHover) {
                            registerHover = newRegHover;
                            backHover = newBackHover;
                            needRedraw = true;
                        }
                    }
                }
                else if (msg.message == WM_LBUTTONDOWN) {
                    // 第一列输入框
                    if (msg.x >= col1InputX && msg.x <= col1InputX + inputWidth) {
                        if (msg.y >= userInputY && msg.y <= userInputY + INPUT_HEIGHT) focusIndex = 0;
                        else if (msg.y >= pwdInputY && msg.y <= pwdInputY + INPUT_HEIGHT) focusIndex = 1;
                        else if (msg.y >= idInputY && msg.y <= idInputY + INPUT_HEIGHT) focusIndex = 2;
                        else if (msg.y >= nameInputY && msg.y <= nameInputY + INPUT_HEIGHT) focusIndex = 3;
                        else if (msg.y >= phoneInputY && msg.y <= phoneInputY + INPUT_HEIGHT) focusIndex = 4;
                        needRedraw = true;
                    }
                    // 第二列输入框
                    else if (msg.x >= col2InputX && msg.x <= col2InputX + inputWidth) {
                        if (msg.y >= genderInputY && msg.y <= genderInputY + INPUT_HEIGHT) focusIndex = 5;
                        else if (msg.y >= ageInputY && msg.y <= ageInputY + INPUT_HEIGHT) focusIndex = 6;
                        else if (msg.y >= clnameInputY && msg.y <= clnameInputY + INPUT_HEIGHT) focusIndex = 7;
                        else if (msg.y >= locationInputY && msg.y <= locationInputY + INPUT_HEIGHT) focusIndex = 8;
                        needRedraw = true;
                    }
                    // 按钮
                    if (msg.x >= registerBtnX && msg.x <= registerBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT) {
                        char usernameA[50], passwordA[50], nameA[20], studentIdA[20], phoneA[20], genderA[10], clnameA[30], locationA[100];
                        WideCharToMultiByte(CP_ACP, 0, username, -1, usernameA, 50, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, password, -1, passwordA, 50, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, name, -1, nameA, 20, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, studentId, -1, studentIdA, 20, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, phone, -1, phoneA, 20, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, gender, -1, genderA, 10, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, clname, -1, clnameA, 30, NULL, NULL);
                        WideCharToMultiByte(CP_ACP, 0, location, -1, locationA, 100, NULL, NULL);

                        if (strlen(usernameA) == 0 || strlen(passwordA) == 0 || strlen(studentIdA) == 0 || strlen(nameA) == 0) {
                            MessageBoxW(GetHWnd(), L"请填写完整信息！", L"注册失败", MB_OK);
                        }
                        else {
                            int regResult = register_student(usernameA, passwordA, nameA, studentIdA, phoneA, genderA, age, clnameA, locationA);
                            if (regResult == 1) {
                                MessageBoxW(GetHWnd(), L"注册成功！请登录", L"成功", MB_OK);
                                return PAGE_LOGIN;
                            }
                            else {
                                MessageBoxW(GetHWnd(), L"注册失败，用户名可能已存在！", L"错误", MB_OK);
                            }
                        }
                        needRedraw = true;
                    }
                    else if (msg.x >= backBtnX && msg.x <= backBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT) {
                        return PAGE_LOGIN;
                    }
                }
            }
            else if (msg.message == WM_CHAR) {
                wchar_t* target = NULL;
                int maxLen = 0;
                switch (focusIndex) {
                case 0: target = username; maxLen = 49; break;
                case 1: target = password; maxLen = 49; break;
                case 2: target = studentId; maxLen = 19; break;
                case 3: target = name; maxLen = 19; break;
                case 4: target = phone; maxLen = 19; break;
                case 5: target = gender; maxLen = 9; break;
                case 6:
                    if (msg.ch == VK_BACK) {
                        int len = wcslen(ageStr);
                        if (len > 0) ageStr[len - 1] = L'\0';
                        if (wcslen(ageStr) > 0) age = _wtoi(ageStr);
                        else age = 0;
                        needRedraw = true;
                    }
                    else if (msg.ch >= L'0' && msg.ch <= L'9' && wcslen(ageStr) < 9) {
                        ageStr[wcslen(ageStr)] = msg.ch;
                        age = _wtoi(ageStr);
                        needRedraw = true;
                    }
                    target = NULL;
                    break;
                case 7: target = clname; maxLen = 29; break;
                case 8: target = location; maxLen = 99; break;
                }
                if (target) {
                    if (msg.ch == VK_BACK) {
                        int len = wcslen(target);
                        if (len > 0) {
                            target[len - 1] = L'\0';
                            needRedraw = true;
                        }
                    }
                    else if (msg.ch != 0 && msg.ch != VK_RETURN && msg.ch != VK_ESCAPE) {
                        if (wcslen(target) < maxLen) {
                            target[wcslen(target)] = msg.ch;
                            needRedraw = true;
                        }
                    }
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_TAB) {
                focusIndex = (focusIndex + 1) % 9;
                needRedraw = true;
            }
        }

        if (needRedraw) {
            cleardevice();

            const wchar_t* title = L"学生注册";
            settextcolor(RGB(30, 60, 120));
            settextstyle(28, 0, L"微软雅黑");
            outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 40, title);

            settextstyle(18, 0, L"微软雅黑");
            setbkmode(TRANSPARENT);

            // 第一列标签
            settextcolor(RGB(60, 60, 60));
            outtextxy(col1LabelX, userInputY + 8, L"用户名：");
            outtextxy(col1LabelX, pwdInputY + 8, L"密  码：");
            outtextxy(col1LabelX, idInputY + 8, L"学  号：");
            outtextxy(col1LabelX, nameInputY + 8, L"姓  名：");
            outtextxy(col1LabelX, phoneInputY + 8, L"联系方式：");

            // 第二列标签
            outtextxy(col2LabelX, genderInputY + 8, L"性  别：");
            outtextxy(col2LabelX, ageInputY + 8, L"年  龄：");
            outtextxy(col2LabelX, clnameInputY + 8, L"班  级：");
            outtextxy(col2LabelX, locationInputY + 8, L"位  置：");

            // 第一列输入框
            drawInputBoxNoLabel(col1InputX, userInputY, inputWidth, INPUT_HEIGHT, username, focusIndex == 0);
            drawInputBoxNoLabel(col1InputX, pwdInputY, inputWidth, INPUT_HEIGHT, password, focusIndex == 1);
            drawInputBoxNoLabel(col1InputX, idInputY, inputWidth, INPUT_HEIGHT, studentId, focusIndex == 2);
            drawInputBoxNoLabel(col1InputX, nameInputY, inputWidth, INPUT_HEIGHT, name, focusIndex == 3);
            drawInputBoxNoLabel(col1InputX, phoneInputY, inputWidth, INPUT_HEIGHT, phone, focusIndex == 4);

            // 第二列输入框
            drawInputBoxNoLabel(col2InputX, genderInputY, inputWidth, INPUT_HEIGHT, gender, focusIndex == 5);
            drawInputBoxNoLabel(col2InputX, ageInputY, inputWidth, INPUT_HEIGHT, ageStr, focusIndex == 6);
            drawInputBoxNoLabel(col2InputX, clnameInputY, inputWidth, INPUT_HEIGHT, clname, focusIndex == 7);
            drawInputBoxNoLabel(col2InputX, locationInputY, inputWidth, INPUT_HEIGHT, location, focusIndex == 8);

            // 按钮
            settextstyle(20, 0, L"微软雅黑");
            drawButton(registerBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"注册", registerHover);
            drawButton(backBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"返回登录", backHover);

            needRedraw = false;
        }
        Sleep(10);
    }
}

<<<<<<< HEAD
// 临时占位函数
void showStudentMainPage() {}
void showAdminMainPage() {}

// ==================== 主函数 ===================
=======
// ==================== 主函数 ====================
void showStudentMainPage() {}
void showAdminMainPage() {}

>>>>>>> 99f9f257c9498487e20e1e118ab329e8078133e6
int main() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    init_system();

    PageType currentPage = PAGE_WELCOME;
    Role currentRole = ROLE_STUDENT;

    while (true) {
        switch (currentPage) {
        case PAGE_WELCOME:
            currentPage = showWelcomePage();
            break;
        case PAGE_LOGIN:
            currentPage = showLoginPage(currentRole);
            break;
        case PAGE_REGISTER:
            currentPage = showRegisterPage();
            break;
        case PAGE_STUDENT_MAIN:
            showStudentMainPage();
            currentPage = PAGE_LOGIN;
            break;
        case PAGE_ADMIN_MAIN:
            showAdminMainPage();
            currentPage = PAGE_LOGIN;
            break;
        }
    }

    closegraph();
    return 0;
}