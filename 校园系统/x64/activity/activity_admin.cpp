#include "../../head.h"
#include <ctype.h>

/*
    添加活动（默认草稿）
    输入检查：活动名称、人数上限等
*/
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
    int new_id = get_next_activity_id();   // 直接获取新ID

    Activity* node = (Activity*)malloc(sizeof(Activity));
    if (!node) return -1;
    memcpy(node, act, sizeof(Activity));
    node->id = new_id;
    node->current_count = 0;   // 当前报名人数初始化为0
    node->status = DRAFT;       // 设置默认状态为草稿
    node->next = activity_head; // 头插法！新来的下一个一定是第一个，所以next指向当前的链表头
    activity_head = node;       // 链表头永远指向最新添加的节点

    // 保存成功返回1，失败返回-1，0 表示无数据或未找到文件
    if (save_activities(ACTIVITIES_FILE) != 1) {
        // 如果保存失败，回退链表到之前的状态（删除新添加的节点）
        Activity* tmp = activity_head;
        activity_head = tmp->next;
        free(tmp);
        return -1;
    }

    return new_id;
}

/*
    修改活动：根据 act->id 查找已有活动并更新其可修改字段。
    返回 1 表示成功，-1 表示失败（未找到/验证失败/保存失败）。
*/
int update_activity(Activity* act) {
    if (!act) return -1;

    // 在链表中查找已有活动
    Activity* p = find_activity_by_id(act->id);
    if (!p) return -1; // 未找到

    // 基本校验
    // 这里的前半段判断是说：在交互页面，我们询问用户是否更改人数上限，如果不更改，将人数上限设置为0，这里就不会返回-1
    // 后半段：不让修改后的人数上限小于当前报名人数，用户不能改当前报名人数，只能在学生层改
    if (act->max_participants > 0 && act->max_participants < p->current_count) return -1;

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
    strncpy(p->reward, act->reward, MAX_REWARD - 1);
    p->reward[MAX_REWARD - 1] = '\0';
    p->category_id = act->category_id;

    // 状态可以更新（例如草稿->发布），但不做额外验证 here
    p->status = act->status;

    // 持久化
    if (save_activities(ACTIVITIES_FILE) != 1) {
        return -1;
    }

    // 活动修改后通知所有报名的学生
    remind_activity_update(act->id);

    return 1;
}

/*
    下架活动：根据 activity_id 将状态设置为 TAKEN_DOWN 并持久化
    返回 1 成功，-1 失败
*/
int take_down_activity(int activity_id) {
    if (activity_id <= 0) return -1;
    Activity* p = find_activity_by_id(activity_id);
    if (!p) return -1;

    if (p->status == TAKEN_DOWN) return 1; // 已下架
    p->status = TAKEN_DOWN;
    return (save_activities(ACTIVITIES_FILE) == 1) ? 1 : -1;
}

/*
    发布草稿活动：将活动状态从 DRAFT 变为 PUBLISHED 并持久化
    返回 1 成功，-1 失败（未找到/验证失败/保存失败）
*/
int publish_activity(int activity_id) {
    if (activity_id <= 0) return -1;
    Activity* p = find_activity_by_id(activity_id);
    if (!p) return -1;

    // 只有草稿可以发布
    if (p->status == PUBLISHED) return 1; // 已发布
    if (p->status != DRAFT) return -1;    // 不在草稿箱内

    // 基本校验：名称、人数上限、分类
    if (p->name[0] == '\0') return -1;
    if (p->max_participants <= 0) return -1;
    if (p->category_id != 0 && !find_category_by_id(p->category_id)) return -1;  //填了分类但是找不到分类

    p->status = PUBLISHED;
    if (save_activities(ACTIVITIES_FILE) != 1) {
        // 回滚状态
        p->status = DRAFT;
        return -1;
    }
    return 1;
}

/*
    删除活动：在确保没有相关报名记录的情况下从链表中移除并持久化
    返回 1 成功，-1 失败（未找到 / 有报名 / 保存失败）
*/
int delete_activity(int activity_id) {
    if (activity_id <= 0) return -1;

    Activity* pre = NULL;    //用来记录要删除节点的前一个节点，方便修改next指针
    Activity* cur = activity_head;
    while (cur && cur->id != activity_id) {
        pre = cur;
        cur = cur->next;
    }
    if (!cur) return -1; // 未找到

    // 如果已有报名记录，拒绝删除（避免数据不一致）
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->activity_id == activity_id) return -1;
        e = e->next;
    }

    // 从链表中移除
    if (pre) pre->next = cur->next;  //数据逻辑内键和链表的连接 区分开，虽然是删除节点的前一个，逻辑上是后添加的，但是链表逻辑里就应该由前一个指向后一个
    else activity_head = cur->next;  //如果pre不为NULL，说明要删除的节点不是头节点，直接让前一个节点的next指向当前节点的下一个节点

    free(cur);

    // 持久化
    return (save_activities(ACTIVITIES_FILE) == 1) ? 1 : -1;
}

/*
    添加活动分类：若同名分类已存在则返回其 id，否则添加新分类并返回新 id
*/
int add_category(const char* name) {
    if (!name || name[0] == '\0') return -1;   //名字不为空
    // 检查是否已存在同名分类
    Category* p = category_head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p->id;   //如果找到了同名分类，直接返回它的id，不添加新分类
        p = p->next;
    }

    //修改活动分类链表
    Category* node = (Category*)malloc(sizeof(Category));
    if (!node) return -1;
    node->id = get_next_category_id();
    strncpy(node->name, name, MAX_CATEGORY_NAME - 1);
    node->name[MAX_CATEGORY_NAME - 1] = '\0';
    node->next = NULL;

    if (!category_head) category_head = node;
    else {
        Category* tail = category_head;
        while (tail->next) tail = tail->next;
        tail->next = node;
    }

    if (save_categories(CATEGORIES_FILE) != 1) {
        // 回滚
        if (category_head == node) category_head = node->next;
        else {
            Category* cur = category_head;
            while (cur && cur->next != node) cur = cur->next;
            if (cur) cur->next = node->next;
        }
        free(node);
        return -1;
    }

    return node->id;
}

/*
    删除分类：如果有活动仍引用该分类，则拒绝删除以避免数据不一致。
    返回 1 成功，-1 失败（未找到 / 有活动引用 / 保存失败）。
*/
int delete_category(int id) {
    if (id <= 0) return -1;

    // 如果已有活动使用该分类，拒绝删除
    Activity* a = activity_head;
    while (a) {
        if (a->category_id == id) return -1;
        a = a->next;
    }

    Category* prev = NULL;
    Category* cur = category_head;
    while (cur && cur->id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur) return -1; // 未找到

    if (prev) prev->next = cur->next;
    else category_head = cur->next;

    free(cur);

    return (save_categories(CATEGORIES_FILE) == 1) ? 1 : -1;
}

/*
    按名称或描述搜索已发布的活动（不区分大小写），在控制台打印结果
*/
void search_activities(const char* keyword) {
    if (!keyword || keyword[0] == '\0') {
        printf("搜索关键字为空\n");
        return;
    }

    //把搜索关键字转换为小写，准备进行不区分大小写的比较
    char kw[MAX_DESCRIPTION];
    size_t klen = strlen(keyword);
    if (klen >= sizeof(kw)) klen = sizeof(kw) - 1;
    for (size_t i = 0; i < klen; ++i) kw[i] = (char)tolower((unsigned char)keyword[i]);
    //tolower函数将大写字母转换为小写字母，unsigned char是为了确保输入字符被正确处理，避免负值导致的问题
    kw[klen] = '\0';

    int found = 0;
    Activity* p = activity_head;
    while (p) {
        if (p->status == PUBLISHED) {
            // 准备小写的名称进行不区分大小写的比较
            char name_lower[MAX_ACTIVITY_NAME];
            size_t nlen = strlen(p->name);
            if (nlen >= sizeof(name_lower)) nlen = sizeof(name_lower) - 1;  //留一个位置给'\0'
            for (size_t i = 0; i < nlen; ++i) name_lower[i] = (char)tolower((unsigned char)p->name[i]);
            name_lower[nlen] = '\0';
            // 准备小写的描述进行不区分大小写的比较
            char desc_lower[MAX_DESCRIPTION];
            size_t dlen = strlen(p->description);
            if (dlen >= sizeof(desc_lower)) dlen = sizeof(desc_lower) - 1;
            for (size_t i = 0; i < dlen; ++i) desc_lower[i] = (char)tolower((unsigned char)p->description[i]);
            desc_lower[dlen] = '\0';
            //开始搜索，使用strstr函数检查关键字是否出现在名称或描述中
            if (strstr(name_lower, kw) != NULL || strstr(desc_lower, kw) != NULL) {
                if (!found) printf("搜索结果：\n");
                found = 1;
                printf("ID:%d | 名称:%s | 时间:%s | 地点:%s\n", p->id, p->name, p->start_time, p->location);
                printf("    报名:%d/%d | 分类ID:%d\n", p->current_count, p->max_participants, p->category_id);
                if (p->description[0]) printf("    描述: %s\n", p->description);
            }
        }
        p = p->next;
    }
    if (!found) printf("未找到匹配的已发布活动\n");
}