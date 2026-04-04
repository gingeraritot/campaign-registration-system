#include "head.h"
#include <stdio.h>

// 同学A未实现的函数
void list_activities(void) { printf("待实现: list_activities\n"); }
void list_activities_by_category(int cid) { printf("待实现: list_activities_by_category\n"); }
void list_drafts(void) { printf("待实现: list_drafts\n"); }
void search_activities(const char* kw) { printf("待实现: search_activities\n"); }
void sort_activities_by_enrollment(void) { printf("待实现: sort_activities_by_enrollment\n"); }
void list_categories(void) { printf("待实现: list_categories\n"); }
int add_activity(Activity* a) { printf("待实现: add_activity\n"); return -1; }
int publish_activity(int id) { printf("待实现: publish_activity\n"); return -1; }
int take_down_activity(int id) { printf("待实现: take_down_activity\n"); return -1; }
int update_activity(Activity* a) { printf("待实现: update_activity\n"); return -1; }
int delete_activity(int id) { printf("待实现: delete_activity\n"); return -1; }
int add_category(const char* name) { printf("待实现: add_category\n"); return -1; }
int delete_category(int id) { printf("待实现: delete_category\n"); return -1; }

// 同学B未实现的函数
int enroll_student(int sid, int aid) { printf("待实现: enroll_student\n"); return -1; }
int cancel_enrollment(int sid, int aid) { printf("待实现: cancel_enrollment\n"); return -1; }
int check_time_conflict(int sid, int aid) { printf("待实现: check_time_conflict\n"); return 0; }
void list_enrollments_for_activity(int aid) { printf("待实现: list_enrollments_for_activity\n"); }
int export_enrollment_list(int aid, const char* fname) { printf("待实现: export_enrollment_list\n"); return -1; }
int checkin(int sid, int aid) { printf("待实现: checkin\n"); return -1; }
int checkout(int sid, int aid) { printf("待实现: checkout\n"); return -1; }
int is_checked_in(int sid, int aid) { printf("待实现: is_checked_in\n"); return 0; }
int is_checked_out(int sid, int aid) { printf("待实现: is_checked_out\n"); return 0; }
void list_activity_attendance(int aid) { printf("待实现: list_activity_attendance\n"); }
void list_my_attendance(int sid) { printf("待实现: list_my_attendance\n"); }