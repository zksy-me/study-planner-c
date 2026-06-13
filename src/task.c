#include "task.h"

#include <string.h>

Task tasks[MAX_TASKS];
int task_count = 0;

int is_valid_due_date(const char *date) {
    int month;
    int day;
    int days_in_month[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

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

    if (month < 1 || month > 12) {
        return 0;
    }

    return day >= 1 && day <= days_in_month[month - 1];
}

const char *priority_text(int priority) {
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

void remove_task_at(int task_index) {
    int i;

    for (i = task_index; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;
}
