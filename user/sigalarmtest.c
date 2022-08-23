#include "kernel/types.h"
#include "user/user.h"

void periodic();

int
main(int argc, char *argv[])
{
  int i;
  printf("alarmtest starting\n");
  sigalarm(1, periodic);
  for(i = 0; i < 2000*500000; i++){
    if((i++ % 1000000) == 0)
      printf(".");
  }
  exit(0);
}

void
periodic()
{
  printf("alarm!\n");
  sigreturn();
}