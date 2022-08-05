#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE_LENGTH 512

void free_mem_after_n(char *argv[], int n, int arglen) {
    int i;
    for (i = n;i < arglen; i++) {
        free(argv[i]);
    }
}

int main(int argc, char *argv[]) {
    char buf[MAX_LINE_LENGTH];
    int buf_len = 0;

    char *command;
    char *args[MAXARG];
    int argslen = 0;

    int i;
    int pid;

    if (argc - 1 > MAXARG) {
        fprintf(2, "xargs: the argument length exceed the maximum argument length %d\n", MAXARG);
        exit(1);
    }

    if (argc < 2) {
        fprintf(2, "xargs: please provide command.\n");
        exit(1);
    }

    command = argv[1];
    for (i = 1;i < argc; i++) {
        args[argslen++] = argv[i];
    }
    args[argslen] = 0;

    while(read(0, buf + buf_len++, 1)) {
        if (buf[buf_len - 1] == ' ' || buf[buf_len - 1] == '\n') {
            args[argslen++] = malloc(buf_len);
            memcpy(args[argslen - 1], buf, buf_len);
            args[argslen - 1][buf_len - 1] = 0;

            if (buf[buf_len - 1] == '\n') {
                pid = fork();
                if (pid < 0) {
                    printf("xargs: error fork.\n");
                    exit(1);
                }

                if (pid == 0) {
                    exec(command, args);
                    exit(1);
                } else {
                    wait((int *) 0);
                }
                free_mem_after_n(args, argc - 1, argslen);
                argslen = argc - 1;
            }

            buf_len = 0;
        }
    }

    exit(0);
}