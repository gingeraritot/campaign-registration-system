#include "head.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局链表头
Student* student_head = NULL;
Admin* admin_head = NULL;
Activity* activity_head = NULL;
Category* category_head = NULL;
Enrollment* enrollment_head = NULL;
Attendance* attendance_head = NULL;
Medal* medal_head = NULL;
UserMedal* usermedal_head = NULL;
Suggestion* suggestion_head = NULL;
Reminder* reminder_head = NULL;

// ID 计数器（用于分配新ID）
static int next_student_id = 1;
static int next_admin_id = 1;
static int next_activity_id = 1;
static int next_category_id = 1;
static int next_enrollment_id = 1;
static int next_attendance_id = 1;
static int next_medal_id = 1;
static int next_usermedal_id = 1;
static int next_suggestion_id = 1;
static int next_reminder_id = 1;

int get_next_student_id(void) { return next_student_id++; }
int get_next_admin_id(void) { return next_admin_id++; }
int get_next_activity_id(void) { return next_activity_id++; }
int get_next_category_id(void) { return next_category_id++; }
int get_next_enrollment_id(void) { return next_enrollment_id++; }
int get_next_attendance_id(void) { return next_attendance_id++; }
int get_next_medal_id(void) { return next_medal_id++; }
int get_next_usermedal_id(void) { return next_usermedal_id++; }
int get_next_suggestion_id(void) { return next_suggestion_id++; }
int get_next_reminder_id(void) { return next_reminder_id++; }

// -------------------- 学生 --------------------
int load_students(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Student* p = student_head;
    while (p) { Student* tmp = p; p = p->next; free(tmp); }
    student_head = NULL;
    Student s;
    int max_id = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        Student* new_node = (Student*)malloc(sizeof(Student));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = s;
        new_node->next = NULL;
        if (!student_head) student_head = new_node;
        else {
            Student* tail = student_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }



        if (s.id > max_id) max_id = s.id;
    }
    fclose(fp);
    if (max_id >= next_student_id) next_student_id = max_id + 1;
    return 1;
}
int save_students(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Student* p = student_head;
    int count = 0;  // 添加计数变量

    // 【在这里添加调试输出】
    printf("【调试】开始保存学生数据到 %s\n", filename);

    while (p) {
        // 【在这里添加调试输出】
        printf("【调试】保存学生 #%d: ID=%d, 用户名=%s, 密码=%s\n",
            ++count, p->id, p->username, p->password);

        if (fwrite(p, sizeof(Student), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
        p = p->next;
    }
    printf("【调试】成功保存 %d 个学生到文件\n", count);
    fclose(fp);
    return 1;
}

// -------------------- 管理员 --------------------
int load_admins(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Admin* p = admin_head;
    while (p) { Admin* tmp = p; p = p->next; free(tmp); }
    admin_head = NULL;
    Admin a;
    int max_id = 0;
    while (fread(&a, sizeof(Admin), 1, fp) == 1) {
        Admin* new_node = (Admin*)malloc(sizeof(Admin));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = a;
        new_node->next = NULL;
        if (!admin_head) admin_head = new_node;
        else {
            Admin* tail = admin_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (a.id > max_id) max_id = a.id;
    }
    fclose(fp);
    if (max_id >= next_admin_id) next_admin_id = max_id + 1;
    return 1;
}
int save_admins(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Admin* p = admin_head;
    while (p) {
        if (fwrite(p, sizeof(Admin), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 活动分类 --------------------
int load_categories(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Category* p = category_head;
    while (p) { Category* tmp = p; p = p->next; free(tmp); }
    category_head = NULL;
    Category c;
    int max_id = 0;
    while (fread(&c, sizeof(Category), 1, fp) == 1) {
        Category* new_node = (Category*)malloc(sizeof(Category));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = c;
        new_node->next = NULL;
        if (!category_head) category_head = new_node;
        else {
            Category* tail = category_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (c.id > max_id) max_id = c.id;
    }
    fclose(fp);
    if (max_id >= next_category_id) next_category_id = max_id + 1;
    return 1;
}
int save_categories(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Category* p = category_head;
    while (p) {
        if (fwrite(p, sizeof(Category), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 活动 --------------------
int load_activities(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Activity* p = activity_head;
    while (p) { Activity* tmp = p; p = p->next; free(tmp); }
    activity_head = NULL;
    Activity a;
    int max_id = 0;
    while (fread(&a, sizeof(Activity), 1, fp) == 1) {
        Activity* new_node = (Activity*)malloc(sizeof(Activity));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = a;
        new_node->next = NULL;
        if (!activity_head) activity_head = new_node;
        else {
            Activity* tail = activity_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (a.id > max_id) max_id = a.id;
    }
    fclose(fp);
    if (max_id >= next_activity_id) next_activity_id = max_id + 1;
    return 1;
}
int save_activities(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Activity* p = activity_head;
    while (p) {
        if (fwrite(p, sizeof(Activity), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 报名记录 --------------------
int load_enrollments(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Enrollment* p = enrollment_head;
    while (p) { Enrollment* tmp = p; p = p->next; free(tmp); }
    enrollment_head = NULL;
    Enrollment e;
    int max_id = 0;
    while (fread(&e, sizeof(Enrollment), 1, fp) == 1) {
        Enrollment* new_node = (Enrollment*)malloc(sizeof(Enrollment));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = e;
        new_node->next = NULL;
        if (!enrollment_head) enrollment_head = new_node;
        else {
            Enrollment* tail = enrollment_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (e.id > max_id) max_id = e.id;
    }
    fclose(fp);
    if (max_id >= next_enrollment_id) next_enrollment_id = max_id + 1;
    return 1;
}
int save_enrollments(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Enrollment* p = enrollment_head;
    while (p) {
        if (fwrite(p, sizeof(Enrollment), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 签到记录 --------------------
int load_attendances(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Attendance* p = attendance_head;
    while (p) { Attendance* tmp = p; p = p->next; free(tmp); }
    attendance_head = NULL;
    Attendance a;
    int max_id = 0;
    while (fread(&a, sizeof(Attendance), 1, fp) == 1) {
        Attendance* new_node = (Attendance*)malloc(sizeof(Attendance));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = a;
        new_node->next = NULL;
        if (!attendance_head) attendance_head = new_node;
        else {
            Attendance* tail = attendance_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (a.id > max_id) max_id = a.id;
    }
    fclose(fp);
    if (max_id >= next_attendance_id) next_attendance_id = max_id + 1;
    return 1;
}
int save_attendances(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Attendance* p = attendance_head;
    while (p) {
        if (fwrite(p, sizeof(Attendance), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 勋章定义 --------------------
int load_medals(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Medal* p = medal_head;
    while (p) { Medal* tmp = p; p = p->next; free(tmp); }
    medal_head = NULL;
    Medal m;
    int max_id = 0;
    while (fread(&m, sizeof(Medal), 1, fp) == 1) {
        Medal* new_node = (Medal*)malloc(sizeof(Medal));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = m;
        new_node->next = NULL;
        if (!medal_head) medal_head = new_node;
        else {
            Medal* tail = medal_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (m.id > max_id) max_id = m.id;
    }
    fclose(fp);
    if (max_id >= next_medal_id) next_medal_id = max_id + 1;
    return 1;
}
int save_medals(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Medal* p = medal_head;
    while (p) {
        if (fwrite(p, sizeof(Medal), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 用户勋章 --------------------
int load_usermedals(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    UserMedal* p = usermedal_head;
    while (p) { UserMedal* tmp = p; p = p->next; free(tmp); }
    usermedal_head = NULL;
    UserMedal um;
    int max_id = 0;
    while (fread(&um, sizeof(UserMedal), 1, fp) == 1) {
        UserMedal* new_node = (UserMedal*)malloc(sizeof(UserMedal));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = um;
        new_node->next = NULL;
        if (!usermedal_head) usermedal_head = new_node;
        else {
            UserMedal* tail = usermedal_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (um.id > max_id) max_id = um.id;
    }
    fclose(fp);
    if (max_id >= next_usermedal_id) next_usermedal_id = max_id + 1;
    return 1;
}
int save_usermedals(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    UserMedal* p = usermedal_head;
    while (p) {
        if (fwrite(p, sizeof(UserMedal), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 建议箱 --------------------
int load_suggestions(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Suggestion* p = suggestion_head;
    while (p) { Suggestion* tmp = p; p = p->next; free(tmp); }
    suggestion_head = NULL;
    Suggestion s;
    int max_id = 0;
    while (fread(&s, sizeof(Suggestion), 1, fp) == 1) {
        Suggestion* new_node = (Suggestion*)malloc(sizeof(Suggestion));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = s;
        new_node->next = NULL;
        if (!suggestion_head) suggestion_head = new_node;
        else {
            Suggestion* tail = suggestion_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (s.id > max_id) max_id = s.id;
    }
    fclose(fp);
    if (max_id >= next_suggestion_id) next_suggestion_id = max_id + 1;
    return 1;
}
int save_suggestions(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Suggestion* p = suggestion_head;
    while (p) {
        if (fwrite(p, sizeof(Suggestion), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 提醒记录 --------------------
int load_reminders(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    Reminder* p = reminder_head;
    while (p) { Reminder* tmp = p; p = p->next; free(tmp); }
    reminder_head = NULL;
    Reminder r;
    int max_id = 0;
    while (fread(&r, sizeof(Reminder), 1, fp) == 1) {
        Reminder* new_node = (Reminder*)malloc(sizeof(Reminder));
        if (!new_node) { fclose(fp); return -1; }
        *new_node = r;
        new_node->next = NULL;
        if (!reminder_head) reminder_head = new_node;
        else {
            Reminder* tail = reminder_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }
        if (r.id > max_id) max_id = r.id;
    }
    fclose(fp);
    if (max_id >= next_reminder_id) next_reminder_id = max_id + 1;
    return 1;
}
int save_reminders(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;
    Reminder* p = reminder_head;
    while (p) {
        if (fwrite(p, sizeof(Reminder), 1, fp) != 1) { fclose(fp); return -1; }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// -------------------- 查找函数 --------------------
Student* find_student_by_id(int id) {
    Student* p = student_head;
    while (p) { if (p->id == id) return p; p = p->next; }
    return NULL;
}
Admin* find_admin_by_id(int id) {
    Admin* p = admin_head;
    while (p) { if (p->id == id) return p; p = p->next; }
    return NULL;
}
Student* find_student_by_username(const char* username) {
    Student* p = student_head;
    while (p) { if (strcmp(p->username, username) == 0) return p; p = p->next; }
    return NULL;
}
Admin* find_admin_by_username(const char* username) {
    Admin* p = admin_head;
    while (p) { if (strcmp(p->username, username) == 0) return p; p = p->next; }
    return NULL;
}
Activity* find_activity_by_id(int id) {
    Activity* p = activity_head;
    while (p) { if (p->id == id) return p; p = p->next; }
    return NULL;
}

Category* find_category_by_id(int id) {
    Category* p = category_head;
    while (p) { if (p->id == id) return p; p = p->next; }
    return NULL;
}

// -------------------- 用户管理 --------------------
void* login(const char* username, const char* password, Role* role) {
    Student* s = find_student_by_username(username);
    if (s && strcmp(s->password, password) == 0) { *role = ROLE_STUDENT; return s; }
    Admin* a = find_admin_by_username(username);
    if (a && strcmp(a->password, password) == 0) { *role = ROLE_ADMIN; return a; }
    return NULL;
}
int register_student(const char* username, const char* password, const char* name,
    const char* student_id, const char* phone, const char* gender,
    int age, const char* class_name, const char* location) {
    if (find_student_by_username(username) != NULL) return -1;
    if (find_admin_by_username(username) != NULL) return -1;

    Student* new_s = (Student*)malloc(sizeof(Student));
    if (!new_s) return -2;

    new_s->id = get_next_student_id();
    strcpy(new_s->username, username);
    strcpy(new_s->password, password);
    strcpy(new_s->name, name);
    strcpy(new_s->student_id, student_id);
    strcpy(new_s->phone, phone);
    strcpy(new_s->gender, gender);
    new_s->age = age;
    strcpy(new_s->class_name, class_name);
    strcpy(new_s->location, location);
    new_s->role = ROLE_STUDENT;
    new_s->remind_ahead = 24 * 3600;
    new_s->next = NULL;

    if (!student_head) student_head = new_s;
    else {
        Student* tail = student_head;
        while (tail->next) tail = tail->next;
        tail->next = new_s;
    }

    // ✅ 添加这一行：保存到文件
    if (save_students(STUDENTS_FILE) != 1) {
        return -2;  // 保存失败
    }

    return new_s->id;
}
// -------------------- 系统初始化与清理 --------------------
int load_all_data(void) {
    int ret;
    ret = load_students(STUDENTS_FILE); if (ret == -1) return -1;
    ret = load_admins(ADMINS_FILE); if (ret == -1) return -1;
    ret = load_activities(ACTIVITIES_FILE); if (ret == -1) return -1;
    ret = load_categories(CATEGORIES_FILE); if (ret == -1) return -1;
    ret = load_enrollments(ENROLLMENTS_FILE); if (ret == -1) return -1;
    ret = load_attendances(ATTENDANCES_FILE); if (ret == -1) return -1;
    ret = load_medals(MEDALS_FILE); if (ret == -1) return -1;
    ret = load_usermedals(USERMEDALS_FILE); if (ret == -1) return -1;
    ret = load_suggestions(SUGGESTIONS_FILE); if (ret == -1) return -1;
    ret = load_reminders(REMINDERS_FILE); if (ret == -1) return -1;
    return 1;
}
int save_all_data(void) {
    if (save_students(STUDENTS_FILE) != 1) return -1;
    if (save_admins(ADMINS_FILE) != 1) return -1;
    if (save_activities(ACTIVITIES_FILE) != 1) return -1;
    if (save_categories(CATEGORIES_FILE) != 1) return -1;
    if (save_enrollments(ENROLLMENTS_FILE) != 1) return -1;
    if (save_attendances(ATTENDANCES_FILE) != 1) return -1;
    if (save_medals(MEDALS_FILE) != 1) return -1;
    if (save_usermedals(USERMEDALS_FILE) != 1) return -1;
    if (save_suggestions(SUGGESTIONS_FILE) != 1) return -1;
    if (save_reminders(REMINDERS_FILE) != 1) return -1;
    return 1;
}
void free_all(void) {
    Student* s = student_head;
    while (s) { Student* tmp = s; s = s->next; free(tmp); }
    student_head = NULL;
    Admin* a = admin_head;
    while (a) { Admin* tmp = a; a = a->next; free(tmp); }
    admin_head = NULL;
    Activity* act = activity_head;
    while (act) { Activity* tmp = act; act = act->next; free(tmp); }
    activity_head = NULL;
    Category* cat = category_head;
    while (cat) { Category* tmp = cat; cat = cat->next; free(tmp); }
    category_head = NULL;
    Enrollment* enr = enrollment_head;
    while (enr) { Enrollment* tmp = enr; enr = enr->next; free(tmp); }
    enrollment_head = NULL;
    Attendance* att = attendance_head;
    while (att) { Attendance* tmp = att; att = att->next; free(tmp); }
    attendance_head = NULL;
    Medal* med = medal_head;
    while (med) { Medal* tmp = med; med = med->next; free(tmp); }
    medal_head = NULL;
    UserMedal* um = usermedal_head;
    while (um) { UserMedal* tmp = um; um = um->next; free(tmp); }
    usermedal_head = NULL;
    Suggestion* sug = suggestion_head;
    while (sug) { Suggestion* tmp = sug; sug = sug->next; free(tmp); }
    suggestion_head = NULL;
    Reminder* rem = reminder_head;
    while (rem) { Reminder* tmp = rem; rem = rem->next; free(tmp); }
    reminder_head = NULL;
}
void init_system(void) {
    int ret = load_all_data();
    if (ret == -1) {
        printf("数据加载失败，程序退出\n");
        exit(1);
    }
    // 如果没有任何管理员，创建一个默认管理员
    if (admin_head == NULL) {
        Admin* default_admin = (Admin*)malloc(sizeof(Admin));
        default_admin->id = get_next_admin_id();
        strcpy(default_admin->username, "admin");
        strcpy(default_admin->password, "123456");
        strcpy(default_admin->name, "系统管理员");
        strcpy(default_admin->phone, "1234567890");
        default_admin->role = ROLE_ADMIN;
        default_admin->next = NULL;
        admin_head = default_admin;
    }
    // 如果没有任何勋章，创建默认勋章（add_medal 在 system_service.c 中实现）
    if (medal_head == NULL) {
        add_medal("首次报名");
        add_medal("时长达人");
    }
    save_all_data();
}