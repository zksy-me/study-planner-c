#include "storage.h"

#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_tasks(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[MAX_TITLE + 32];

    clear_tasks();

    if (file == 0) {
        return;
    }

    while (fgets(line, sizeof(line), file) != 0 && get_task_count() < MAX_TASKS) {
        char *first_separator;
        char *second_separator;
        char *third_separator;
        int done;
        int priority;
        const char *due_date;
        const char *title;
        size_t len;

        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        first_separator = strchr(line, '|');
        if (first_separator == 0) {
            continue;
        }

        *first_separator = '\0';
        done = atoi(line);

        second_separator = strchr(first_separator + 1, '|');
        if (second_separator == 0) {
            priority = 2;
            due_date = "00-00";
            title = first_separator + 1;
        } else {
            *second_separator = '\0';
            priority = atoi(first_separator + 1);

            third_separator = strchr(second_separator + 1, '|');
            if (third_separator == 0) {
                due_date = "00-00";
                title = second_separator + 1;
            } else {
                *third_separator = '\0';
                due_date = second_separator + 1;
                title = third_separator + 1;
            }
        }

        add_loaded_task(done, priority, due_date, title);
    }

    fclose(file);
}

int save_tasks(void) {
    FILE *file = fopen(DATA_FILE, "w");
    int i;

    if (file == 0) {
        return 0;
    }

    for (i = 0; i < get_task_count(); i++) {
        const Task *task = get_task(i);
        if (task != 0) {
            fprintf(file, "%d|%d|%s|%s\n", task->done, task->priority, task->due_date, task->title);
        }
    }

    fclose(file);
    return 1;
}
