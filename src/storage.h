#ifndef STORAGE_H
#define STORAGE_H

#define DATA_FILE "tasks.txt"
#define STORAGE_SCHEMA_VERSION 2
#define STORAGE_FORMAT_HEADER "# study-planner-c tasks v2"
#define STORAGE_FIELDS_HEADER "# fields: record_type|done|priority|due_date|title"
#define STORAGE_TASK_RECORD "TASK"

void load_tasks(void);
int save_tasks(void);

#endif
