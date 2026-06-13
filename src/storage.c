#include "storage.h"

#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STORAGE_LINE_MAX 512

static void remove_line_end(char *line) {
    size_t len = strlen(line);

    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[len - 1] = '\0';
        len--;
    }
}

static char *find_unescaped_separator(char *text) {
    int escaped = 0;

    while (*text != '\0') {
        if (escaped) {
            escaped = 0;
        } else if (*text == '\\') {
            escaped = 1;
        } else if (*text == '|') {
            return text;
        }
        text++;
    }

    return 0;
}

static char *take_field(char **cursor) {
    char *field = *cursor;
    char *separator = find_unescaped_separator(*cursor);

    if (separator == 0) {
        *cursor = 0;
        return field;
    }

    *separator = '\0';
    *cursor = separator + 1;
    return field;
}

static void unescape_text(char *target, int target_size, const char *source) {
    int i = 0;
    int j = 0;

    while (source[i] != '\0' && j < target_size - 1) {
        if (source[i] == '\\' && source[i + 1] != '\0') {
            i++;
            if (source[i] == 'n' || source[i] == 'r') {
                target[j] = ' ';
            } else {
                target[j] = source[i];
            }
        } else {
            target[j] = source[i];
        }

        i++;
        j++;
    }

    target[j] = '\0';
}

static void write_escaped_text(FILE *file, const char *text) {
    int i;

    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\\' || text[i] == '|') {
            fputc('\\', file);
            fputc(text[i], file);
        } else if (text[i] == '\n') {
            fputs("\\n", file);
        } else if (text[i] == '\r') {
            fputs("\\r", file);
        } else {
            fputc(text[i], file);
        }
    }
}

static void load_task_record(char *line) {
    char *cursor = line;
    char *record_type;
    char *done_text;
    char *priority_text_value;
    char *due_date;
    char *title_text;
    char title[MAX_TITLE];

    record_type = take_field(&cursor);
    done_text = cursor == 0 ? 0 : take_field(&cursor);
    priority_text_value = cursor == 0 ? 0 : take_field(&cursor);
    due_date = cursor == 0 ? 0 : take_field(&cursor);
    title_text = cursor;

    if (record_type == 0 || strcmp(record_type, STORAGE_TASK_RECORD) != 0 ||
        done_text == 0 || priority_text_value == 0 || due_date == 0 || title_text == 0) {
        return;
    }

    unescape_text(title, sizeof(title), title_text);
    add_loaded_task(atoi(done_text), atoi(priority_text_value), due_date, title);
}

static void load_legacy_task_record(char *line) {
    char *first_separator;
    char *second_separator;
    char *third_separator;
    int done;
    int priority;
    const char *due_date;
    const char *title;

    first_separator = strchr(line, '|');
    if (first_separator == 0) {
        return;
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

void load_tasks(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[STORAGE_LINE_MAX];

    clear_tasks();

    if (file == 0) {
        return;
    }

    while (fgets(line, sizeof(line), file) != 0 && get_task_count() < MAX_TASKS) {
        remove_line_end(line);

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (strncmp(line, STORAGE_TASK_RECORD "|", strlen(STORAGE_TASK_RECORD) + 1) == 0) {
            load_task_record(line);
        } else {
            load_legacy_task_record(line);
        }
    }

    fclose(file);
}

int save_tasks(void) {
    FILE *file = fopen(DATA_FILE, "w");
    int i;

    if (file == 0) {
        return 0;
    }

    fprintf(file, "%s\n", STORAGE_FORMAT_HEADER);
    fprintf(file, "%s\n", STORAGE_FIELDS_HEADER);

    for (i = 0; i < get_task_count(); i++) {
        const Task *task = get_task(i);
        if (task != 0) {
            fprintf(file, "%s|%d|%d|%s|",
                    STORAGE_TASK_RECORD,
                    task->done,
                    task->priority,
                    task->due_date);
            write_escaped_text(file, task->title);
            fputc('\n', file);
        }
    }

    fclose(file);
    return 1;
}
