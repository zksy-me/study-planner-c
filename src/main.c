#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    printf("\nPress Enter to continue...");
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
        printf("Enter due date (MM-DD): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            strcpy(due_date, "00-00");
            return;
        }

        remove_newline(buffer);

        if (is_valid_due_date(buffer)) {
            strcpy(due_date, buffer);
            return;
        }

        printf("Invalid date format. Please use MM-DD.\n");
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

    printf("Priority options: 1=High, 2=Medium, 3=Low\n");
    priority = read_number("Enter priority: ");

    if (priority < 1 || priority > 3) {
        printf("Invalid priority. Medium will be used.\n");
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
        printf("Could not save tasks to %s.\n", DATA_FILE);
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
        printf("Task list is full.\n");
        return;
    }

    printf("Enter task title: ");
    if (fgets(title, sizeof(title), stdin) == NULL) {
        return;
    }

    remove_newline(title);

    if (strlen(title) == 0) {
        printf("Task title cannot be empty.\n");
        return;
    }

    strncpy(tasks[task_count].title, title, MAX_TITLE - 1);
    tasks[task_count].title[MAX_TITLE - 1] = '\0';
    tasks[task_count].done = 0;
    tasks[task_count].priority = read_priority();
    read_due_date(tasks[task_count].due_date);
    task_count++;

    save_tasks();
    printf("Task added.\n");
}

static int list_tasks(int sorted_indices[]) {
    int i;
    int task_index;

    if (task_count == 0) {
        printf("No tasks yet.\n");
        return 0;
    }

    build_sorted_indices(sorted_indices);

    printf("\nTasks:\n");
    for (i = 0; i < task_count; i++) {
        task_index = sorted_indices[i];
        printf("%d. [%c] [%s] [%s] %s\n", i + 1, tasks[task_index].done ? 'x' : ' ', priority_text(tasks[task_index].priority), tasks[task_index].due_date, tasks[task_index].title);
    }

    return 1;
}

static void update_due_date(void) {
    int number;
    int task_index;
    int sorted_indices[MAX_TASKS];

    if (!list_tasks(sorted_indices)) {
        return;
    }

    number = read_number("Enter task number to update due date: ");
    if (number < 1 || number > task_count) {
        printf("Invalid task number.\n");
        return;
    }

    task_index = sorted_indices[number - 1];
    read_due_date(tasks[task_index].due_date);
    save_tasks();
    printf("Due date updated.\n");
}

static void delete_task(void) {
    int number;
    int i;
    int task_index;
    int sorted_indices[MAX_TASKS];

    if (!list_tasks(sorted_indices)) {
        return;
    }

    number = read_number("Enter task number to delete: ");
    if (number < 1 || number > task_count) {
        printf("Invalid task number.\n");
        return;
    }

    task_index = sorted_indices[number - 1];
    for (i = task_index; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;

    save_tasks();
    printf("Task deleted.\n");
}

static void mark_done(void) {
    int number;
    int task_index;
    int sorted_indices[MAX_TASKS];

    if (!list_tasks(sorted_indices)) {
        return;
    }

    number = read_number("Enter task number to mark as done: ");
    if (number < 1 || number > task_count) {
        printf("Invalid task number.\n");
        return;
    }

    task_index = sorted_indices[number - 1];
    tasks[task_index].done = 1;
    save_tasks();
    printf("Task marked as done.\n");
}

static void view_tasks(void) {
    int sorted_indices[MAX_TASKS];
    list_tasks(sorted_indices);
}

static void show_menu(void) {
    printf("\n==== Study Planner C ====\n");
    printf("1. Add task\n");
    printf("2. View all tasks\n");
    printf("3. Delete task\n");
    printf("4. Mark task as done\n");
    printf("5. Update due date\n");
    printf("6. Save tasks\n");
    printf("7. Exit\n");
}

int main(void) {
    int choice;

    load_tasks();

    while (1) {
        show_menu();
        choice = read_number("Choose an option: ");

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
                delete_task();
                wait_for_enter();
                break;
            case 4:
                mark_done();
                wait_for_enter();
                break;
            case 5:
                update_due_date();
                wait_for_enter();
                break;
            case 6:
                if (save_tasks()) {
                    printf("Tasks saved to %s.\n", DATA_FILE);
                }
                wait_for_enter();
                break;
            case 7:
                save_tasks();
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid option.\n");
                wait_for_enter();
                break;
        }
    }
}
