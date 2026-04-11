#ifndef ACTIVITY_SYSTEM_H
#define ACTIVITY_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 常量定义
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


// 数据文件路径（直接指向你的 Debug 目录）
#define DATA_DIR          "C:\\Users\\GingerariToT\\Desktop\\xiaoyuanxitong\\x64\\Debug\\"
#define STUDENTS_FILE     DATA_DIR "students.dat"
#define ADMINS_FILE       DATA_DIR "admins.dat"
#define ACTIVITIES_FILE   DATA_DIR "activities.dat"
#define CATEGORIES_FILE   DATA_DIR "categories.dat"
#define ENROLLMENTS_FILE  DATA_DIR "enrollments.dat"
#define ATTENDANCES_FILE  DATA_DIR "attendances.dat"
#define MEDALS_FILE       DATA_DIR "medals.dat"
#define USERMEDALS_FILE   DATA_DIR "usermedals.dat"
#define SUGGESTIONS_FILE  DATA_DIR "suggestions.dat"
#define REMINDERS_FILE    DATA_DIR "reminders.dat"
#define CONFIG_FILE       DATA_DIR "system.cfg"

// ID分配器
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

// 枚举
typedef enum { ROLE_ADMIN, ROLE_STUDENT } Role;
typedef enum { DRAFT = 0, PUBLISHED, TAKEN_DOWN } ActivityStatus;
typedef enum { UNREAD = 0, READ } SuggestionStatus;
typedef enum { NOT_CHECKED = 0, CHECKED } CheckStatus;

// 学生结构体
typedef struct Student {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char name[MAX_NAME];
    char student_id[MAX_STUDENT_ID];
    char phone[MAX_PHONE];
    char gender[5];
    int age;
    char class_name[20];
    char location[100];
    int remind_ahead;
    Role role;
    struct Student* next;
} Student;

// 管理员结构体
typedef struct Admin {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    Role role;
    struct Admin* next;
} Admin;

// 活动分类结构体
typedef struct Category {
    int id;
    char name[MAX_CATEGORY_NAME];
    struct Category* next;
} Category;

// 活动结构体
typedef struct Activity {
    int id;
    char name[MAX_ACTIVITY_NAME];
    char start_time[MAX_TIME];
    int duration;
    char location[MAX_LOCATION];
    int max_participants;
    int current_count;
    char description[MAX_DESCRIPTION];
    char reward[MAX_REWARD];
    int category_id;
    char items_to_bring[200];
    ActivityStatus status;
    struct Activity* next;
} Activity;

// 报名记录结构体
typedef struct Enrollment {
    int id;
    int activity_id;
    int student_id;
    char student_name[MAX_NAME];
    char student_phone[MAX_PHONE];
    time_t enroll_time;
    CheckStatus check_status;
    struct Enrollment* next;
} Enrollment;

// 勋章定义结构体
typedef struct Medal {
    int id;
    char name[20];
    struct Medal* next;
} Medal;

// 用户勋章记录结构体
typedef struct UserMedal {
    int id;
    int student_id;
    int medal_id;
    time_t obtain_time;
    struct UserMedal* next;
} UserMedal;

// 建议箱结构体
typedef struct Suggestion {
    int id;
    int student_id;
    char content[MAX_SUGGESTION];
    time_t submit_time;
    SuggestionStatus status;
    char reply[MAX_SUGGESTION];
    time_t reply_time;
    struct Suggestion* next;
} Suggestion;

// 提醒记录结构体
typedef struct Reminder {
    int id;
    int student_id;
    int activity_id;
    int reminded;
    struct Reminder* next;
} Reminder;

// 签到签退记录结构体
typedef struct Attendance {
    int id;
    int student_id;
    int activity_id;
    time_t checkin_time;
    time_t checkout_time;
    struct Attendance* next;
} Attendance;

// 全局链表声明
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

// 文件操作
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

// 清理过期活动
void expire_activities(void);

// 用户管理
void* login(const char* username, const char* password, Role* role);
int register_student(const char* username, const char* password, const char* name,
    const char* student_id, const char* phone, const char* gender,
    int age, const char* class_name, const char* location);
Student* find_student_by_id(int id);
Admin* find_admin_by_id(int id);
Student* find_student_by_username(const char* username);
Admin* find_admin_by_username(const char* username);

// 活动管理
int add_activity(Activity* act);
int publish_activity(int activity_id);
int take_down_activity(int activity_id);
int update_activity(Activity* act);
int delete_activity(int activity_id);
Activity* find_activity_by_id(int id);
void list_activities(void);
void list_activities_by_category(int category_id);
void list_drafts(void);
void search_activities(const char* keyword);
void sort_activities_by_enrollment(void);

// 活动分类管理
int add_category(const char* name);
int delete_category(int id);
void list_categories(void);
Category* find_category_by_id(int id);

// 报名管理
int enroll_student(int student_id, int activity_id);
int cancel_enrollment(int student_id, int activity_id);
int check_time_conflict(int student_id, int new_activity_id);
void list_enrollments_for_activity(int activity_id);
int export_enrollment_list(int activity_id, const char* filename);

// 签到签退管理
int checkin(int student_id, int activity_id);
int checkout(int student_id, int activity_id);
int is_checked_in(int student_id, int activity_id);
int is_checked_out(int student_id, int activity_id);
void list_activity_attendance(int activity_id);
void list_my_attendance(int student_id);
int create_attendance_record(int student_id, int activity_id);
int delete_attendance_record(int student_id, int activity_id);

// 勋章管理
int add_medal(const char* name);
int award_medal(int student_id, int medal_id);
void list_user_medals(int student_id);
void check_and_award_first_enroll(int student_id);
void check_and_award_time_based(int student_id);

// 建议箱管理
int add_suggestion(int student_id, const char* content);
int reply_suggestion(int suggestion_id, const char* reply_content);
void list_suggestions(void);
int mark_suggestion_read(int suggestion_id);

// 自动提醒
void check_and_remind(int student_id);
int is_reminded(int student_id, int activity_id);

// 系统初始化与清理
void init_system(void);
void free_all(void);

// 增强功能
int set_remind_ahead(int student_id, int seconds);
void open_map_navigation(const char* location);
void remind_activity_update(int activity_id);

// 界面入口
void runStudentMainPage(int studentId);
void runAdminMainPage(void);

#endif /* ACTIVITY_SYSTEM_H */