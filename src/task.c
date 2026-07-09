#include "task.h"

// Inserisce task_t t 
void enqueue(task_t *t) {
    t->next = NULL;
    // se la coda è completamente vuota l nuovo elemento diventa contemporaneamente sia la testa che la coda
    if (!coda_q) testa_q = coda_q = t;
    // L'attuale ultimo elemento punta al nuovo arrivato e il puntatore di coda si sposta sul nuovo elemento
    else { coda_q->next = t; coda_q = t; }
}

task_t *dequeue(void) {
    // Se la coda è vuota(Non ha testa) ritorna NULL per dire al programma che non c'è lavoro da fare
        if (!testa_q) return NULL;
        
        // Salva il puntatore all'elemento da estrarre in una variabile temporanea 't'
        task_t *t = testa_q;
        // Sposta la testa della coda sul secondo elemento della lista
        testa_q = t->next;
        //La coda è diventata vuota dopo questa estrazione
        if (!testa_q) 
            coda_q = NULL; //Azzera anche il puntatore di coda
    
        return t;
}
