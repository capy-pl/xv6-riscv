#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/memlayout.h"
#include "user/user.h"

int main(int argc, char *argv) {
    printf("pid: %d\n", ugetpid());
    exit(0);
}