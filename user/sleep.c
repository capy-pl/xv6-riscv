#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int second;
  if(argc < 2){
    fprintf(2, "Usage: sleep...\n");
    exit(1);
  }

  second = atoi(argv[1]);
  sleep(second);
  exit(0);
}