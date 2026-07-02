#include "task.h"


void enqueue(task_t *t) {
    t->next = NULL;
    if (!coda_q) testa_q = coda_q = t;
    else { coda_q->next = t; coda_q = t; }
}

task_t *dequeue(void) {
        if (!testa_q) return NULL;
        task_t *t = testa_q;
        testa_q = t->next;
        if (!testa_q) coda_q = NULL;
            return t;
}
