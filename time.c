#include "types.h"
#include "user.h"

int
main(int argc, char * argv[])
{
  if(argc == 1) {
    printf(2, "Usage: time command [...args]\n");
    exit();
  }

  int f = fork();

  if(f < 0) {
    printf(2, "fork() failed\n");
    exit();
  } else if(f == 0) {
    exec(argv[1], argv + 1);
    printf(2, "exec() failed\n");
    exit();
  } else {
    int rtime, wtime;
    waitx(&wtime, &rtime);

    printf(1, "rtime = %d\nwtime = %d\n", rtime, wtime);
  }

  exit();
}
