#include "head.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ==================== 勋章管理 ====================
int add_medal(const char* name) {
    Medal* p = medal_head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p->id;
        p = p->next;
    }
    Medal* new_medal = (Medal*)malloc(sizeof(Medal));
    if (!new_medal) return -1;
    new_medal->id = get_next_medal_id();
    strcpy(new_medal->name, name);
    new_medal->next = NULL;
    if (!medal_head) medal_head = new_medal;
    else {
        Medal* tail = medal_head;
        while (tail->next) tail = tail->next;
        tail->next = new_medal;
    }
    return new_medal->id;
}

int award_medal(int student_id, int medal_id) {
    if (!find_student_by_id(student_id)) return -1;
    Medal* m = medal_head;
    while (m && m->id != medal_id) m = m->next;
    if (!m) return -2;
    UserMedal* um = usermedal_head;
    while (um) {
        if (um->student_id == student_id && um->medal_id == medal_id) return -3;
        um = um->next;
    }
    UserMedal* new_um = (UserMedal*)malloc(sizeof(UserMedal));
    if (!new_um) return -4;
    new_um->id = get_next_usermedal_id();
    new_um->student_id = student_id;
    new_um->medal_id = medal_id;
    new_um->obtain_time = time(NULL);
    new_um->next = NULL;
    if (!usermedal_head) usermedal_head = new_um;
    else {
        UserMedal* tail = usermedal_head;
        while (tail->next) tail = tail->next;
        tail->next = new_um;
    }
    return 1;
}

void list_user_medals(int student_id) {
    printf("学生 %d 获得的勋章：\n", student_id);
    UserMedal* um = usermedal_head;
    int found = 0;
    while (um) {
        if (um->student_id == student_id) {
            Medal* m = medal_head;
            while (m && m->id != um->medal_id) m = m->next;
            if (m) {
                printf("  - %s (获得时间: %s)", m->name, ctime(&um->obtain_time));
            }
            found = 1;
        }
        um = um->next;
    }
    if (!found) printf("  暂无勋章\n");
}

void check_and_award_first_enroll(int student_id) {
    int count = 0;
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == student_id) count++;   // 报名记录存在即表示已报名
        e = e->next;
    }
    if (count == 1) {
        Medal* m = medal_head;
        int medal_id = -1;
        while (m) {
            if (strcmp(m->name, "首次报名") == 0) { medal_id = m->id; break; }
            m = m->next;
        }
        if (medal_id != -1) award_medal(student_id, medal_id);
    }
}

void check_and_award_time_based(int student_id) {
    long total_seconds = 0;
    Attendance* att = attendance_head;
    while (att) {
        if (att->student_id == student_id && att->checkout_time > att->checkin_time) {
            total_seconds += (att->checkout_time - att->checkin_time);
        }
        att = att->next;
    }
    long total_hours = total_seconds / 3600;
    if (total_hours >= 5) {
        Medal* m = medal_head;
        int medal_id = -1;
        while (m) {
            if (strcmp(m->name, "时长达人") == 0) { medal_id = m->id; break; }
            m = m->next;
        }
        if (medal_id != -1) award_medal(student_id, medal_id);
    }
}

// ==================== 建议箱管理 ====================
int add_suggestion(int student_id, const char* content) {
    Suggestion* s = (Suggestion*)malloc(sizeof(Suggestion));
    if (!s) return -1;
    s->id = get_next_suggestion_id();
    s->student_id = student_id;
    strcpy(s->content, content);
    s->status = UNREAD;
    s->submit_time = time(NULL);
    s->next = NULL;
    if (!suggestion_head) suggestion_head = s;
    else {
        Suggestion* tail = suggestion_head;
        while (tail->next) tail = tail->next;
        tail->next = s;
    }
    return s->id;
}

void list_suggestions(void) {
    printf("所有建议（管理员视图）：\n");
    Suggestion* s = suggestion_head;
    if (!s) { printf("  暂无建议\n"); return; }
    while (s) {
        printf("ID:%d | 学生ID:%d | 状态:%s | 提交时间:%s内容: %s\n",
            s->id, s->student_id,
            s->status == UNREAD ? "未读" : "已读",
            ctime(&s->submit_time), s->content);
        s = s->next;
    }
}

int mark_suggestion_read(int suggestion_id) {
    Suggestion* s = suggestion_head;
    while (s) {
        if (s->id == suggestion_id) { s->status = READ; return 1; }
        s = s->next;
    }
    return -1;
}

// ==================== 自动提醒 ====================
int is_reminded(int student_id, int activity_id) {
    Reminder* r = reminder_head;
    while (r) {
        if (r->student_id == student_id && r->activity_id == activity_id) return 1;
        r = r->next;
    }
    return 0;
}

// 辅助函数：将字符串 "YYYY-MM-DD HH:MM" 转换为 time_t（简化版）
static time_t str_to_time(const char* str) {
    struct tm tm = { 0 };
    if (sscanf(str, "%d-%d-%d %d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
        &tm.tm_hour, &tm.tm_min) == 5) {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        return mktime(&tm);
    }
    return 0;
}

void check_and_remind(int student_id) {
    Student* s = find_student_by_id(student_id);
    if (!s) return;
    Enrollment* e = enrollment_head;
    time_t now = time(NULL);
    while (e) {
        if (e->student_id == student_id) {
            Activity* act = find_activity_by_id(e->activity_id);
            if (act && act->status == PUBLISHED) {
                time_t start = str_to_time(act->start_time);
                if (start > 0) {
                    double diff = difftime(start, now);
                    if (diff > 0 && diff <= s->remind_ahead) {   // 使用学生自定义阈值
                        if (!is_reminded(student_id, act->id)) {
                            printf("【提醒】学生 %d：活动 \"%s\" 将在 %.0f 小时后开始！\n",
                                student_id, act->name, diff / 3600);
                            // 输出携带物品
                            if (strlen(act->items_to_bring) > 0) {
                                printf("       请携带：%s\n", act->items_to_bring);
                            }
                            // 记录提醒
                            Reminder* r = (Reminder*)malloc(sizeof(Reminder));
                            if (r) {
                                r->id = get_next_reminder_id();
                                r->student_id = student_id;
                                r->activity_id = act->id;
                                r->reminded = 1;
                                r->next = NULL;
                                if (!reminder_head) reminder_head = r;
                                else {
                                    Reminder* tail = reminder_head;
                                    while (tail->next) tail = tail->next;
                                    tail->next = r;
                                }
                            }
                        }
                    }
                }
            }
        }
        e = e->next;
    }
}

// ==================== 签到记录辅助 ====================
int create_attendance_record(int student_id, int activity_id) {
    Attendance* a = attendance_head;
    while (a) {
        if (a->student_id == student_id && a->activity_id == activity_id) return -1;
        a = a->next;
    }
    Attendance* new_a = (Attendance*)malloc(sizeof(Attendance));
    if (!new_a) return -2;
    new_a->id = get_next_attendance_id();
    new_a->student_id = student_id;
    new_a->activity_id = activity_id;
    new_a->checkin_time = 0;
    new_a->checkout_time = 0;
    new_a->next = NULL;
    if (!attendance_head) attendance_head = new_a;
    else {
        Attendance* tail = attendance_head;
        while (tail->next) tail = tail->next;
        tail->next = new_a;
    }
    return 1;
}

int delete_attendance_record(int student_id, int activity_id) {
    Attendance* prev = NULL;
    Attendance* curr = attendance_head;
    while (curr) {
        if (curr->student_id == student_id && curr->activity_id == activity_id) {
            if (prev) prev->next = curr->next;
            else attendance_head = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

// 学生设置提醒提前量（秒）
int set_remind_ahead(int student_id, int seconds) {
    Student* s = find_student_by_id(student_id);
    if (!s) return -1;
    if (seconds < 0) return -2;
    s->remind_ahead = seconds;
    return 1;
}

// 打开高德地图导航
void open_map_navigation(const char* location) {
    if (!location || strlen(location) == 0) return;
#ifdef _WIN32
    char url[512];
    char encoded[256];
    strcpy(encoded, location);
    // 简单编码：将空格替换为+，中文等不做处理（浏览器可识别）
    for (int i = 0; encoded[i]; i++) {
        if (encoded[i] == ' ') encoded[i] = '+';
    }
    sprintf(url, "https://uri.amap.com/navigation?to=%s&mode=驾车", encoded);
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#else
    printf("仅Windows支持地图导航。\n");
#endif
}

// 活动修改后通知所有报名的学生
void remind_activity_update(int activity_id) {
    Activity* act = find_activity_by_id(activity_id);
    if (!act) return;
    Enrollment* e = enrollment_head;
    int count = 0;
    while (e) {
        if (e->activity_id == activity_id) {
            printf("【通知】活动 \"%s\" 已修改，请重新查看详情。学生ID: %d\n", act->name, e->student_id);
            // 可选：记录到提醒表，避免重复提醒
            // 这里简单打印，也可以像 check_and_remind 一样创建 Reminder 记录
            count++;
        }
        e = e->next;
    }
    if (count == 0) {
        printf("活动 \"%s\" 没有学生报名，无需提醒。\n", act->name);
    }
}