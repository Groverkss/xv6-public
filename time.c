#include "types.h"
#include "user.h"

int
main(int argc, char *argv[]) 
{
  int child_pid = fork();
  if (child_pid == -1) {
    printf(2, "Error while forking\n");
    exit();
  }

  if (!child_pid) {
    volatile int a = 0;
    for (int i = 0; i < 2000000000; i++) {
      a++; 
    }
    printf(1, "%d\n", a);
    exit();
  } else {
    int wtime, rtime;
    waitx(&wtime, &rtime);
    printf(1, "Parent: Wait Time: %d, Run Time: %d\n", wtime, rtime);
    exit();
  }
}
