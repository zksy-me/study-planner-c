#include "storage.h"

#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_tasks(void) {
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
        size_t len;

        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

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

int save_tasks(void) {
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
