#include "head.h"
#include <stdio.h>

// 未实现的活动管理显示函数
void list_activities(void) { printf("待实现: list_activities\n"); }
void list_activities_by_category(int cid) { printf("待实现: list_activities_by_category\n"); }
void list_drafts(void) { printf("待实现: list_drafts\n"); }
void sort_activities_by_enrollment(void) { printf("待实现: sort_activities_by_enrollment\n"); }
void list_categories(void) { printf("待实现: list_categories\n"); }

// 未实现的报名管理函数
int check_time_conflict(int sid, int aid) { printf("待实现: check_time_conflict\n"); return 0; }
void list_enrollments_for_activity(int aid) { printf("待实现: list_enrollments_for_activity\n"); }
int export_enrollment_list(int aid, const char* fname) { printf("待实现: export_enrollment_list\n"); return -1; }

// 未实现的签到签退查询函数
int is_checked_in(int sid, int aid) { printf("待实现: is_checked_in\n"); return 0; }
int is_checked_out(int sid, int aid) { printf("待实现: is_checked_out\n"); return 0; }
void list_activity_attendance(int aid) { printf("待实现: list_activity_attendance\n"); }
void list_my_attendance(int sid) { printf("待实现: list_my_attendance\n"); }