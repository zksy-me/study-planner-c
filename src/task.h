#ifndef TASK_H
#define TASK_H

#define MAX_TASKS 100
#define MAX_TITLE 128
#define DATE_LEN 6

#define TASK_NOT_DONE 0
#define TASK_DONE 1

#define TASK_PRIORITY_HIGH 1
#define TASK_PRIORITY_MEDIUM 2
#define TASK_PRIORITY_LOW 3

typedef struct {
    char title[MAX_TITLE];
    char due_date[DATE_LEN];
    int done;
    int priority;
} Task;

int get_task_count(void);
const Task *get_task(int task_index);

const char *priority_text(int priority);
int normalize_priority(int priority);
int is_valid_due_date(const char *date);
int is_valid_task_index(int task_index);

int add_task(const char *title, int priority, const char *due_date);
int add_loaded_task(int done, int priority, const char *due_date, const char *title);
int update_task_title(int task_index, const char *title);
int update_task_priority(int task_index, int priority);
int update_task_due_date(int task_index, const char *due_date);
int toggle_task_done(int task_index);
int delete_task(int task_index);
void clear_tasks(void);

void build_sorted_indices(int sorted_indices[]);
int search_tasks(const char *keyword, int matching_indices[]);
int filter_tasks_by_done(int done, int matching_indices[]);
int is_task_index_in_list(int task_index, int valid_indices[], int valid_count);

#endif
