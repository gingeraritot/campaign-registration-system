#include "head.h"
#include <stdio.h>

// 未实现的活动管理显示函数
void list_activities(void) { printf("待实现: list_activities\n"); }
void list_activities_by_category(int cid) { printf("待实现: list_activities_by_category\n"); }
void list_drafts(void) { printf("待实现: list_drafts\n"); }
void sort_activities_by_enrollment(void) { printf("待实现: sort_activities_by_enrollment\n"); }
void list_categories(void) { printf("待实现: list_categories\n"); }

// 报名管理函数实现（简化）
static time_t parse_time(const char* str) {
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

int check_time_conflict(int sid, int aid) {
    if (sid <= 0 || aid <= 0) return 0;
    Activity* newAct = find_activity_by_id(aid);
    if (!newAct) return 0;
    time_t s1 = parse_time(newAct->start_time);
    time_t e1 = s1 + (time_t)newAct->duration * 3600;

    Enrollment* e = enrollment_head;
    while (e) {
        if (e->student_id == sid) {
            Activity* a2 = find_activity_by_id(e->activity_id);
            if (a2) {
                time_t s2 = parse_time(a2->start_time);
                time_t e2 = s2 + (time_t)a2->duration * 3600;
                if (s1 > 0 && s2 > 0) {
                    if (s1 < e2 && s2 < e1) return 1; // 冲突
                }
            }
        }
        e = e->next;
    }
    return 0;
}

void list_enrollments_for_activity(int aid) {
    if (aid <= 0) { printf("无效的活动ID\n"); return; }
    printf("报名列表(Activity ID=%d):\n", aid);
    Enrollment* e = enrollment_head;
    int found = 0;
    while (e) {
        if (e->activity_id == aid) {
            printf(" 学生ID:%d 名称:%s 电话:%s 时间:%s\n", e->student_id, e->student_name, e->student_phone, ctime(&e->enroll_time));
            found = 1;
        }
        e = e->next;
    }
    if (!found) printf("  暂无报名\n");
}

int export_enrollment_list(int aid, const char* fname) {
    if (aid <= 0 || !fname) return -1;
    FILE* fp = fopen(fname, "w");
    if (!fp) return -1;
    // 写表头（不包含 enroll_time 字段）
    fprintf(fp, "student_id,student_name,phone,checked_in,checkin_time,checkout_time\n");
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->activity_id == aid) {
            Student* s = find_student_by_id(e->student_id);
            const char* sid = s ? s->student_id : "";
            const char* name = e->student_name;
            const char* phone = e->student_phone;
            // 查找签到记录
            int checked = 0;
            char checkinBuf[64] = "";
            char checkoutBuf[64] = "";
            Attendance* aRec = attendance_head;
            while (aRec) {
                if (aRec->student_id == e->student_id && aRec->activity_id == aid) {
                    if (aRec->checkin_time > 0) {
                        checked = 1;
                        if (localtime_s(&tm, &aRec->checkin_time) == 0) strftime(checkinBuf, sizeof(checkinBuf), "%Y-%m-%d %H:%M", &tm);
                    }
                    if (aRec->checkout_time > 0) {
                        if (localtime_s(&tm, &aRec->checkout_time) == 0) strftime(checkoutBuf, sizeof(checkoutBuf), "%Y-%m-%d %H:%M", &tm);
                    }
                    break;
                }
                aRec = aRec->next;
            }
            fprintf(fp, "%s,%s,%s,%s,%d,%s,%s\n", sid, name, phone, timeBuf, checked, checkinBuf, checkoutBuf);
        }
        e = e->next;
    }
    fclose(fp);
    return 0;
}

// 未实现的签到签退查询函数
int is_checked_in(int sid, int aid) { printf("待实现: is_checked_in\n"); return 0; }
int is_checked_out(int sid, int aid) { printf("待实现: is_checked_out\n"); return 0; }
void list_activity_attendance(int aid) { printf("待实现: list_activity_attendance\n"); }
void list_my_attendance(int sid) { printf("待实现: list_my_attendance\n"); }