#ifndef READ_LINE_H
#define READ_LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 100

/* 
 * Lee una línea desde "fichero" y la guarda en *ptr sin el "\n" 
 * Si falla, devuelve -1, si no, 0
 */
int read_line_from_file(FILE *fichero, char **ptr);

#endif
