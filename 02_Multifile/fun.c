#include <stdio.h>
#include "outlib.h"

void output(char *str) {
    printf("%d: %s\n", Count++, str);
}

void usage(char *prog) {
    fprintf(stderr, "%s v%.2f: Print all arguments\n\t"
           "Usage: %s arg1 [arg2 …]\n", prog, VERSION, prog);
}

