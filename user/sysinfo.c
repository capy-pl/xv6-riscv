#include "kernel/types.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

int main(int argc, char *argv[]) {
    struct sysinfo info;
    sysinfo(&info);
    printf("number of processes: %d\n", info.nproc);
    printf("number of free memory: %d\n", info.freemem);
    exit(0);
}
