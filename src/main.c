#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define MAX_TITLE 128
#define DATA_FILE "tasks.txt"

typedef struct {
    char title[MAX_TITLE];
    int done;
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

static void load_tasks(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[MAX_TITLE + 8];

    task_count = 0;

    if (file == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL && task_count < MAX_TASKS) {
        char *separator;
        remove_newline(line);

        separator = strchr(line, '|');
        if (separator == NULL) {
            continue;
        }

        *separator = '\0';
        tasks[task_count].done = atoi(line);
        strncpy(tasks[task_count].title, separator + 1, MAX_TITLE - 1);
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
        fprintf(file, "%d|%s\n", tasks[i].done, tasks[i].title);
    }

    fclose(file);
    return 1;
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
    task_count++;

    save_tasks();
    printf("Task added.\n");
}

static void list_tasks(void) {
    int i;

    if (task_count == 0) {
        printf("No tasks yet.\n");
        return;
    }

    printf("\nTasks:\n");
    for (i = 0; i < task_count; i++) {
        printf("%d. [%c] %s\n", i + 1, tasks[i].done ? 'x' : ' ', tasks[i].title);
    }
}

static void delete_task(void) {
    int number;
    int i;

    list_tasks();
    if (task_count == 0) {
        return;
    }

    number = read_number("Enter task number to delete: ");
    if (number < 1 || number > task_count) {
        printf("Invalid task number.\n");
        return;
    }

    for (i = number - 1; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;

    save_tasks();
    printf("Task deleted.\n");
}

static void mark_done(void) {
    int number;

    list_tasks();
    if (task_count == 0) {
        return;
    }

    number = read_number("Enter task number to mark as done: ");
    if (number < 1 || number > task_count) {
        printf("Invalid task number.\n");
        return;
    }

    tasks[number - 1].done = 1;
    save_tasks();
    printf("Task marked as done.\n");
}

static void show_menu(void) {
    printf("\n==== Study Planner C ====\n");
    printf("1. Add task\n");
    printf("2. View all tasks\n");
    printf("3. Delete task\n");
    printf("4. Mark task as done\n");
    printf("5. Save tasks\n");
    printf("6. Exit\n");
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
                list_tasks();
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
                if (save_tasks()) {
                    printf("Tasks saved to %s.\n", DATA_FILE);
                }
                wait_for_enter();
                break;
            case 6:
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
