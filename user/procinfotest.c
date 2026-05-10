#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(void)
{
  struct procinfo info;
  
  if (procinfo(getpid(), &info) < 0) {
    printf("procinfo failed\n");
    exit(1);
  }
  
  printf("Process: %s\n", info.name);
  printf("PID: %d, PPID: %d\n", info.pid, info.ppid);
  if (info.state == 0) printf("State: UNUSED\n");
  else if (info.state == 1) printf("State: USED\n");
  else if (info.state == 2) printf("State: SLEEPING\n");
  else if (info.state == 3) printf("State: RUNNABLE\n");
  else if (info.state == 4) printf("State: RUNNING\n");
  else if (info.state == 5) printf("State: ZOMBIE\n");
  else printf("State: UNKNOWN\n");
  printf("Memory: %d bytes\n", (int)info.sz);
  
  exit(0);
}
