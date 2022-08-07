#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int mask;
    if (argc < 3) {
        fprintf(2, "trace: trace <bitmask> [commands...]\n");
        exit(1);
    }

    mask = atoi(argv[1]);
    if (mask < 0) {
        fprintf(2, "trace: bit mask is not valid\n");
        exit(1);
    }
    trace(mask);
    exec(argv[2], &argv[2]);
    exit(0);
}
