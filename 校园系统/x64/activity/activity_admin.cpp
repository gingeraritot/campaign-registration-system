#include "../../head.h"

int add_activity(Activity* act) {
    /*
        输入检查
    */
	if (!act) return -1;    //空指针检查
	if (act->name[0] == '\0') return -1;   // 活动名称不能为空
	if (act->max_participants <= 0) return -1;   // 人数上限不合理

    /*
        添加操作
    */
    int max_id = 0;
    Activity* p = activity_head;     //与存储数据的链表进行连接（调取数据）
                                     //链表头在data.cpp中已经设置为空
	while (p) {              //如果链表为空，头指针为NULL，循环体不执行，max_id保持为0
		if (p->id > max_id) max_id = p->id;    //设置最大id，确保新活动的id唯一且递增
        p = p->next;
    }
    int new_id = max_id + 1;

	Activity* node = (Activity*)malloc(sizeof(Activity));  //新建节点
    if (!node) return -1;
	memcpy(node, act, sizeof(Activity));  //新知识：内存复制函数，将act的内容复制到node中
    node->id = new_id;        //更换id
	node->current_count = 0;   //当前报名人数初始化为0
	node->status = DRAFT;     //设置默认状态为草稿11111111111111111111111111111111111111111111111111111111111111111111
	node->next = activity_head;         //头插法！新来的下一个一定是第一个，所以next指向当前的链表头
	activity_head = node;            //链表头永远指向最新添加的节点

	// 保存成功返回1，失败返回-1，0 表示无数据或未找到文件
	if (save_activities(ACTIVITIES_FILE) != 1) {
        //如果保存失败，回退链表到之前的状态
        // 下面都是在（删除新添加的节点）
		Activity* tmp = activity_head;   
        activity_head = tmp->next;         
        free(tmp);
        return -1;
    }

    return new_id;
}

// 在链表中按 id 查找活动并返回指针，找不到返回 NULL
Activity* find_activity_by_id(int id) {
    Activity* q = activity_head;
    while (q) {
        if (q->id == id) return q;
        q = q->next;
    }
    return NULL;
}




// 修改活动：根据 act->id 查找已有活动并更新其可修改字段。
// 返回 1 表示成功，-1 表示失败（未找到/验证失败/保存失败）。
int update_activity(Activity* act) {
    if (!act) return -1;

    // 在链表中查找已有活动
    Activity* p = find_activity_by_id(act->id);
    if (!p) return -1; // 未找到


    // 基本校验
	if (act->max_participants > 0 && act->max_participants < p->current_count) return -1;  
    //这里的前半段判断是说：在交互页面，我们询问用户是否更改人数上限，如果不更改，将人数上限设置为0，这里就不会返回-1
    //后半段：不让修改后的人数上限小于当前报名人数，用户不能改 当前报名人数，只能在学生层改



    /*
        安全地逐字段更新（保留系统字段 id / current_count / next，这些都是用户不能改的）
    */ 

    // 字符串字段使用 strncpy 并保证以 NUL 结束
    strncpy(p->name, act->name, MAX_ACTIVITY_NAME - 1);
    p->name[MAX_ACTIVITY_NAME - 1] = '\0';
    strncpy(p->start_time, act->start_time, MAX_TIME - 1);
    p->start_time[MAX_TIME - 1] = '\0';
    p->duration = act->duration;
    strncpy(p->location, act->location, MAX_LOCATION - 1);
    p->location[MAX_LOCATION - 1] = '\0';
    p->max_participants = act->max_participants;
    strncpy(p->description, act->description, MAX_DESCRIPTION - 1);
    p->description[MAX_DESCRIPTION - 1] = '\0';
    strncpy(p->reward, act->reward, MAX_REWARD - 1); p->reward[MAX_REWARD - 1] = '\0';
    p->category_id = act->category_id;

    // 状态可以更新（例如草稿->发布），但不做额外验证 here
    p->status = act->status;

    // 持久化
    if (save_activities(ACTIVITIES_FILE) != 1) {
        return -1;
    }

    return 1;
}





// 下架活动：根据 activity_id 将状态设置为 TAKEN_DOWN 并持久化
// 返回 1 成功，-1 失败
int take_down_activity(int activity_id) {
    if (activity_id <= 0) return -1;
    Activity* p = find_activity_by_id(activity_id);
    if (!p) return -1;

    if (p->status == TAKEN_DOWN) return 1; // 已下架
    p->status = TAKEN_DOWN;
    return (save_activities(ACTIVITIES_FILE) == 1) ? 1 : -1;
}