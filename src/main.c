#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_TASKS 100
#define MAX_TITLE 128
#define DATE_LEN 6
#define DATA_FILE "tasks.txt"

typedef struct {
    char title[MAX_TITLE];
    char due_date[DATE_LEN];
    int done;
    int priority;
} Task;

static Task tasks[MAX_TASKS];
static int task_count = 0;

static void remove_newline(char *text) {
    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
    }
}

static void wait_for_enter(void) {
    char buffer[8];
    printf("\n按回车继续...");
    fgets(buffer, sizeof(buffer), stdin);
}

static int read_number(const char *prompt) {
    char buffer[32];
    int value;

    printf("%s", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }

    if (sscanf(buffer, "%d", &value) != 1) {
        return -1;
    }

    return value;
}

static int confirm_action(const char *prompt) {
    char answer[16];

    printf("%s", prompt);
    if (fgets(answer, sizeof(answer), stdin) == NULL) {
        return 0;
    }

    remove_newline(answer);

    if (strcmp(answer, "y") == 0 || strcmp(answer, "yes") == 0) {
        return 1;
    }

    if (strcmp(answer, "n") == 0 || strcmp(answer, "no") == 0) {
        return 0;
    }

    printf("输入无效，操作已取消。\n");
    return 0;
}

static int is_valid_due_date(const char *date) {
    int month;
    int day;

    if (strlen(date) != 5) {
        return 0;
    }

    if (date[0] < '0' || date[0] > '9' ||
        date[1] < '0' || date[1] > '9' ||
        date[2] != '-' ||
        date[3] < '0' || date[3] > '9' ||
        date[4] < '0' || date[4] > '9') {
        return 0;
    }

    month = (date[0] - '0') * 10 + (date[1] - '0');
    day = (date[3] - '0') * 10 + (date[4] - '0');

    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

static void read_due_date(char *due_date) {
    char buffer[32];

    while (1) {
        printf("请输入截止日期 (MM-DD): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            strcpy(due_date, "00-00");
            return;
        }

        remove_newline(buffer);

        if (is_valid_due_date(buffer)) {
            strcpy(due_date, buffer);
            return;
        }

        printf("日期格式无效，请使用 MM-DD。\n");
    }
}

static const char *priority_text(int priority) {
    switch (priority) {
        case 1:
            return "H";
        case 2:
            return "M";
        case 3:
            return "L";
        default:
            return "M";
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

static void load_tasks(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[MAX_TITLE + 32];

    task_count = 0;

    if (file == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL && task_count < MAX_TASKS) {
        char *first_separator;
        char *second_separator;
        char *third_separator;
        remove_newline(line);

        first_separator = strchr(line, '|');
        if (first_separator == NULL) {
            continue;
        }

        *first_separator = '\0';
        tasks[task_count].done = atoi(line);

        second_separator = strchr(first_separator + 1, '|');
        if (second_separator == NULL) {
            tasks[task_count].priority = 2;
            strcpy(tasks[task_count].due_date, "00-00");
            strncpy(tasks[task_count].title, first_separator + 1, MAX_TITLE - 1);
        } else {
            *second_separator = '\0';
            tasks[task_count].priority = atoi(first_separator + 1);
            if (tasks[task_count].priority < 1 || tasks[task_count].priority > 3) {
                tasks[task_count].priority = 2;
            }

            third_separator = strchr(second_separator + 1, '|');
            if (third_separator == NULL) {
                strcpy(tasks[task_count].due_date, "00-00");
                strncpy(tasks[task_count].title, second_separator + 1, MAX_TITLE - 1);
            } else {
                *third_separator = '\0';
                if (is_valid_due_date(second_separator + 1)) {
                    strcpy(tasks[task_count].due_date, second_separator + 1);
                } else {
                    strcpy(tasks[task_count].due_date, "00-00");
                }
                strncpy(tasks[task_count].title, third_separator + 1, MAX_TITLE - 1);
            }
        }

        tasks[task_count].title[MAX_TITLE - 1] = '\0';
        task_count++;
    }

    fclose(file);
}

static int save_tasks(void) {
    FILE *file = fopen(DATA_FILE, "w");
    int i;

    if (file == NULL) {
        printf("无法保存任务到 %s。\n", DATA_FILE);
        return 0;
    }

    for (i = 0; i < task_count; i++) {
        fprintf(file, "%d|%d|%s|%s\n", tasks[i].done, tasks[i].priority, tasks[i].due_date, tasks[i].title);
    }

    fclose(file);
    return 1;
}

static int due_date_sort_value(const char *date) {
    int month;
    int day;

    if (!is_valid_due_date(date)) {
        return 9999;
    }

    month = (date[0] - '0') * 10 + (date[1] - '0');
    day = (date[3] - '0') * 10 + (date[4] - '0');

    return month * 100 + day;
}

static int compare_task_indices(int left, int right) {
    int left_date;
    int right_date;

    if (tasks[left].priority != tasks[right].priority) {
        return tasks[left].priority - tasks[right].priority;
    }

    left_date = due_date_sort_value(tasks[left].due_date);
    right_date = due_date_sort_value(tasks[right].due_date);

    if (left_date != right_date) {
        return left_date - right_date;
    }

    return left - right;
}

static void build_sorted_indices(int sorted_indices[]) {
    int i;
    int j;
    int temp;

    for (i = 0; i < task_count; i++) {
        sorted_indices[i] = i;
    }

    for (i = 0; i < task_count - 1; i++) {
        for (j = i + 1; j < task_count; j++) {
            if (compare_task_indices(sorted_indices[i], sorted_indices[j]) > 0) {
                temp = sorted_indices[i];
                sorted_indices[i] = sorted_indices[j];
                sorted_indices[j] = temp;
            }
        }
    }
}

static void add_task(void) {
    char title[MAX_TITLE];

    if (task_count >= MAX_TASKS) {
        printf("任务列表已满。\n");
        return;
    }

    printf("请输入任务名称: ");
    if (fgets(title, sizeof(title), stdin) == NULL) {
        return;
    }

    remove_newline(title);

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
    int i;

    if (!confirm_action("确认删除这个任务吗？输入 y/yes 确认，n/no 取消: ")) {
        printf("删除已取消。\n");
        return;
    }

    for (i = task_index; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;

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
    if (fgets(title, sizeof(title), stdin) == NULL) {
        return;
    }

    remove_newline(title);

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

static void select_task_to_manage(int valid_indices[], int valid_count) {
    int number;
    int task_index;

    number = read_number("请输入要操作的任务编号，输入 0 返回: ");
    if (number == 0) {
        return;
    }

    if (number < 1 || number > task_count) {
        printf("任务编号无效。\n");
        return;
    }

    task_index = number - 1;
    if (valid_indices != NULL && !index_in_list(task_index, valid_indices, valid_count)) {
        printf("该任务编号不在当前列表中。\n");
        return;
    }

    task_action_menu(task_index);
}

static void view_tasks(void) {
    int sorted_indices[MAX_TASKS];
    if (list_tasks(sorted_indices)) {
        select_task_to_manage(NULL, 0);
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
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
        return;
    }

    remove_newline(keyword);

    if (strlen(keyword) == 0) {
        printf("搜索关键词不能为空。\n");
        return;
    }

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

    select_task_to_manage(matching_indices, matching_count);
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

int main(void) {
    int choice;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    load_tasks();

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
                wait_for_enter();
                break;
            case 3:
                search_tasks();
                wait_for_enter();
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
                return 0;
            default:
                printf("选项无效。\n");
                wait_for_enter();
                break;
        }
    }
}

