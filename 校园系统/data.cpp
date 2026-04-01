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