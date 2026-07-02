
#include "task_handler.h"

task_t *testa_q = NULL, *coda_q = NULL;
slot_t *attivi = NULL;


void slot_add(pid_t pid, task_t *t) {
    slot_t *s = malloc(sizeof(*s));
    if (!s) {
        perror("malloc");
        exit(1);
    }
    s->pid = pid; s->task = t; s->next = attivi; attivi = s;
}

task_t *slot_remove(pid_t pid) {
    slot_t **pp = &attivi;
    while (*pp) {
        if ((*pp)->pid == pid) {
            slot_t *s= *pp;
            task_t *t= s->task;
            *pp = s->next;
            free(s);
            return t;
        }
        pp = &(*pp)->next;
    }
    return NULL;
}