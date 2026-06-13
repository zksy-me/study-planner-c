#ifndef TASK_H
#define TASK_H

#define MAX_TASKS 100
#define MAX_TITLE 128
#define DATE_LEN 6

typedef struct {
    char title[MAX_TITLE];
    char due_date[DATE_LEN];
    int done;
    int priority;
} Task;

extern Task tasks[MAX_TASKS];
extern int task_count;

const char *priority_text(int priority);
int is_valid_due_date(const char *date);
void build_sorted_indices(int sorted_indices[]);
void remove_task_at(int task_index);

#endif
