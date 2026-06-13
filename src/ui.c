#include "ui.h"

#include "storage.h"
#include "task.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void flush_line(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int remove_newline(char *text) {
    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
        return 1;
    }

    return 0;
}

static int read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }

    if (!remove_newline(buffer)) {
        flush_line();
    }

    return 1;
}

static void wait_for_enter(void) {
    char buffer[8];
    printf("\n按回车继续...");
    read_line(buffer, sizeof(buffer));
}

static int read_number(const char *prompt) {
    char buffer[32];
    char *end;
    long value;

    printf("%s", prompt);
    if (!read_line(buffer, sizeof(buffer))) {
        return -1;
    }

    errno = 0;
    value = strtol(buffer, &end, 10);
    if (end == buffer) {
        return -1;
    }

    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            return -1;
        }
        end++;
    }

    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return -1;
    }

    return (int)value;
}

static int confirm_action(const char *prompt) {
    char answer[16];

    printf("%s", prompt);
    if (!read_line(answer, sizeof(answer))) {
        return 0;
    }

    if (strcmp(answer, "y") == 0 || strcmp(answer, "yes") == 0) {
        return 1;
    }

    if (strcmp(answer, "n") == 0 || strcmp(answer, "no") == 0) {
        return 0;
    }

    printf("输入无效，操作已取消。\n");
    return 0;
}

static void read_due_date(char *due_date) {
    char buffer[32];

    while (1) {
        printf("请输入截止日期 (MM-DD): ");
        if (!read_line(buffer, sizeof(buffer))) {
            strcpy(due_date, "00-00");
            return;
        }

        if (is_valid_due_date(buffer)) {
            strcpy(due_date, buffer);
            return;
        }

        printf("日期格式无效，请使用 MM-DD。\n");
    }
}

static int read_priority(void) {
    int priority;

    printf("优先级选项：1=高, 2=中, 3=低\n");
    priority = read_number("请输入优先级: ");

    if (priority < 1 || priority > 3) {
        printf("优先级无效，将使用中优先级。\n");
        return 2;
    }

    return priority;
}

static void add_task(void) {
    char title[MAX_TITLE];

    if (task_count >= MAX_TASKS) {
        printf("任务列表已满。\n");
        return;
    }

    printf("请输入任务名称: ");
    if (!read_line(title, sizeof(title))) {
        return;
    }

    if (strlen(title) == 0) {
        printf("任务名称不能为空。\n");
        return;
    }

    strncpy(tasks[task_count].title, title, MAX_TITLE - 1);
    tasks[task_count].title[MAX_TITLE - 1] = '\0';
    tasks[task_count].done = 0;
    tasks[task_count].priority = read_priority();
    read_due_date(tasks[task_count].due_date);
    task_count++;

    save_tasks();
    printf("任务已添加。\n");
}

static int list_tasks(int sorted_indices[]) {
    int i;
    int task_index;

    if (task_count == 0) {
        printf("还没有任务。\n");
        return 0;
    }

    build_sorted_indices(sorted_indices);

    printf("\n任务列表:\n");
    for (i = 0; i < task_count; i++) {
        task_index = sorted_indices[i];
        printf("%d. [%c] [%s] [%s] %s\n", task_index + 1, tasks[task_index].done ? 'x' : ' ', priority_text(tasks[task_index].priority), tasks[task_index].due_date, tasks[task_index].title);
    }

    return 1;
}

static void delete_task_at(int task_index) {
    if (!confirm_action("确认删除这个任务吗？输入 y/yes 确认，n/no 取消: ")) {
        printf("删除已取消。\n");
        return;
    }

    remove_task_at(task_index);
    save_tasks();
    printf("任务已删除。\n");
}

static void clear_all_tasks(void) {
    if (task_count == 0) {
        printf("还没有任务。\n");
        return;
    }

    if (!confirm_action("确认清空全部任务吗？输入 y/yes 确认，n/no 取消: ")) {
        printf("清空已取消。\n");
        return;
    }

    task_count = 0;
    save_tasks();
    printf("全部任务已清空。\n");
}

static void print_task(int task_index) {
    printf("%d. [%c] [%s] [%s] %s\n", task_index + 1, tasks[task_index].done ? 'x' : ' ', priority_text(tasks[task_index].priority), tasks[task_index].due_date, tasks[task_index].title);
}

static void edit_task_title(int task_index) {
    char title[MAX_TITLE];

    printf("请输入新的任务名称: ");
    if (!read_line(title, sizeof(title))) {
        return;
    }

    if (strlen(title) == 0) {
        printf("任务名称不能为空。\n");
        return;
    }

    strncpy(tasks[task_index].title, title, MAX_TITLE - 1);
    tasks[task_index].title[MAX_TITLE - 1] = '\0';
    save_tasks();
    printf("任务名称已更新。\n");
}

static void task_action_menu(int task_index) {
    int choice;

    while (task_index >= 0 && task_index < task_count) {
        printf("\n任务:\n");
        print_task(task_index);
        printf("\n");
        printf("1. 标记完成 / 取消完成\n");
        printf("2. 修改任务名称\n");
        printf("3. 修改优先级\n");
        printf("4. 修改截止日期\n");
        printf("5. 删除任务\n");
        printf("6. 返回\n");

        choice = read_number("请选择操作: ");
        printf("\n");

        switch (choice) {
            case 1:
                tasks[task_index].done = !tasks[task_index].done;
                save_tasks();
                printf("任务状态已更新。\n");
                break;
            case 2:
                edit_task_title(task_index);
                break;
            case 3:
                tasks[task_index].priority = read_priority();
                save_tasks();
                printf("任务优先级已更新。\n");
                break;
            case 4:
                read_due_date(tasks[task_index].due_date);
                save_tasks();
                printf("截止日期已更新。\n");
                break;
            case 5:
                delete_task_at(task_index);
                return;
            case 6:
                return;
            default:
                printf("选项无效。\n");
                break;
        }
    }
}

static int index_in_list(int task_index, int valid_indices[], int valid_count) {
    int i;

    for (i = 0; i < valid_count; i++) {
        if (valid_indices[i] == task_index) {
            return 1;
        }
    }

    return 0;
}

static int select_task_to_manage(int valid_indices[], int valid_count) {
    int number;
    int task_index;

    number = read_number("请输入要操作的任务编号，输入 0 返回: ");
    if (number == 0) {
        return 0;
    }

    if (number < 1 || number > task_count) {
        printf("任务编号无效。\n");
        return 1;
    }

    task_index = number - 1;
    if (valid_indices != NULL && !index_in_list(task_index, valid_indices, valid_count)) {
        printf("该任务编号不在当前列表中。\n");
        return 1;
    }

    task_action_menu(task_index);
    return 1;
}

static void view_tasks(void) {
    int sorted_indices[MAX_TASKS];

    while (1) {
        if (!list_tasks(sorted_indices)) {
            return;
        }

        if (!select_task_to_manage(NULL, 0)) {
            return;
        }
    }
}

static void search_tasks(void) {
    char keyword[MAX_TITLE];
    int i;
    int task_index;
    int found = 0;
    int sorted_indices[MAX_TASKS];
    int matching_indices[MAX_TASKS];
    int matching_count = 0;

    if (task_count == 0) {
        printf("还没有任务。\n");
        return;
    }

    printf("请输入搜索关键词: ");
    if (!read_line(keyword, sizeof(keyword))) {
        return;
    }

    if (strlen(keyword) == 0) {
        printf("搜索关键词不能为空。\n");
        return;
    }

search_again:
    found = 0;
    matching_count = 0;
    build_sorted_indices(sorted_indices);

    printf("\n搜索结果:\n");
    for (i = 0; i < task_count; i++) {
        task_index = sorted_indices[i];
        if (strstr(tasks[task_index].title, keyword) != NULL) {
            printf("%d. [%c] [%s] [%s] %s\n", task_index + 1, tasks[task_index].done ? 'x' : ' ', priority_text(tasks[task_index].priority), tasks[task_index].due_date, tasks[task_index].title);
            matching_indices[matching_count] = task_index;
            matching_count++;
            found = 1;
        }
    }

    if (!found) {
        printf("没有找到匹配的任务。\n");
        return;
    }

    if (!select_task_to_manage(matching_indices, matching_count)) {
        return;
    }
    goto search_again;
}

static void show_menu(void) {
    printf("\n==== 学习任务计划 ====\n");
    printf("1. 添加任务\n");
    printf("2. 查看所有任务\n");
    printf("3. 搜索任务\n");
    printf("4. 清空全部任务\n");
    printf("5. 保存任务\n");
    printf("6. 退出\n");
}

void run_app(void) {
    int choice;

    while (1) {
        show_menu();
        choice = read_number("请选择操作: ");

        printf("\n");

        switch (choice) {
            case 1:
                add_task();
                wait_for_enter();
                break;
            case 2:
                view_tasks();
                break;
            case 3:
                search_tasks();
                break;
            case 4:
                clear_all_tasks();
                wait_for_enter();
                break;
            case 5:
                if (save_tasks()) {
                    printf("任务已保存到 %s。\n", DATA_FILE);
                }
                wait_for_enter();
                break;
            case 6:
                save_tasks();
                printf("再见！\n");
                return;
            default:
                printf("选项无效。\n");
                wait_for_enter();
                break;
        }
    }
}
