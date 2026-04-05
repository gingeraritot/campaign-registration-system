#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <easyx.h>
#include <windows.h>
#include "head.h"

// ==================== 全局常量定义 ====================
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define BTN_WIDTH     200
#define BTN_HEIGHT    50
#define BTN_GAP       80
#define INPUT_WIDTH   300
#define INPUT_HEIGHT  40

// ==================== 演示用账号密码 ====================
#define ADMIN_USER    "admin"
#define ADMIN_PWD     "123456"

typedef struct {
    char username[20];
    char password[20];
} StudentAccount;

StudentAccount stuAccounts[] = {
    {"jiang", "123"},
    {"yang", "456"},
    {"chen", "111"},
    {"guo", "222"}
};
int stuCount = sizeof(stuAccounts) / sizeof(StudentAccount);

// ==================== 全局状态定义 ====================
typedef enum {
    PAGE_WELCOME,
    PAGE_LOGIN,
    PAGE_REGISTER,
    PAGE_STUDENT_MAIN,
    PAGE_ADMIN_MAIN
} PageType;

// =================== 工具函数：绘制通用按钮 ===================
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

// ==================== 工具函数：绘制通用输入框 ====================
void drawInputBox(int x, int y, int w, int h, const wchar_t* label, const char* content, bool isFocus) {
    setfillcolor(WHITE);
    setlinecolor(isFocus ? RGB(0, 120, 255) : RGB(180, 180, 180));
    fillroundrect(x, y, x + w, y + h, 5, 5);
    settextcolor(BLACK);
    settextstyle(18, 0, L"微软雅黑");
    outtextxy(x - 120, y + 8, label);

    wchar_t contentW[100] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, content, -1, contentW, 100);
    if (strlen(content) > 0) {
        outtextxy(x + 10, y + 8, contentW);
    }
    if (isFocus) {
        int cursorX = x + 10 + textwidth(contentW);
        setlinecolor(BLACK);
        line(cursorX, y + 10, cursorX, y + h - 10);
    }
}

// ==================== 登录验证（临时用写死的账号）====================
int loginCheck(const char* username, const char* password, Role role) {
    if (role == ROLE_ADMIN) {
        if (strcmp(username, ADMIN_USER) == 0 && strcmp(password, ADMIN_PWD) == 0)
            return 1;
    }
    else if (role == ROLE_STUDENT) {
        for (int i = 0; i < stuCount; i++) {
            if (strcmp(username, stuAccounts[i].username) == 0 &&
                strcmp(password, stuAccounts[i].password) == 0)
                return 1;
        }
    }
    return 0;
}

// ==================== 页面1：欢迎页 ====================
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
    int mx = 0, my = 0;

    while (true) {
        // 处理所有鼠标消息，更新坐标并检测点击
        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_MOUSEMOVE) {
                mx = msg.x;
                my = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                bool studentHover = (msg.x >= studentBtnX && msg.x <= studentBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                bool adminHover = (msg.x >= adminBtnX && msg.x <= adminBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                if (studentHover) return PAGE_REGISTER;
                if (adminHover) return PAGE_LOGIN;
            }
        }

        bool studentHover = (mx >= studentBtnX && mx <= studentBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool adminHover = (mx >= adminBtnX && mx <= adminBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        drawButton(studentBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"学生登录", studentHover);
        drawButton(adminBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"管理员登录", adminHover);

        Sleep(10);
    }
}

// ==================== 页面2：登录页 ====================
PageType showLoginPage(Role& currentRole) {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    settextcolor(RGB(30, 60, 120));
    settextstyle(30, 0, L"微软雅黑");
    const wchar_t* title = (currentRole == ROLE_STUDENT) ? L"学生登录" : L"管理员登录";
    outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 60, title);

    int inputX = WINDOW_WIDTH / 2 + 50;
    int userInputY = 180;
    int pwdInputY = 260;
    int btnY = 380;
    char username[50] = { 0 };
    char password[50] = { 0 };
    int focusIndex = 0;

    int loginBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH - 20;
    int registerBtnX = WINDOW_WIDTH / 2 + 20;
    int backBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH / 2;
    int backBtnY = 480;

    ExMessage msg;
    int mx = 0, my = 0;

    while (true) {
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
                    }
                }
                else if (registerHover && currentRole == ROLE_STUDENT) {
                    return PAGE_REGISTER;
                }
                else if (backHover) {
                    return PAGE_WELCOME;
                }
            }
        }

        // 处理键盘输入
        while (peekmessage(&msg, EM_KEY)) {
            if (msg.message == WM_CHAR) {
                if (focusIndex == 0 && strlen(username) < 49) {
                    if (msg.ch == VK_BACK) {
                        if (strlen(username) > 0) username[strlen(username) - 1] = '\0';
                    }
                    else {
                        username[strlen(username)] = msg.ch;
                    }
                }
                else if (focusIndex == 1 && strlen(password) < 49) {
                    if (msg.ch == VK_BACK) {
                        if (strlen(password) > 0) password[strlen(password) - 1] = '\0';
                    }
                    else {
                        password[strlen(password)] = msg.ch;
                    }
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_TAB) {
                focusIndex = 1 - focusIndex;
            }
        }

        // 绘制界面
        drawInputBox(inputX, userInputY, INPUT_WIDTH, INPUT_HEIGHT, L"用户名：", username, focusIndex == 0);
        drawInputBox(inputX, pwdInputY, INPUT_WIDTH, INPUT_HEIGHT, L"密  码：", password, focusIndex == 1);

        bool loginHover = (mx >= loginBtnX && mx <= loginBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool registerHover = (currentRole == ROLE_STUDENT) && (mx >= registerBtnX && mx <= registerBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool backHover = (mx >= backBtnX && mx <= backBtnX + BTN_WIDTH && my >= backBtnY && my <= backBtnY + BTN_HEIGHT);

        drawButton(loginBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"登录", loginHover);
        if (currentRole == ROLE_STUDENT) {
            drawButton(registerBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"注册", registerHover);
        }
        drawButton(backBtnX, backBtnY, BTN_WIDTH, BTN_HEIGHT, L"返回", backHover);

        Sleep(10);
    }
}

// ==================== 页面3：注册页 ====================
PageType showRegisterPage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    settextcolor(RGB(30, 60, 120));
    settextstyle(30, 0, L"微软雅黑");
    const wchar_t* title = L"学生注册";
    outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 40, title);

    int inputX = WINDOW_WIDTH / 2 + 50;
    int userInputY = 120;
    int pwdInputY = 190;
    int idInputY = 260;
    int nameInputY = 330;
    int phoneInputY = 400;
    int genderInputY = 470;
    int ageInputY = 540;
    int clnameInputY = 610;
    int locationInputY = 680;
    int btnY = 420;

    char username[50] = { 0 };
    char password[50] = { 0 };
    char studentId[20] = { 0 };
    char name[20] = { 0 };
    char phone[20] = { 0 };
    char gender[10] = { 0 };
    char clname[30] = { 0 };
    char location[100] = { 0 };
    int age = 0;
    char ageStr[10] = { 0 };
    int focusIndex = 0;

    int registerBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH - 20;
    int backBtnX = WINDOW_WIDTH / 2 + 20;

    ExMessage msg;
    int mx = 0, my = 0;

    while (true) {
        // 处理鼠标消息
        while (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_MOUSEMOVE) {
                mx = msg.x;
                my = msg.y;
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                // 点击输入框切换焦点
                if (msg.x >= inputX && msg.x <= inputX + INPUT_WIDTH) {
                    if (msg.y >= userInputY && msg.y <= userInputY + INPUT_HEIGHT) focusIndex = 0;
                    else if (msg.y >= pwdInputY && msg.y <= pwdInputY + INPUT_HEIGHT) focusIndex = 1;
                    else if (msg.y >= idInputY && msg.y <= idInputY + INPUT_HEIGHT) focusIndex = 2;
                    else if (msg.y >= nameInputY && msg.y <= nameInputY + INPUT_HEIGHT) focusIndex = 3;
                    else if (msg.y >= phoneInputY && msg.y <= phoneInputY + INPUT_HEIGHT) focusIndex = 4;
                    else if (msg.y >= genderInputY && msg.y <= genderInputY + INPUT_HEIGHT) focusIndex = 5;
                    else if (msg.y >= ageInputY && msg.y <= ageInputY + INPUT_HEIGHT) focusIndex = 6;
                    else if (msg.y >= clnameInputY && msg.y <= clnameInputY + INPUT_HEIGHT) focusIndex = 7;
                    else if (msg.y >= locationInputY && msg.y <= locationInputY + INPUT_HEIGHT) focusIndex = 8;
                }
                // 按钮点击
                bool registerHover = (msg.x >= registerBtnX && msg.x <= registerBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                bool backHover = (msg.x >= backBtnX && msg.x <= backBtnX + BTN_WIDTH && msg.y >= btnY && msg.y <= btnY + BTN_HEIGHT);
                if (registerHover) {
                    if (strlen(username) == 0 || strlen(password) == 0 || strlen(studentId) == 0 || strlen(name) == 0) {
                        MessageBoxW(GetHWnd(), L"请填写完整信息！", L"注册失败", MB_OK | MB_ICONWARNING);
                        continue;
                    }
                    int regResult = register_student(username, password, name, studentId, phone, gender, age, clname, location);
                    if (regResult > 0) {
                        MessageBoxW(GetHWnd(), L"注册成功！请登录", L"成功", MB_OK | MB_ICONINFORMATION);
                        return PAGE_LOGIN;
                    }
                    else {
                        MessageBoxW(GetHWnd(), L"注册失败，用户名已存在！", L"错误", MB_OK | MB_ICONERROR);
                    }
                }
                else if (backHover) {
                    return PAGE_LOGIN;
                }
            }
        }

        // 处理键盘输入
        while (peekmessage(&msg, EM_KEY)) {
            if (msg.message == WM_CHAR) {
                char* target = NULL;
                int maxLen = 0;
                switch (focusIndex) {
                case 0: target = username; maxLen = 49; break;
                case 1: target = password; maxLen = 49; break;
                case 2: target = studentId; maxLen = 19; break;
                case 3: target = name; maxLen = 19; break;
                case 4: target = phone; maxLen = 19; break;
                case 5: target = gender; maxLen = 9; break;
                case 6: break; // age 单独处理
                case 7: target = clname; maxLen = 29; break;
                case 8: target = location; maxLen = 99; break;
                }
                if (focusIndex == 6) {
                    // 年龄输入
                    if (msg.ch == VK_BACK) {
                        if (age > 0) age = 0;
                    }
                    else if (msg.ch >= '0' && msg.ch <= '9') {
                        age = age * 10 + (msg.ch - '0');
                        if (age > 150) age = 150;
                    }
                }
                else if (target != NULL) {
                    if (msg.ch == VK_BACK) {
                        if (strlen(target) > 0) target[strlen(target) - 1] = '\0';
                    }
                    else if (strlen(target) < maxLen) {
                        target[strlen(target)] = msg.ch;
                    }
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.vkcode == VK_TAB) {
                focusIndex = (focusIndex + 1) % 9;
            }
        }

        // 绘制界面
        drawInputBox(inputX, userInputY, INPUT_WIDTH, INPUT_HEIGHT, L"用户名：", username, focusIndex == 0);
        drawInputBox(inputX, pwdInputY, INPUT_WIDTH, INPUT_HEIGHT, L"密  码：", password, focusIndex == 1);
        drawInputBox(inputX, idInputY, INPUT_WIDTH, INPUT_HEIGHT, L"学  号：", studentId, focusIndex == 2);
        drawInputBox(inputX, nameInputY, INPUT_WIDTH, INPUT_HEIGHT, L"姓  名：", name, focusIndex == 3);
        drawInputBox(inputX, phoneInputY, INPUT_WIDTH, INPUT_HEIGHT, L"联系方式：", phone, focusIndex == 4);
        drawInputBox(inputX, genderInputY, INPUT_WIDTH, INPUT_HEIGHT, L"性  别：", gender, focusIndex == 5);
        sprintf(ageStr, "%d", age);
        drawInputBox(inputX, ageInputY, INPUT_WIDTH, INPUT_HEIGHT, L"年  龄：", ageStr, focusIndex == 6);
        drawInputBox(inputX, clnameInputY, INPUT_WIDTH, INPUT_HEIGHT, L"班  级：", clname, focusIndex == 7);
        drawInputBox(inputX, locationInputY, INPUT_WIDTH, INPUT_HEIGHT, L"位  置：", location, focusIndex == 8);

        bool registerHover = (mx >= registerBtnX && mx <= registerBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool backHover = (mx >= backBtnX && mx <= backBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        drawButton(registerBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"注册", registerHover);
        drawButton(backBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"返回登录", backHover);

        Sleep(10);
    }
}

// ==================== 主函数 ====================
void showStudentMainPage() {}
void showAdminMainPage() {}

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
            // 根据点击位置决定角色（简单处理：欢迎页只有学生和管理员入口）
            // 这里从 welcome 返回时，我们需要知道用户点击的是学生还是管理员。
            // 简单做法：在 welcome 中直接返回对应的目标页面（PAGE_REGISTER 或 PAGE_LOGIN），
            // 并设置 currentRole。但为兼容原逻辑，我们在返回后判断。
            // 由于 welcome 已经返回具体页面，我们不再根据鼠标位置设置角色，而是在登录页手动选择。
            // 为了保持一致性，我们可以在 welcome 中返回 PAGE_LOGIN 时，让调用者设置 currentRole。
            // 简化：去掉 welcome 中的角色判断，登录页自己处理角色切换。
            // 但为了支持管理员登录，我们可以在登录页增加一个角色切换按钮。
            // 这里先简单处理：如果 currentPage == PAGE_LOGIN，说明是管理员登录；如果 currentPage == PAGE_REGISTER，说明是学生注册。
            if (currentPage == PAGE_LOGIN) {
                currentRole = ROLE_ADMIN;
            }
            else if (currentPage == PAGE_REGISTER) {
                currentRole = ROLE_STUDENT;
            }
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