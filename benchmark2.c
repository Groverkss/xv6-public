#include "types.h"
#include "user.h"

const int NITER = 1e9;
const int NSLEEP = 1e8;

int main(int argc, char *argv[])
{
  for (volatile int i = 1; i <= NITER; i++) {
    ;
  }
  exit();
}
