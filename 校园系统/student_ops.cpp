#include "head.h"
#include <stdio.h>
#include <string.h>
#include <time.h>



int enroll_student(int student_id, int activity_id) {
    if (student_id <= 0 || activity_id <= 0) return -1;

    Student* s = find_student_by_id(student_id);
    Activity* a = find_activity_by_id(activity_id);

    if (!s || !a) return -2;
    if (a->status != PUBLISHED) return -3;
    if (a->current_count >= a->max_participants) return -4;

    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == student_id && e->activity_id == activity_id) {
            return -5;
        }
        e = e->next;
    }

    Enrollment* new_e = (Enrollment*)malloc(sizeof(Enrollment));
    if (!new_e) return -6;

    new_e->id = get_next_enrollment_id();
    new_e->student_id = student_id;
    new_e->activity_id = activity_id;
    new_e->enroll_time = time(NULL);
    new_e->next = NULL;

    if (!enrollment_head) {
        enrollment_head = new_e;
    }
    else {
        Enrollment* tail = enrollment_head;
        while (tail->next) tail = tail->next;
        tail->next = new_e;
    }

    a->current_count++;
    return 0;
}

int cancel_enrollment(int student_id, int activity_id) {
    if (student_id <= 0 || activity_id <= 0) return -1;

    Enrollment* prev = NULL;
    Enrollment* cur = enrollment_head;

    while (cur) {
        if (cur->student_id == student_id && cur->activity_id == activity_id) {
            if (prev) prev->next = cur->next;
            else enrollment_head = cur->next;

            free(cur);
            Activity* a = find_activity_by_id(activity_id);
            if (a && a->current_count > 0) a->current_count--;
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -2;
}

int checkin(int student_id, int activity_id) {
    Attendance* ar = attendance_head;
    while (ar) {
        if (ar->student_id == student_id && ar->activity_id == activity_id) {
            ar->checkin_time = time(NULL);
            return 0;
        }
        ar = ar->next;
    }
    return -1;
}

int checkout(int student_id, int activity_id) {
    Attendance* ar = attendance_head;
    while (ar) {
        if (ar->student_id == student_id && ar->activity_id == activity_id) {
            ar->checkout_time = time(NULL);
            return 0;
        }
        ar = ar->next;
    }
    return -1;
}

int view_enrolled_activities(int student_id) {
    printf("===== 已报名活动 =====\n");
    Enrollment* e = enrollment_head;
    int found = 0;

    while (e) {
        if (e->student_id == student_id) {
            Activity* a = find_activity_by_id(e->activity_id);
            if (a) {
                printf("活动：%s\n", a->name);
                found = 1;
            }
        }
        e = e->next;
    }

    if (!found) printf("暂无报名\n");
    return 0;
}