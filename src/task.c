#include "task.h"

#include <string.h>

static Task tasks[MAX_TASKS];
static int task_count = 0;

static void copy_text(char *target, int target_size, const char *source) {
    strncpy(target, source, target_size - 1);
    target[target_size - 1] = '\0';
}

int get_task_count(void) {
    return task_count;
}

const Task *get_task(int task_index) {
    if (!is_valid_task_index(task_index)) {
        return 0;
    }

    return &tasks[task_index];
}

int is_valid_task_index(int task_index) {
    return task_index >= 0 && task_index < task_count;
}

int normalize_priority(int priority) {
    if (priority < TASK_PRIORITY_HIGH || priority > TASK_PRIORITY_LOW) {
        return TASK_PRIORITY_MEDIUM;
    }

    return priority;
}

int is_valid_due_date(const char *date) {
    int month;
    int day;
    int days_in_month[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (date == 0 || strlen(date) != 5) {
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

    if (month < 1 || month > 12) {
        return 0;
    }

    return day >= 1 && day <= days_in_month[month - 1];
}

const char *priority_text(int priority) {
    switch (priority) {
        case TASK_PRIORITY_HIGH:
            return "H";
        case TASK_PRIORITY_MEDIUM:
            return "M";
        case TASK_PRIORITY_LOW:
            return "L";
        default:
            return "M";
    }
}

int add_task(const char *title, int priority, const char *due_date) {
    if (task_count >= MAX_TASKS || title == 0 || title[0] == '\0' || !is_valid_due_date(due_date)) {
        return 0;
    }

    copy_text(tasks[task_count].title, MAX_TITLE, title);
    copy_text(tasks[task_count].due_date, DATE_LEN, due_date);
    tasks[task_count].done = TASK_NOT_DONE;
    tasks[task_count].priority = normalize_priority(priority);
    task_count++;

    return 1;
}

int add_loaded_task(int done, int priority, const char *due_date, const char *title) {
    if (task_count >= MAX_TASKS || title == 0 || title[0] == '\0') {
        return 0;
    }

    copy_text(tasks[task_count].title, MAX_TITLE, title);
    if (is_valid_due_date(due_date)) {
        copy_text(tasks[task_count].due_date, DATE_LEN, due_date);
    } else {
        copy_text(tasks[task_count].due_date, DATE_LEN, "00-00");
    }
    tasks[task_count].done = done ? TASK_DONE : TASK_NOT_DONE;
    tasks[task_count].priority = normalize_priority(priority);
    task_count++;

    return 1;
}

int update_task_title(int task_index, const char *title) {
    if (!is_valid_task_index(task_index) || title == 0 || title[0] == '\0') {
        return 0;
    }

    copy_text(tasks[task_index].title, MAX_TITLE, title);
    return 1;
}

int update_task_priority(int task_index, int priority) {
    if (!is_valid_task_index(task_index)) {
        return 0;
    }

    tasks[task_index].priority = normalize_priority(priority);
    return 1;
}

int update_task_due_date(int task_index, const char *due_date) {
    if (!is_valid_task_index(task_index) || !is_valid_due_date(due_date)) {
        return 0;
    }

    copy_text(tasks[task_index].due_date, DATE_LEN, due_date);
    return 1;
}

int toggle_task_done(int task_index) {
    if (!is_valid_task_index(task_index)) {
        return 0;
    }

    tasks[task_index].done = !tasks[task_index].done;
    return 1;
}

int delete_task(int task_index) {
    int i;

    if (!is_valid_task_index(task_index)) {
        return 0;
    }

    for (i = task_index; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;

    return 1;
}

void clear_tasks(void) {
    task_count = 0;
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

void build_sorted_indices(int sorted_indices[]) {
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

int search_tasks(const char *keyword, int matching_indices[]) {
    int i;
    int task_index;
    int sorted_indices[MAX_TASKS];
    int matching_count = 0;

    if (keyword == 0 || keyword[0] == '\0') {
        return 0;
    }

    build_sorted_indices(sorted_indices);
    for (i = 0; i < task_count; i++) {
        task_index = sorted_indices[i];
        if (strstr(tasks[task_index].title, keyword) != 0) {
            matching_indices[matching_count] = task_index;
            matching_count++;
        }
    }

    return matching_count;
}

int filter_tasks_by_done(int done, int matching_indices[]) {
    int i;
    int task_index;
    int sorted_indices[MAX_TASKS];
    int matching_count = 0;

    build_sorted_indices(sorted_indices);
    for (i = 0; i < task_count; i++) {
        task_index = sorted_indices[i];
        if (tasks[task_index].done == (done ? TASK_DONE : TASK_NOT_DONE)) {
            matching_indices[matching_count] = task_index;
            matching_count++;
        }
    }

    return matching_count;
}

int is_task_index_in_list(int task_index, int valid_indices[], int valid_count) {
    int i;

    for (i = 0; i < valid_count; i++) {
        if (valid_indices[i] == task_index) {
            return 1;
        }
    }

    return 0;
}
