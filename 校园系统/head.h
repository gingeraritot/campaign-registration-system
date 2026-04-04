#pragma once
#ifndef ACTIVITY_SYSTEM_H
#define ACTIVITY_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 常量定义 */
#define MAX_USERNAME     20
#define MAX_PASSWORD     20
#define MAX_NAME         20
#define MAX_STUDENT_ID   12
#define MAX_PHONE        15
#define MAX_ACTIVITY_NAME 50
#define MAX_LOCATION     100
#define MAX_TIME         20
#define MAX_DESCRIPTION  500
#define MAX_CATEGORY_NAME 20
#define MAX_REWARD       50
#define MAX_SUGGESTION   500
#define MAX_FILENAME     100

// 数据文件路径
#define STUDENTS_FILE     "students.dat"
#define ADMINS_FILE       "admins.dat"
#define ACTIVITIES_FILE   "activities.dat"
#define CATEGORIES_FILE   "categories.dat"
#define ENROLLMENTS_FILE  "enrollments.dat"
#define ATTENDANCES_FILE  "attendances.dat"
#define MEDALS_FILE       "medals.dat"
#define USERMEDALS_FILE   "usermedals.dat"
#define SUGGESTIONS_FILE  "suggestions.dat"
#define REMINDERS_FILE    "reminders.dat"

/* ---------- ID分配器 ---------- */
int get_next_student_id(void);
int get_next_admin_id(void);
int get_next_activity_id(void);
int get_next_category_id(void);
int get_next_enrollment_id(void);
int get_next_attendance_id(void);
int get_next_medal_id(void);
int get_next_usermedal_id(void);
int get_next_suggestion_id(void);
int get_next_reminder_id(void);


/* 枚举定义 */
typedef enum { ROLE_ADMIN, ROLE_STUDENT } Role;           // 角色
typedef enum { DRAFT = 0, PUBLISHED, TAKEN_DOWN } ActivityStatus; // 活动状态
typedef enum { UNREAD = 0, READ } SuggestionStatus;       // 建议状态
typedef enum { NOT_CHECKED = 0, CHECKED } CheckStatus;    // 签到状态（快速标记）

/* ---------- 学生结构体 ---------- */
typedef struct Student {
    int id;                         // 学生ID
    char username[MAX_USERNAME];    // 用户名
    char password[MAX_PASSWORD];    // 密码
    char name[MAX_NAME];            // 姓名
    char student_id[MAX_STUDENT_ID];// 学号
    char phone[MAX_PHONE];          // 联系方式
    char gender[5];                 // 性别
    int age;                        // 年龄
    char class_name[20];            // 班级
    char location[100];             // 定位
    int remind_ahead;               // 提前提醒秒数（默认 86400 = 24小时）
    Role role;                      // 角色标记（固定为 ROLE_STUDENT）
    struct Student* next;           // 链表指针
} Student;

/* ---------- 管理员结构体 ---------- */
typedef struct Admin {
    int id;                         // 管理员ID
    char username[MAX_USERNAME];    // 用户名
    char password[MAX_PASSWORD];    // 密码
    char name[MAX_NAME];            // 姓名
    char phone[MAX_PHONE];          // 联系方式
    Role role;                      // 角色标记（固定为 ROLE_ADMIN）
    struct Admin* next;             // 链表指针
} Admin;

/* ---------- 活动分类结构体 ---------- */
typedef struct Category {
    int id;                         // 分类ID
    char name[MAX_CATEGORY_NAME];   // 类别名
    struct Category* next;          // 链表指针
} Category;


/* ---------- 活动结构体 ---------- */
typedef struct Activity {
    int id;                         // 活动ID
    char name[MAX_ACTIVITY_NAME];   // 活动名称
    char start_time[MAX_TIME];      // 开始时间（如 "2025-05-10 14:00"）
    int duration;                   // 时长（小时）
    char location[MAX_LOCATION];    // 地点
    int max_participants;           // 人数上限
    int current_count;              // 当前报名人数
    char description[MAX_DESCRIPTION]; // 描述（携带物品、专业要求等）
    char reward[MAX_REWARD];        // 奖励
    int category_id;                // 所属分类ID
    char items_to_bring[200];       // 需要携带的物品清单
    ActivityStatus status;          // 状态（草稿/发布/下架）
    struct Activity* next;          // 链表指针
} Activity;

/* ---------- 报名记录结构体 ---------- */
typedef struct Enrollment {
    int id;                         // 记录ID
    int activity_id;                // 关联活动ID
    int student_id;                 // 关联学生ID
    char student_name[MAX_NAME];    // 学生姓名（冗余）
    char student_phone[MAX_PHONE];  // 联系方式（冗余）
    time_t enroll_time;             // 报名时间戳
    CheckStatus check_status;       // 签到状态（快速标记）
    struct Enrollment* next;        // 链表指针
} Enrollment;

/* ---------- 勋章定义结构体 ---------- */
typedef struct Medal {
    int id;                         // 勋章ID
    char name[20];                  // 勋章名称
    struct Medal* next;             // 链表指针
} Medal;

/* ---------- 用户勋章记录结构体 ---------- */
typedef struct UserMedal {
    int id;                         // 记录ID
    int student_id;                 // 学生ID
    int medal_id;                   // 勋章ID
    time_t obtain_time;             // 获得时间
    struct UserMedal* next;         // 链表指针
} UserMedal;

/* ---------- 建议箱结构体 ---------- */
typedef struct Suggestion {
    int id;                         // 建议ID
    int student_id;                 // 学生ID
    char content[MAX_SUGGESTION];   // 内容
    time_t submit_time;             // 提交时间
    SuggestionStatus status;        // 状态（未读/已读）
    struct Suggestion* next;        // 链表指针
} Suggestion;

/* ---------- 提醒记录结构体 ---------- */
typedef struct Reminder {
    int id;                         // 记录ID
    int student_id;                 // 学生ID
    int activity_id;                // 活动ID
    int reminded;                   // 0-未提醒，1-已提醒
    struct Reminder* next;          // 链表指针
} Reminder;

/* ---------- 签到签退记录结构体 ---------- */
typedef struct Attendance {
    int id;                         // 记录ID
    int student_id;                 // 学生ID
    int activity_id;                // 活动ID
    time_t checkin_time;            // 签到时间戳（0表示未签到）
    time_t checkout_time;           // 签退时间戳（0表示未签退）
    struct Attendance* next;        // 链表指针
} Attendance;



/* ---------- 全局链表声明 ---------- */
extern Student* student_head;
extern Admin* admin_head;
extern Category* category_head;
extern Activity* activity_head;
extern Enrollment* enrollment_head;
extern Medal* medal_head;
extern UserMedal* usermedal_head;
extern Suggestion* suggestion_head;
extern Reminder* reminder_head;
extern Attendance* attendance_head;



/* ---------- 文件操作 ---------- */
int load_all_data(void);
int save_all_data(void);
int load_students(const char* filename);
int save_students(const char* filename);
int load_admins(const char* filename);
int save_admins(const char* filename);
int load_categories(const char* filename);
int save_categories(const char* filename);
int load_activities(const char* filename);
int save_activities(const char* filename);
int load_enrollments(const char* filename);
int save_enrollments(const char* filename);
int load_medals(const char* filename);
int save_medals(const char* filename);
int load_usermedals(const char* filename);
int save_usermedals(const char* filename);
int load_suggestions(const char* filename);
int save_suggestions(const char* filename);
int load_reminders(const char* filename);
int save_reminders(const char* filename);
int load_attendances(const char* filename);
int save_attendances(const char* filename);

/* ---------- 用户管理 ---------- */
void* login(const char* username, const char* password, Role* role); // 返回 Student* 或 Admin*
int register_student(const char* username, const char* password, const char* name,
    const char* student_id, const char* phone, const char* gender,
    int age, const char* class_name, const char* location);
Student* find_student_by_id(int id);
Admin* find_admin_by_id(int id);
Student* find_student_by_username(const char* username);
Admin* find_admin_by_username(const char* username);

/* ---------- 活动管理 ---------- */
int add_activity(Activity* act);                 // 添加活动（默认草稿）
int publish_activity(int activity_id);           // 发布草稿
int take_down_activity(int activity_id);         // 下架活动
int update_activity(Activity* act);              // 修改活动（支持草稿）
int delete_activity(int activity_id);
Activity* find_activity_by_id(int id);
void list_activities(void);                      // 显示已发布活动（按热度排序）
void list_activities_by_category(int category_id); // 按分类显示
void list_drafts(void);                          // 显示草稿箱（管理员）
void search_activities(const char* keyword);     // 按名称/描述搜索
void sort_activities_by_enrollment(void);        // 按报名人数排序

/* ---------- 活动分类管理 ---------- */
int add_category(const char* name);
int delete_category(int id);
void list_categories(void);
Category* find_category_by_id(int id);
Category* find_category_by_id(int id);

/* ---------- 报名管理 ---------- */
int enroll_student(int student_id, int activity_id);     // 报名（含时间冲突检查）
int cancel_enrollment(int student_id, int activity_id);  // 退选
int check_time_conflict(int student_id, int new_activity_id); // 时间互斥检查
void list_enrollments_for_activity(int activity_id);     // 管理员查看报名详情
int export_enrollment_list(int activity_id, const char* filename); // 导出名单（CSV）

/* ---------- 签到签退管理 ---------- */
int checkin(int student_id, int activity_id);            // 签到
int checkout(int student_id, int activity_id);           // 签退
int is_checked_in(int student_id, int activity_id);
int is_checked_out(int student_id, int activity_id);
void list_activity_attendance(int activity_id);          // 管理员查看签到情况
void list_my_attendance(int student_id);                 // 学生查看自己的签到记录
int create_attendance_record(int student_id, int activity_id); // 报名后自动创建签到记录

/* ---------- 勋章管理 ---------- */
int add_medal(const char* name);                         // 添加勋章
int award_medal(int student_id, int medal_id);           // 授予勋章
void list_user_medals(int student_id);                   // 查看学生获得的勋章
void check_and_award_first_enroll(int student_id);       // 首次报名勋章
void check_and_award_time_based(int student_id);         // 累计时长勋章

/* ---------- 建议箱管理 ---------- */
int add_suggestion(int student_id, const char* content); // 提交建议
void list_suggestions(void);                             // 管理员查看所有建议
int mark_suggestion_read(int suggestion_id);             // 标记已读

/* ---------- 自动提醒 ---------- */
void check_and_remind(int student_id);                   // 登录时调用，提醒即将开始的活动
int is_reminded(int student_id, int activity_id);        // 检查是否已提醒过

/* ---------- 系统初始化与清理 ---------- */
void init_system(void);                                  // 加载所有数据
void free_all(void);                                     // 释放所有链表内存

/* ---------- 增强功能 ---------- */
int set_remind_ahead(int student_id, int seconds);
void open_map_navigation(const char* location);
void remind_activity_update(int activity_id);

#endif /* ACTIVITY_SYSTEM_H */