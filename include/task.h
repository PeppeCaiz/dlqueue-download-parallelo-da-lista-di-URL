#ifndef TASKS_H
#define TASKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct task {
    char *url;
    int tentativi;
    struct task *next;
} task_t;

extern task_t *testa_q , *coda_q ;

void enqueue(task_t *t);
task_t *dequeue(void);


#endif