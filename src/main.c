#define _POSIX_C_SOURCE 200809L

#include "task.h"
#include "task_handler.h"
#include "tools.h"


#define URLPATH "url_log/lista_url.txt"
#define LOGPATH "url_log/download.log"
#define DESTDIR "downloads"
#define MAXLINE 1024
#define MAX_WORKERS 5
#define MAX_TENTATIVI 3


static volatile sig_atomic_t flag_sigchld = 0;

static void sigchld_handler(int signo) { 
    (void)signo; 
    flag_sigchld = 1; 
}

int main(void) {

    FILE *furl = fopen(URLPATH, "r");
    if (!furl) {
        perror("fopen url");
        return 1;
    }
    FILE *flog = fopen(LOGPATH, "a");
    if (!flog) {
    perror("fopen log");
        fclose(furl);
        return 1;
    }
    // Legge gli URL dal file e li inserisce nella coda di task 
    char buf[MAXLINE];
    while (fgets(buf, MAXLINE, furl)) {
        size_t n = strlen(buf);
        while (n && (buf[n-1] == '\n' || buf[n-1] == '\r')) buf[--n] = '\0';
            if (n == 0) continue;
        
        task_t *t = malloc(sizeof(*t));
        if (!t) {
            perror("malloc");
            return 1;
        }
        t->url = strdup(buf);
        t->tentativi = 0;
        t->next= NULL;
        enqueue(t);
    }
    fclose(furl);

    // Imposta il gestore del segnale SIGCHLD per rilevare la terminazione dei figli
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    // Maschera SIGCHLD per evitare una race condition tra il controllo del flag e la sigsuspend
    sigset_t mask_chld, mask_orig;
    sigemptyset(&mask_chld);
    sigaddset(&mask_chld, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask_chld, &mask_orig);
    int n_attivi = 0;
    while (testa_q || n_attivi > 0) {
        // Avvia nuovi figli finché non si raggiunge il numero massimo di worker o non ci sono più task in coda
        while (n_attivi < MAX_WORKERS && testa_q) {
            task_t *t = dequeue();
            
            const char *fname = url_basename(t->url);
            char fullpath[MAXLINE];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", DESTDIR, fname);
            
            // Controlla se il file esiste già nella directory di destinazione
            if (fname[0] != '\0' && access(fullpath, F_OK) == 0) {
                log_write(flog, t->url, "SKIP");
                free(t->url); free(t);
                continue;
            }
            
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                enqueue(t); 
                break;
            }
            if (pid == 0) {
                execlp("wget", "wget", "-q", "--timeout=3", "-P", DESTDIR, t->url, (char *)NULL);
                //non vado ad utilizzare --tries perchè non voglio bloccare il figlio in caso di fallimento,
                // voglio che torni subito al padre per poter reinserire il task in coda
                perror("execlp wget");
                exit(127);
            }
            
            slot_add(pid, t);
            n_attivi++;
        }

        if (n_attivi == 0) {
            continue;
        }
        
        // Aspetta figli terminati, usando sigsuspend per evitare busy waiting 
        while (!flag_sigchld)
            sigsuspend(&mask_orig);
        flag_sigchld = 0;
        
        //Raccoglie tutti i figli terminati e WNOHANG dice: non bloccare se non ci sono ancora figli pronti
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            task_t *t = slot_remove(pid);
            if (!t) continue;
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                // Se il figlio è terminato con successo, scrive nel log e libera la memoria 
                log_write(flog, t->url, "OK");
                free(t->url); free(t);
            }
            else {
                // Se il figlio è terminato con errore, incrementa il numero di tentativi 
                // e reinserisce il task in coda se non ha superato il limite massimo di tentativi
                t->tentativi++;
                if (t->tentativi >= MAX_TENTATIVI) {
                    log_write(flog, t->url, "FAILED");
                    free(t->url); free(t);
                }
                else {
                    enqueue(t); 
                }
            }
            n_attivi--;
        }
    }   
    sigprocmask(SIG_SETMASK, &mask_orig, NULL);
    log_write(flog, " ", "\n");
    printf("Downloads completati. Controlla il file di log '%s' per i dettagli.\n", LOGPATH);
    fclose(flog);
    return 0;
}