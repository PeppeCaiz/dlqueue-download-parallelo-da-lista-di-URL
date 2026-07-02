#ifndef tools_H
#define tools_H

#include <stdio.h>
#include <string.h>
#include <time.h>

void log_write(FILE *flog, const char *url, const char *status);
const char *url_basename(const char *url);
int menu(const char *url);
#endif 
