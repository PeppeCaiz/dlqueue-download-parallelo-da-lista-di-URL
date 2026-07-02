#include "tools.h"
int a=0;

void log_write(FILE *flog, const char *url, const char *status) {
    time_t t= time(NULL);
    char ts[64];
    if(strcmp(status, "\n") == 0) {
        fprintf(flog, "%s", status);
        printf("%s", status);
        fflush(flog);
        return;
    }
    else{

        struct tm *tm_info = localtime(&t);
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S ", tm_info); 
        a=a+1;
        printf("Download n %d eseguito\n", a);
        fprintf(flog, "%s %-6s %s\n", ts, status, url);
        fflush(flog);
    }
}

const char *url_basename(const char *url) {
    if (url == NULL) {
        return NULL;
    }
    // Cerchiamo l'ultima occorrenza del carattere separatore '/'
    const char *last_slash = strrchr(url, '/');
    if (last_slash != NULL) {
        return last_slash + 1;
    } 
    else {
        return url;
    }
    
}
int menu(const char *url) {
    char in;
    printf("Vuoi scaricare i file elencati in '%s'? (y/n):", url);
    in=getchar();
    switch(in){
        case 'y':
        case 'Y':
            printf("Inizio download dei file...\n");
            return 1;
        case 'n':
        case 'N':
            printf("Download annullato.\n");
            return 0;
        default:
            printf("Input non valido. Inserisci 'y' o 'n'.\n");
            return menu(url);
    }
}
 
