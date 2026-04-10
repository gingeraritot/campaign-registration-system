#include "../../head.h"
#include <ctype.h>

int add_activity(Activity* act) {
    if (!act) return -1;
    if (act->name[0] == '\0') return -1;
    if (act->max_participants <= 0) return -1;

    int new_id = get_next_activity_id();

    Activity* node = (Activity*)malloc(sizeof(Activity));
    if (!node) return -1;
    memcpy(node, act, sizeof(Activity));
    node->id = new_id;
    node->current_count = 0;
    node->status = DRAFT;
    node->next = activity_head;
    activity_head = node;

    if (save_activities(ACTIVITIES_FILE) != 1) {
        activity_head = node->next;
        free(node);
        return -1;
    }
    return new_id;
}

int update_activity(Activity* act) {
    if (!act) return -1;
    Activity* p = find_activity_by_id(act->id);
    if (!p) return -1;
    if (act->max_participants > 0 && act->max_participants < p->current_count) return -1;

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
    p->status = act->status;

    if (save_activities(ACTIVITIES_FILE) != 1) return -1;
    remind_activity_update(act->id);
    return 1;
}

int take_down_activity(int activity_id) {
    if (activity_id <= 0) return -1;
    Activity* p = find_activity_by_id(activity_id);
    if (!p) return -1;
    if (p->status == TAKEN_DOWN) return 1;
    p->status = TAKEN_DOWN;
    return (save_activities(ACTIVITIES_FILE) == 1) ? 1 : -1;
}

int publish_activity(int activity_id) {
    if (activity_id <= 0) return -1;
    Activity* p = find_activity_by_id(activity_id);
    if (!p) return -1;
    if (p->status == PUBLISHED) return 1;
    if (p->status != DRAFT) return -1;
    if (p->name[0] == '\0') return -1;
    if (p->max_participants <= 0) return -1;
    if (p->category_id != 0 && !find_category_by_id(p->category_id)) return -1;
    p->status = PUBLISHED;
    if (save_activities(ACTIVITIES_FILE) != 1) {
        p->status = DRAFT;
        return -1;
    }
    expire_activities();  // 新增
    return 1;
}

int delete_activity(int activity_id) {
    if (activity_id <= 0) return -1;
    Activity* pre = NULL;
    Activity* cur = activity_head;
    while (cur && cur->id != activity_id) {
        pre = cur;
        cur = cur->next;
    }
    if (!cur) return -1;
    Enrollment* e = enrollment_head;
    while (e) {
        if (e->activity_id == activity_id) return -1;
        e = e->next;
    }
    if (pre) pre->next = cur->next;
    else activity_head = cur->next;
    free(cur);
    return (save_activities(ACTIVITIES_FILE) == 1) ? 1 : -1;
}

int add_category(const char* name) {
    if (!name || name[0] == '\0') return -1;
    Category* p = category_head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p->id;
        p = p->next;
    }
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

int delete_category(int id) {
    if (id <= 0) return -1;
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
    if (!cur) return -1;
    if (prev) prev->next = cur->next;
    else category_head = cur->next;
    free(cur);
    return (save_categories(CATEGORIES_FILE) == 1) ? 1 : -1;
}

void search_activities(const char* keyword) {
    if (!keyword || keyword[0] == '\0') {
        printf("搜索关键字为空\n");
        return;
    }
    char kw[MAX_DESCRIPTION];
    size_t klen = strlen(keyword);
    if (klen >= sizeof(kw)) klen = sizeof(kw) - 1;
    for (size_t i = 0; i < klen; ++i) kw[i] = (char)tolower((unsigned char)keyword[i]);
    kw[klen] = '\0';
    int found = 0;
    Activity* p = activity_head;
    while (p) {
        if (p->status == PUBLISHED) {
            char name_lower[MAX_ACTIVITY_NAME];
            size_t nlen = strlen(p->name);
            if (nlen >= sizeof(name_lower)) nlen = sizeof(name_lower) - 1;
            for (size_t i = 0; i < nlen; ++i) name_lower[i] = (char)tolower((unsigned char)p->name[i]);
            name_lower[nlen] = '\0';
            char desc_lower[MAX_DESCRIPTION];
            size_t dlen = strlen(p->description);
            if (dlen >= sizeof(desc_lower)) dlen = sizeof(desc_lower) - 1;
            for (size_t i = 0; i < dlen; ++i) desc_lower[i] = (char)tolower((unsigned char)p->description[i]);
            desc_lower[dlen] = '\0';
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