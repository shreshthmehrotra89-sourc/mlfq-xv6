#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int x = 0;

struct result {
  int pid;
  int start_time;
  int first_run_time;
  int end_time;
  int cpu_time;
};

void
cpu_work(int amount)
{
  int i;

  for(i = 0; i < amount; i++)
    x++;
}

int
main(void)
{
  int pid;
  int i;

  struct proc_info info;

  printf("\n");
  printf("========================================\n");
  printf("          MLFQ BENCHMARK TEST\n");
  printf("========================================\n");
  printf("\n");

  /*
   * Create four CPU-bound processes.
   */
  for(i = 0; i < 4; i++){

    pid = fork();

    if(pid < 0){
      printf("[BENCH] fork failed\n");
      exit(1);
    }

    if(pid == 0){

      printf("[BENCH] Process started: pid=%d workload=%d\n",
             getpid(), i);

      if(i == 0)
  cpu_work(50000000);
else if(i == 1)
  cpu_work(100000000);
else if(i == 2)
  cpu_work(150000000);
else
  cpu_work(200000000);

      printf("[BENCH] Process finished: pid=%d workload=%d\n",
             getpid(), i);

      exit(0);
    }
  }

  /*
   * Collect timing information for all children.
   *
   * wait_info() copies the timing information
   * before the child process is freed.
   */
  printf("\n");
  printf("========================================\n");
  printf("             TIMING RESULTS\n");
  printf("========================================\n");

  printf("PID\tStart\tFirst\tEnd\tCPU\tResponse\tTurnaround\tWaiting\n");

  for(i = 0; i < 4; i++){

    if(wait_info(&info) < 0){
      printf("[BENCH] wait_info failed\n");
      exit(1);
    }

    int response;
    int turnaround;
    int waiting;

    response = info.first_run_time - info.start_time;

    turnaround = info.end_time - info.start_time;

    waiting = turnaround - info.cpu_time;

    printf("%d\t%d\t%d\t%d\t%d\t%d\t\t%d\t\t%d\n",
           info.pid,
           info.start_time,
           info.first_run_time,
           info.end_time,
           info.cpu_time,
           response,
           turnaround,
           waiting);
  }

  printf("\n");
  printf("========================================\n");
  printf("       BENCHMARK TEST COMPLETE\n");
  printf("========================================\n");
  printf("\n");

  exit(0);
}