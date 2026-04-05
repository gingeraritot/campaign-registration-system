#include "head.h"

int delete_attendance_record(int student_id, int activity_id);

//活动报名（包含时间冲突检查、人数上限检查、活动状态检查等）
int enroll_student(int student_id, int activity_id) {
	if (student_id <= 0 || activity_id <= 0) return -1;
	Student* s = find_student_by_id(student_id);
	Activity* a = find_activity_by_id(activity_id);
	if (!s || !a) return -1; // 学生或活动不存在
	if (a->status != PUBLISHED) return -1; // 活动未发布
	if (a->current_count >= a->max_participants) return -1; // 人数上限检查

	//时间冲突和是否已报名检查
	Enrollment* e = enrollment_head;
	while (e) {
		if (e->student_id == student_id) {
			Activity* other = find_activity_by_id(e->activity_id);
			if (e->activity_id == activity_id) {
				return -1;
			}
			if (other && strcmp(other->start_time, a->start_time) == 0) {
				return -1;
			}//待修复的时间冲突检查：目前只检查了开始时间相同的情况，实际应该检查活动时间段是否有重叠，这里简化了逻辑，后续可以改进为更全面的时间冲突检查
		}
		e = e->next;
	}

	// 创建报名记录
	Enrollment* new_enrollment = (Enrollment*)malloc(sizeof(Enrollment));
	if (!new_enrollment) return -1;
	new_enrollment->id = get_next_enrollment_id();
	new_enrollment->activity_id = activity_id;
	new_enrollment->student_id = student_id;
	new_enrollment->enroll_time = time(NULL);
	new_enrollment->check_status = NOT_CHECKED;
	new_enrollment->next = NULL;
	//	更新活动人数
	a->current_count++;
	//创建签到记录
	create_attendance_record(student_id, activity_id);
	//检查首次报名勋章
	check_and_award_first_enroll(student_id);

	return 0; // 成功返回 0
}




//退报活动
int cancel_enrollment (int student_id, int activity_id) {
	if (student_id <= 0 || activity_id <= 0) return -1;
	Student* s = find_student_by_id(student_id);
	Activity* a = find_activity_by_id(activity_id);
	if (!s || !a) return -1; // 学生或活动不存在

	Enrollment* prev_e = NULL;
	Enrollment* cur_e = enrollment_head;
	while (cur_e) {
		if (cur_e->student_id == student_id && cur_e->activity_id == activity_id) {
			// 删除报名记录
			if (prev_e) prev_e->next = cur_e->next;
			else enrollment_head = cur_e->next;
			free(cur_e);
			a->current_count--; // 更新活动人数

			// 删除对应的签到记录
			delete_attendance_record(student_id, activity_id);

			return 0; // 成功0
		}
		prev_e = cur_e;
		cur_e = cur_e->next;
	}
	return -1; // 未找到报名记录
}




//签到（同步更新报名记录和签到记录）
int checkin(int student_id, int activity_id) {
	if (student_id <= 0 || activity_id <= 0) return -1;
	Student* s = find_student_by_id(student_id);
	Activity* a = find_activity_by_id(activity_id);
	if (!s || !a) return -1; // 学生或活动不存在

	// 查找报名记录，确保学生已报名
	Enrollment* e = enrollment_head;
	while (e) {
		if (e->student_id == student_id && e->activity_id == activity_id) {
			e->check_status = CHECKED; // 更新报名记录的签到状态
			break;
		}
		e = e->next;
	}
	if (e == NULL) {
		return -1; // 没找到人，返回错误
	}

	// 查找对应的签到记录
	Attendance* a_record = attendance_head;
	while (a_record) {
		if (a_record->student_id == student_id && a_record->activity_id == activity_id) {
			if (a_record->checkin_time != 0) {
				return -1; // 已签到过
			}
			a_record->checkin_time = time(NULL); // 设置签到时间戳
			// 2. 重置签退时间
			// 逻辑解释：学生既然刚刚签到（进场），那他肯定还没有签退（出场）。
			// 为了防止上次活动的残留数据（比如复用节点的情况），必须手动清零。
			a_record->checkout_time = 0;

			return 0; // 成功
		}
		a_record = a_record->next;
	}
	return -1; // 未找到签到记录（理论上不应该发生，因为报名时会创建签到记录）
}




//签退（同步更新签退并触发时长勋章检查）
int checkout(int student_id, int activity_id) {
	if (student_id <= 0 || activity_id <= 0) return -1;
	Student* s = find_student_by_id(student_id);
	Activity* a = find_activity_by_id(activity_id);
	if (!s || !a) return -1; // 学生或活动不存在

	// 查找对应的签到记录
	Attendance* a_record = attendance_head;
	while (a_record) {
		if (a_record->student_id == student_id && a_record->activity_id == activity_id) {
			if (a_record->checkin_time == 0) {
				return -1; 
			}
			if (a_record->checkout_time != 0) {
				return -1;
			}
			a_record->checkout_time = time(NULL); // 设置签退时间戳
			// 签退后检查时长勋章（计时）
			check_and_award_time_based(student_id);
			return 0; // 成功
		}
		a_record = a_record->next;
	}
	return -1; // 未找到签到记录（理论上不应该发生，因为报名时会创建签到记录）
}



// 学生查看已报名活动列表
int view_enrolled_activities(int student_id) {
	Student* s = find_student_by_id(student_id);
	if (!s) return -1;
	printf("已报名活动列表：\n");
	Enrollment* e = enrollment_head;
	while (e) {
		if (e->student_id == student_id) {
			Activity* a = find_activity_by_id(e->activity_id);
			if (a) {
				printf("活动ID: %d, 活动名称: %s, 报名时间: %s\n", a->id, a->name, ctime(&e->enroll_time));
			}
		}
		e = e->next;
	}
	return 0; // 成功
}