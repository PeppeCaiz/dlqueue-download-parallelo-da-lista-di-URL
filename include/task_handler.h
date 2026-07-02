#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

#include "task.h"

// Mappa pid -> task per sapere quale URL ha scaricato ogni figlio

typedef struct slot {
    pid_t pid;
    task_t *task;
    struct slot *next;
} slot_t;

void slot_add(pid_t pid, task_t *t);
task_t *slot_remove(pid_t pid);

#endif // TASK_HANDLER_H
