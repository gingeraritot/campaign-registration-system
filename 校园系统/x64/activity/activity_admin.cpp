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

	return new_id;   //返回id就是为了告知调用者新活动的唯一标识符，以便后续操作（如修改、删除等）能够准确定位到该活动
                    //就是说这里需要写一个交互
}