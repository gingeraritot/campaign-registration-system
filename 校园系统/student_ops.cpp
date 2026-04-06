#include "head.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int delete_attendance_record(int student_id, int activity_id);
int search_activities(const char* keyword, Activity** results, int max_results);
int get_all_published_activities(Activity** results, int max_results);
int get_activities_by_category(int category_id, Activity** results, int max_results);

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

// 搜索活动 - 根据关键词返回匹配的活动指针数组
// 参数：
//   keyword: 搜索关键词（活动名称或类别名称）
//   results: 存储 Activity* 指针的数组
//   max_results: 最多返回多少个结果
// 返回值：实际找到的活动数量
int search_activities(const char* keyword, Activity** results, int max_results) {
    if (keyword == NULL || strlen(keyword) == 0 || results == NULL || max_results <= 0) {
        return 0;
    }

    Activity* act = activity_head;
    int found_count = 0;
    int matched_category_id = -1;

    // 检查关键词是否匹配某个分类名称
    Category* cat = category_head;
    while (cat != NULL) {
        if (strstr(cat->name, keyword) != NULL) {
            matched_category_id = cat->id;
            break;
        }
        cat = cat->next;
    }

    // 遍历所有活动
    while (act != NULL && found_count < max_results) {
        int is_match = 0;

        // 只搜索已发布的活动
        if (act->status == PUBLISHED) {
            // 匹配条件1：活动名称包含关键词
            if (strstr(act->name, keyword) != NULL) {
                is_match = 1;
            }

            // 匹配条件2：活动分类ID匹配
            if (!is_match && matched_category_id != -1 && act->category_id == matched_category_id) {
                is_match = 1;
            }

            // 匹配条件3：活动描述包含关键词
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
// 获取所有已发布的活动（返回 Activity* 数组）
int get_all_published_activities(Activity** results, int max_results) {
    if (results == NULL || max_results <= 0) {
        return 0;
    }
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

// 根据分类ID获取该分类下的所有活动
int get_activities_by_category(int category_id, Activity** results, int max_results) {
    if (category_id <= 0 || results == NULL || max_results <= 0) {
        return 0;
    }

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