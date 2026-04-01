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

int get_next_student_id(void) {
    return next_student_id++;
}
int get_next_admin_id(void) {
    return next_admin_id++;
}
int get_next_activity_id(void) {
    return next_activity_id++;
}
int get_next_category_id(void) {
    return next_category_id++;
}
int get_next_enrollment_id(void) {
    return next_enrollment_id++;
}
int get_next_attendance_id(void) {
    return next_attendance_id++;
}
int get_next_medal_id(void) {
    return next_medal_id++;
}
int get_next_usermedal_id(void) {
    return next_usermedal_id++;
}
int get_next_suggestion_id(void) {
    return next_suggestion_id++;
}
int get_next_reminder_id(void) {
    return next_reminder_id++;
}


int load_students(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        // 文件不存在，不是错误，返回0表示没有数据
        return 0;
    }

    // 释放原有的学生链表（避免重复加载导致内存泄漏）
    Student* p = student_head;
    while (p) {
        Student* tmp = p;
        p = p->next;
        free(tmp);
    }
    student_head = NULL;

    // 读取文件
    Student s;
    int max_id = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        Student* new_node = (Student*)malloc(sizeof(Student));
        if (!new_node) {
            fclose(fp);
            return -1; // 内存分配失败
        }
        *new_node = s;
        new_node->next = NULL;

        // 插入链表尾部
        if (!student_head) {
            student_head = new_node;
        }
        else {
            Student* tail = student_head;
            while (tail->next) tail = tail->next;
            tail->next = new_node;
        }

        // 更新最大ID
        if (s.id > max_id) max_id = s.id;
    }
    fclose(fp);

    // 更新ID计数器（next_student_id 是在 data.c 开头定义的静态变量）
    if (max_id >= next_student_id) {
        next_student_id = max_id + 1;
    }

    return 1; // 加载成功
}

int save_students(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;

    Student* p = student_head;
    while (p) {
        if (fwrite(p, sizeof(Student), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }
        p = p->next;
    }
    fclose(fp);
    return 1;
}

// ==================== 管理员 ====================
int load_admins(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Admin* p = admin_head;
    while (p) {
        Admin* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 活动分类 ====================
int load_categories(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Category* p = category_head;
    while (p) {
        Category* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 活动 ====================
int load_activities(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Activity* p = activity_head;
    while (p) {
        Activity* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 报名记录 ====================
int load_enrollments(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Enrollment* p = enrollment_head;
    while (p) {
        Enrollment* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 签到记录 ====================
int load_attendances(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Attendance* p = attendance_head;
    while (p) {
        Attendance* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 勋章定义 ====================
int load_medals(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Medal* p = medal_head;
    while (p) {
        Medal* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 用户勋章 ====================
int load_usermedals(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    UserMedal* p = usermedal_head;
    while (p) {
        UserMedal* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 建议箱 ====================
int load_suggestions(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Suggestion* p = suggestion_head;
    while (p) {
        Suggestion* tmp = p;
        p = p->next;
        free(tmp);
    }
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

// ==================== 提醒记录 ====================
int load_reminders(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    Reminder* p = reminder_head;
    while (p) {
        Reminder* tmp = p;
        p = p->next;
        free(tmp);
    }
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