#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pipes[2];
    int bytes[1];

    pipe(pipes);

    int pid = fork();
    if (pid == 0) {
        read(pipes[0], bytes, 1);
        close(pipes[0]);
        pid = getpid();
        fprintf(1, "child %d: received ping\n", pid);
        write(pipes[1], bytes, 1);
        close(pipes[1]);
    } else {
        bytes[0] = 0;
        write(pipes[1], bytes, 1);
        close(pipes[1]);
        read(pipes[0], bytes, 1);
        close(pipes[1]);
        pid = getpid();
        fprintf(1, "parent%d: received pong\n", pid);
    }

    exit(0);
}