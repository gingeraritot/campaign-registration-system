#include "head.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Helper: parse "YYYY-MM-DD HH:MM" -> time_t
static time_t parse_time_str(const char* str) {
    if (!str) return 0;
    struct tm tm = { 0 };
    if (sscanf(str, "%d-%d-%d %d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min) >= 5) {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        tm.tm_sec = 0;
        return mktime(&tm);
    }
    return 0;
}

int delete_attendance_record(int student_id, int activity_id);
int search_activities(const char* keyword, Activity** results, int max_results);
int get_all_published_activities(Activity** results, int max_results);
int get_activities_by_category(int category_id, Activity** results, int max_results);

// 检查学生是否与已报名活动时间冲突。若冲突返回 1，否则返回 0。
int check_time_conflict(int student_id, int new_activity_id) {
    if (student_id <= 0 || new_activity_id <= 0) return 0;

    Activity* new_act = find_activity_by_id(new_activity_id);
    if (!new_act) return 0;
    time_t new_start = parse_time_str(new_act->start_time);
    if (new_start == 0) return 0;
    time_t new_end = new_start + (time_t)new_act->duration * 3600;

    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == student_id && e->activity_id != new_activity_id) {
            Activity* a = find_activity_by_id(e->activity_id);
            if (!a) { e = e->next; continue; }
            time_t s = parse_time_str(a->start_time);
            if (s == 0) { e = e->next; continue; }
            time_t en = s + (time_t)a->duration * 3600;

            // 重叠判断：区间 [new_start, new_end) 与 [s, en) 有交集则冲突
            if (new_start < en && s < new_end) return 1;
        }
        e = e->next;
    }
    return 0;
}

int enroll_student(int student_id, int activity_id) {
    if (student_id <= 0 || activity_id <= 0) return -1;

    Student* s = find_student_by_id(student_id);
    Activity* a = find_activity_by_id(activity_id);

    if (!s || !a) return -2;
    if (a->status != PUBLISHED) return -3;
    if (a->current_count >= a->max_participants) return -4;
    // 时间冲突检查：若与已报名的活动时间区间重叠则拒绝
    if (check_time_conflict(student_id, activity_id)) return -8;

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
    strcpy(new_e->student_name, s->name);
    strcpy(new_e->student_phone, s->phone);
    new_e->enroll_time = time(NULL);
    new_e->check_status = NOT_CHECKED;
    new_e->next = NULL;

    if (!enrollment_head) enrollment_head = new_e;
    else {
        Enrollment* tail = enrollment_head;
        while (tail->next) tail = tail->next;
        tail->next = new_e;
    }

    a->current_count++;

    // 保存数据
    if (save_enrollments(ENROLLMENTS_FILE) != 1) return -7;
    if (save_activities(ACTIVITIES_FILE) != 1) return -7;

    // 创建签到记录
    create_attendance_record(student_id, activity_id);
    // 授予首次报名勋章
    check_and_award_first_enroll(student_id);

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

            save_enrollments(ENROLLMENTS_FILE);
            save_activities(ACTIVITIES_FILE);

            delete_attendance_record(student_id, activity_id);

            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -2;
}

int checkin(int student_id, int activity_id) {
    // 限制签到时间窗口：活动开始前后15分钟允许签到
    Activity* act = find_activity_by_id(activity_id);
    if (!act) return -1;
    time_t start = parse_time_str(act->start_time);
    if (start == 0) return -1;
    time_t now = time(NULL);
    const int ALLOW_SECONDS = 15 * 60;
    if (now < start - ALLOW_SECONDS || now > start + ALLOW_SECONDS) return -2; // 超出签到窗口

    Attendance* ar = attendance_head;
    while (ar) {
        if (ar->student_id == student_id && ar->activity_id == activity_id) {
            ar->checkin_time = now;
            save_attendances(ATTENDANCES_FILE);
            return 0;
        }
        ar = ar->next;
    }
    return -1;
}

int checkout(int student_id, int activity_id) {
    // 限制签退时间：活动开始后至少经过0分钟且不超过活动时长+15分钟
    Activity* act = find_activity_by_id(activity_id);
    if (!act) return -1;
    time_t start = parse_time_str(act->start_time);
    if (start == 0) return -1;
    time_t now = time(NULL);
    time_t end = start + (time_t)act->duration * 3600;
    const int ALLOW_SECONDS = 15 * 60;
    if (now < start - ALLOW_SECONDS || now > end + ALLOW_SECONDS) return -2; // 超出签退窗口

    Attendance* ar = attendance_head;
    while (ar) {
        if (ar->student_id == student_id && ar->activity_id == activity_id) {
            ar->checkout_time = now;
            save_attendances(ATTENDANCES_FILE);
            check_and_award_time_based(student_id);
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

int search_activities(const char* keyword, Activity** results, int max_results) {
    if (keyword == NULL || strlen(keyword) == 0 || results == NULL || max_results <= 0) {
        return 0;
    }

    Activity* act = activity_head;
    int found_count = 0;
    int matched_category_id = -1;

    Category* cat = category_head;
    while (cat != NULL) {
        if (strstr(cat->name, keyword) != NULL) {
            matched_category_id = cat->id;
            break;
        }
        cat = cat->next;
    }

    while (act != NULL && found_count < max_results) {
        int is_match = 0;
        if (act->status == PUBLISHED) {
            if (strstr(act->name, keyword) != NULL) {
                is_match = 1;
            }
            if (!is_match && matched_category_id != -1 && act->category_id == matched_category_id) {
                is_match = 1;
            }
            if (!is_match && strstr(act->description, keyword) != NULL) {
                is_match = 1;
            }
            if (is_match) {
                results[found_count] = act;
                found_count++;
            }
        }
        act = act->next;
    }
    return found_count;
}

int get_all_published_activities(Activity** results, int max_results) {
    if (results == NULL || max_results <= 0) return 0;
    Activity* act = activity_head;
    int found_count = 0;
    while (act != NULL && found_count < max_results) {
        if (act->status == PUBLISHED) {
            results[found_count] = act;
            found_count++;
        }
        act = act->next;
    }
    return found_count;
}

int get_activities_by_category(int category_id, Activity** results, int max_results) {
    if (category_id <= 0 || results == NULL || max_results <= 0) return 0;
    Activity* act = activity_head;
    int found_count = 0;
    while (act != NULL && found_count < max_results) {
        if (act->status == PUBLISHED && act->category_id == category_id) {
            results[found_count] = act;
            found_count++;
        }
        act = act->next;
    }
    return found_count;
}