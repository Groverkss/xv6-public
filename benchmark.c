#include "types.h"
#include "user.h"

int n_proc = 16;

int
main(void) 
{

  set_priority(5, getpid());
  for (int i = 0; i < n_proc; i++) {
    int pid = fork();

    if (!pid) {
      for (volatile int i = 0; i < 1000000000; i++) {
        ;
      }
      printf(2, "Ded %d\n", i);
      exit();
    } else {
      set_priority(60 - i / 4, pid);
      printf(2, "Index: %d Pid: %d\n", i, pid);
    }
  }

  for (int i = 0; i < n_proc; i++) {
    wait();
  }

  exit();
}
