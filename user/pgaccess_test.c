#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int main(int argc, char *argv) {
    char *buf;
    uint64 bitmask;
    buf = malloc(32 * PGSIZE);
    if (pgaccess(buf, 32, &bitmask) < 0) {
        printf("pgaccess failed.\n");
        exit(1);
    }
    if (bitmask == 0) {
        printf("pgaccess: the buf has not been used yet.\n");
    }

    buf[PGSIZE * 1] = 1;
    buf[PGSIZE * 20] = 2;
    buf[PGSIZE * 30] = 3;

    if (pgaccess(buf, 32, &bitmask) < 0) {
        printf("pgaccess failed.\n");
        exit(1);
    }

    if ((bitmask & (1 << 1)) > 0 && (bitmask & (1 << 20)) > 0 && (bitmask & (1 << 30)) > 0) {
        printf("pgaccess success.\n");
    }  else {
        printf("pgaccess failed.\n");
        exit(1);
    }
    free(buf);
    exit(0);
}