#include <graphics.h>  //easyx图形库的核心头文件
#include <conio.h>  //键盘输入
#include <stdio.h>
#include <string.h>
#include <easyx.h>  //对graphics.h的封装和扩展，包含了更多功能
#include <windows.h> // 必须包含，为了 MessageBoxW
#include "head.h"

int mousex();  //获取鼠标坐标
int mousey();

// ==================== 全局常量定义 ===================
#define WINDOW_WIDTH  800  //窗口宽度
#define WINDOW_HEIGHT 600  //窗口高度
#define BTN_WIDTH     200  //按钮宽度
#define BTN_HEIGHT    50   //按钮按钮高度
#define BTN_GAP       80   //按钮之间的间距
#define INPUT_WIDTH   300  //输入框宽度
#define INPUT_HEIGHT  40   //输入框高度


// ==================== 演示用账号密码 ===================
// 管理员账号
#define ADMIN_USER    "admin"
#define ADMIN_PWD     "123456"

// 学生账号（自己随便加）
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

// 学生账号数量
int stuCount = sizeof(stuAccounts) / sizeof(StudentAccount);

// ==================== 全局状态定义 ====================
//不同的页面状态
typedef enum {
    PAGE_WELCOME,  //欢迎页
    PAGE_LOGIN,    //登录页
    PAGE_REGISTER, //注册页
    PAGE_STUDENT_MAIN,  //学生界面（登录之后）
    PAGE_ADMIN_MAIN     //管理员界面（登录之后）
} PageType;


// =================== 工具函数：绘制通用按钮 ===================
void drawButton(int x, int y, int w, int h, const wchar_t* text, bool isHover) {
    COLORREF btnColor = isHover ? RGB(100, 180, 255) : RGB(70, 150, 230);  //“选颜色”给按钮颜色赋值，当鼠标悬停（ishover为真时），按钮变成浅蓝色，没悬停即为深蓝色
    setfillcolor(btnColor);//“刷颜色”设置按钮内部填充颜色
    setlinecolor(RGB(30, 100, 180));//设置按钮边框的颜色，更深的蓝色区分边框与内部
    fillroundrect(x, y, x + w, y + h, 10, 10);//画出带圆角的按钮矩形，前四个参数是按钮的位置和宽高，最后两个10是圆角的弧度

    settextcolor(WHITE);//把按钮上的文字颜色设为白色
    setbkmode(TRANSPARENT);//让文字背景透明，不会挡住按钮底色
    settextstyle(20, 0, L"微软雅黑"); // 设置字体的大小，样式和类型
    int textX = x + (w - textwidth(text)) / 2;//计算文字在按钮里的坐标，让文字在按钮里水平和垂直居中
    int textY = y + (h - textheight(text)) / 2;
    outtextxy(textX, textY, text);//让文字画到按钮的居中位置上
}

// ==================== 工具函数：绘制通用输入框 ====================
void drawInputBox(int x, int y, int w, int h, const wchar_t* label, const char* content, bool isFocus) {
    setfillcolor(WHITE);//输入框内部填充成白色
    setlinecolor(isFocus ? RGB(0, 120, 255) : RGB(180, 180, 180));//设置边框颜色，输入框被选中是亮蓝色，没选中是浅灰色
    fillroundrect(x, y, x + w, y + h, 5, 5);//画出带圆角的输入框外形

    settextcolor(BLACK);//设置输入框内文字为黑色
    settextstyle(18, 0, L"微软雅黑"); //设置字体的大小，样式和类型
    outtextxy(x - 120, y + 8, label);//在输入框左边画出说明文字（账号/密码）

    // 将窄字符 content 转为宽字符，char对中文字符处理不好，咱们如果全用英文做账号用户名的话可以去掉，不过加上也不影响
    wchar_t contentW[100] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, content, -1, contentW, 100); // 编码改为 CP_UTF8
    if (strlen(content) > 0) {
        outtextxy(x + 10, y + 8, contentW);//把转换后的宽字符显示在输入框里
    }
    //当输入框被选中时，在框内出现闪烁的光标
    if (isFocus) {
        int cursorX = x + 10 + textwidth(contentW);//计算光标的X坐标，再输入框的末尾显示
        setlinecolor(BLACK);//设置光标为黑色
        line(cursorX, y + 10, cursorX, y + h - 10);//画一条短线作为光标，位置在输入框内偏下的位置
    }
}

// ==================== 同学A/B/C函数预留接口 ====================
int login(const char* username, const char* password, Role role) {
    printf("尝试登录：角色=%d, 用户名=%s, 密码=%s\n", role, username, password);

    // 管理员登录
    if (role == ROLE_ADMIN) {
        if (strcmp(username, ADMIN_USER) == 0 && strcmp(password, ADMIN_PWD) == 0) {
            return 1;   // 登录成功
        }
    }

    // 学生登录（遍历我们写死的账号）
    else if (role == ROLE_STUDENT) {
        for (int i = 0; i < stuCount; i++) {
            if (strcmp(username, stuAccounts[i].username) == 0 &&
                strcmp(password, stuAccounts[i].password) == 0) {
                return 1;   // 找到匹配，登录成功
            }
        }
    }

    // 都不对
    return 0;
}

// ==================== 页面1：欢迎页 ====================
PageType showWelcomePage() {
    cleardevice();//清空绘图区域，画欢迎页
    setbkcolor(RGB(240, 245, 250));//设置背景颜色，淡蓝色（只是设置没有改变）
    cleardevice();//真正显示新的背景色

    settextcolor(RGB(30, 60, 120));//设置字体颜色：深青色
    settextstyle(36, 0, L"微软雅黑"); 
    const wchar_t* title = L"校园活动管理系统"; // 定义标题文本
    outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 80, title);//把标题画在界面上方中间

    int centerX = WINDOW_WIDTH / 2;//页面中心点坐标
    int centerY = WINDOW_HEIGHT / 2;
    int studentBtnX = centerX - BTN_WIDTH - BTN_GAP / 2;//登录按钮坐标
    int adminBtnX = centerX + BTN_GAP / 2;
    int btnY = centerY - BTN_HEIGHT / 2;//共同纵坐标，让学生按钮和管理员按钮在同一水平线上显示

    ExMessage msg;//监听循环
    while (true) {
        //获取当前鼠标的坐标
        int mx = mousex();
        int my = mousey();
        //判断鼠标是否悬停在对应按钮上，用来实现高亮效果
        bool studentHover = (mx >= studentBtnX && mx <= studentBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool adminHover = (mx >= adminBtnX && mx <= adminBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        //绘制两个按钮
        drawButton(studentBtnX,btnY,BTN_WIDTH,BTN_HEIGHT,L"xueshengdenglu",studentHover);  
        drawButton(adminBtnX,btnY,BTN_WIDTH,BTN_HEIGHT,L"guanliyuandenglu",adminHover);  
        //监听鼠标点击事件，如果点击了按钮就跳转到登录页面
        if (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                if (studentHover) {
                    return PAGE_REGISTER;
                }
                if (adminHover) {
                    return PAGE_LOGIN;
                }
            }
        }
        Sleep(10);//让程序暂停10ms再继续运行，避免过度循环降低效率
    }
}

// ==================页面3：登录页 =================
PageType showLoginPage(Role& currentRole) {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));//淡蓝色
    cleardevice();

    settextcolor(RGB(30, 60, 120));//文字：深青色
    settextstyle(30, 0, L"微软雅黑");
    const wchar_t* title = (currentRole == ROLE_STUDENT) ? L"Student Registration" : L"Administrator Login"; // 动态生成标题
    outtextxy((WINDOW_WIDTH - textwidth(title)) / 2, 60, title);//标题文本画到页面上方中间，与欢迎页一致
    //定义输入框的位置
    int inputX = WINDOW_WIDTH / 2 + 50;
    int userInputY = 180;
    int pwdInputY = 260;
    int btnY = 380;
    //定义存储用户名，密码的字符数组和输入焦点变量
    char username[50] = { 0 };
    char password[50] = { 0 };
    int focusIndex = 0;
    bool isInputting = true;//标记当前是否处于输入状态
    //计算登录，注册，返回按钮的水平坐标，让他们在页面中水平居中分布
    int loginBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH - 20;
    int registerBtnX = WINDOW_WIDTH / 2 + 20;
    int backBtnX = WINDOW_WIDTH / 2 - BTN_WIDTH / 2;
    int backBtnY = 480;

    ExMessage msg;
    while (isInputting) {
        int mx = mousex();
        int my = mousey();
        //画两个输入框
        drawInputBox(inputX, userInputY, INPUT_WIDTH, INPUT_HEIGHT, L"用户名：", username, focusIndex == 0); // focusIndex用来标记当前哪个输入框处于激活状态，同时高亮边框
        drawInputBox(inputX, pwdInputY, INPUT_WIDTH, INPUT_HEIGHT, L"密  码：", password, focusIndex == 1); 
        //判断鼠标是否悬停在登录，注册，返回这三个按钮上，交互效果
        bool loginHover = (mx >= loginBtnX && mx <= loginBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool registerHover = (currentRole == ROLE_STUDENT) && (mx >= registerBtnX && mx <= registerBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool backHover = (mx >= backBtnX && mx <= backBtnX + BTN_WIDTH && my >= backBtnY && my <= backBtnY + BTN_HEIGHT);
        drawButton(loginBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"登录", loginHover); //绘制登录按钮
        if (currentRole == ROLE_STUDENT) {
            drawButton(registerBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"注册", registerHover); //判断如果是学生，就绘制注册按钮
        }
        drawButton(backBtnX, backBtnY, BTN_WIDTH, BTN_HEIGHT, L"返回", backHover); // 绘制返回按钮
        while (peekmessage(&msg)) {
            //处理键盘输入逻辑
            if (msg.message == WM_CHAR) {
                if (focusIndex == 0 && strlen(username) < 49) {//如果当前激活的是用户名输入框，就把按键字符写在username数组里
                    if (msg.ch == VK_BACK) {
                        if (strlen(username) > 0) username[strlen(username) - 1] = '\0';
                    }
                    else {
                        username[strlen(username)] = msg.ch;
                    }
                }
                else if (focusIndex == 1 && strlen(password) < 49) {//如果当前激活的是密码输入框，就把按键字符写在password数组里
                    if (msg.ch == VK_BACK) {
                        if (strlen(password) > 0) password[strlen(password) - 1] = '\0';
                    }
                    else {
                        password[strlen(password)] = msg.ch;//处理了退格键删除字符的功能
                    }
                }
            }
            //处理鼠标点击事件的逻辑
            if (msg.message == WM_LBUTTONDOWN) {//判断点击的是用户名还是密码输入框，来切换输入焦点（光标位置）
                if (mx >= inputX && mx <= inputX + INPUT_WIDTH && my >= userInputY && my <= userInputY + INPUT_HEIGHT) {
                    focusIndex = 0;
                }
                else if (mx >= inputX && mx <= inputX + INPUT_WIDTH && my >= pwdInputY && my <= pwdInputY + INPUT_HEIGHT) {
                    focusIndex = 1;
                }
                else if (loginHover) {//处理登录按钮
                    int loginResult = login(username, password, currentRole);//调用login函数验证账号密码，成功则跳转到对应角色的主页面，失败则弹窗提示
                    if (loginResult == 1) {
                        return (currentRole == ROLE_STUDENT) ? PAGE_STUDENT_MAIN : PAGE_ADMIN_MAIN;
                    }
                    else {
                        MessageBoxW(GetHWnd(),L"wrong",L"failed", MB_OK | MB_ICONERROR);
                        memset(username, 0, sizeof(username));
                        memset(password, 0, sizeof(password));
                        focusIndex = 0;
                    }
                }
                //点击注册或返回的按钮，跳转到相应界面
                else if (registerHover && currentRole == ROLE_STUDENT) {
                    return PAGE_REGISTER;
                }
                else if (backHover) {
                    return PAGE_WELCOME;
                }
            }
            //按tap建控制换行操作（不需要鼠标）
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_TAB) {
                focusIndex = 1 - focusIndex;
            }
        }
        Sleep(10);
    }
    return PAGE_WELCOME;
}

// =================== 页面2：注册页 ==================
//与欢迎页和登录页一模一样
PageType showRegisterPage() {
    cleardevice();
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    settextcolor(RGB(30, 60, 120));
    settextstyle(30, 0, L"微软雅黑"); 
    const wchar_t* title = L"学生注册"; // 改为宽字符 L""
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
    while (true) {
        int mx = mousex();
        int my = mousey();
        //绘制输入框
        drawInputBox(inputX, userInputY, INPUT_WIDTH, INPUT_HEIGHT, L"用户名：", username, focusIndex == 0); 
        drawInputBox(inputX, pwdInputY, INPUT_WIDTH, INPUT_HEIGHT, L"密  码：", "********", focusIndex == 1); 
        drawInputBox(inputX, idInputY, INPUT_WIDTH, INPUT_HEIGHT, L"学  号：", studentId, focusIndex == 2);
        drawInputBox(inputX, nameInputY, INPUT_WIDTH, INPUT_HEIGHT, L"姓  名：", name, focusIndex == 3); 
        drawInputBox(inputX, phoneInputY, INPUT_WIDTH, INPUT_HEIGHT, L"联系方式：", phone, focusIndex == 3);
        drawInputBox(inputX, genderInputY, INPUT_WIDTH, INPUT_HEIGHT, L"性  别：", gender, focusIndex == 3);
        sprintf(ageStr, "%d", age);
        drawInputBox(inputX, ageInputY, INPUT_WIDTH, INPUT_HEIGHT, L"年  龄：", ageStr, focusIndex == 3);
        drawInputBox(inputX, clnameInputY, INPUT_WIDTH, INPUT_HEIGHT, L"班  级：", clname, focusIndex == 3);
        drawInputBox(inputX, locationInputY, INPUT_WIDTH, INPUT_HEIGHT, L"位  置：", location, focusIndex == 3);

        //判断鼠标悬停在注册or返回
        bool registerHover = (mx >= registerBtnX && mx <= registerBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        bool backHover = (mx >= backBtnX && mx <= backBtnX + BTN_WIDTH && my >= btnY && my <= btnY + BTN_HEIGHT);
        //绘制高亮效果
        drawButton(registerBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"注册", registerHover); 
        drawButton(backBtnX, btnY, BTN_WIDTH, BTN_HEIGHT, L"返回登录", backHover); 

        while (peekmessage(&msg)) {
            //读取键盘输入
            if (msg.message == WM_CHAR) {
                char* target = NULL;
                int maxLen = 0;
                switch (focusIndex) {
                case 0: target = username; maxLen = 49; break;
                case 1: target = password; maxLen = 49; break;
                case 2: target = studentId; maxLen = 19; break;
                case 3: target = name; maxLen = 19; break;
                }
                if (target != NULL) {
                    if (msg.ch == VK_BACK) {
                        if (strlen(target) > 0) target[strlen(target) - 1] = '\0';
                    }
                    else if (strlen(target) < maxLen) {
                        target[strlen(target)] = msg.ch;
                    }
                }
            }
            //读取鼠标输入
            if (msg.message == WM_LBUTTONDOWN) {
                if (mx >= inputX && mx <= inputX + INPUT_WIDTH) {
                    if (my >= userInputY && my <= userInputY + INPUT_HEIGHT) focusIndex = 0;
                    else if (my >= pwdInputY && my <= pwdInputY + INPUT_HEIGHT) focusIndex = 1;
                    else if (my >= idInputY && my <= idInputY + INPUT_HEIGHT) focusIndex = 2;
                    else if (my >= nameInputY && my <= nameInputY + INPUT_HEIGHT) focusIndex = 3;
                }
                else if (registerHover) {
                    if (strlen(username) == 0 || strlen(password) == 0 || strlen(studentId) == 0 || strlen(name) == 0) {
                        MessageBoxW(GetHWnd(), L"请填写完整信息！", L"注册失败", MB_OK | MB_ICONWARNING);
                        continue;
                    }
                    int regResult = register_student(username, password, studentId, name, phone, gender, age, clname, location);
                    if (regResult == 1) {
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
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_TAB) {
                focusIndex = (focusIndex + 1) % 4;
            }
        }
        Sleep(10);
    }
}

// ==================主函数 ==================
void showStudentMainPage();
void showAdminMainPage();
int main() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);//初始化图形窗口
    setbkcolor(RGB(240, 245, 250));
    cleardevice();

    init_system();

    PageType currentPage = PAGE_WELCOME;//首页是欢迎页
    Role currentRole = ROLE_STUDENT;

    while (true) {
        switch (currentPage) {
        case PAGE_WELCOME:
            currentPage = showWelcomePage();
            if (currentPage == PAGE_LOGIN) {
                int mx = mousex();
                currentRole = (mx < WINDOW_WIDTH / 2) ? ROLE_STUDENT : ROLE_ADMIN;
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