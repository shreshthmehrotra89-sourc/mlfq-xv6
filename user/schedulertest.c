
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int x = 0;

/*
 * Perform CPU-intensive work.
 * The volatile variable prevents the compiler from
 * completely removing the loop.
 */
void cpu_burst(int amount)
{
  int i;

  for(i = 0; i < amount; i++)
    x++;
}

/*
 * Each process has a different workload.
 *
 * The workloads are intentionally large so that
 * processes remain runnable long enough to demonstrate:
 *
 * Q0 -> Q1 -> Q2 -> Q3
 *
 * and the periodic 48-tick priority boost.
 */
void run_workload(int id)
{
  int burst;
  int rounds;
  int i;

  if(id == 0){
    burst = 30000000;
    rounds = 20;
  }
  else if(id == 1){
    burst = 60000000;
    rounds = 15;
  }
  else if(id == 2){
    burst = 100000000;
    rounds = 12;
  }
  else{
    burst = 150000000;
    rounds = 10;
  }

  printf("[SCHEDTEST] PID %d started: type=%d burst=%d rounds=%d\n",
         getpid(), id, burst, rounds);

  for(i = 0; i < rounds; i++){

    cpu_burst(burst);

    printf("[SCHEDTEST] PID %d completed burst: round=%d\n",
           getpid(), i);
  }

  printf("[SCHEDTEST] PID %d finished: type=%d\n",
         getpid(), id);

  exit(0);
}

int
main(void)
{
  int pid;
  int i;

  struct proc_info info;

  int response;
  int turnaround;
  int waiting;

  int total_response = 0;
  int total_turnaround = 0;
  int total_waiting = 0;

  printf("\n");
  printf("========================================\n");
  printf("          MLFQ SCHEDULER TEST\n");
  printf("========================================\n");
  printf("\n");

  printf("[SCHEDTEST] Creating 4 processes...\n");
  printf("[SCHEDTEST] Workloads:\n");
  printf("  Process 0 -> short CPU bursts\n");
  printf("  Process 1 -> medium CPU bursts\n");
  printf("  Process 2 -> long CPU bursts\n");
  printf("  Process 3 -> very long CPU bursts / CPU-bound\n");
  printf("\n");

  /*
   * Create four child processes.
   */
  for(i = 0; i < 4; i++){

    pid = fork();

    if(pid < 0){
      printf("[SCHEDTEST] fork failed\n");
      exit(1);
    }

    if(pid == 0){
      run_workload(i);
    }
  }

  /*
   * Collect information about every child.
   */
  printf("\n");
  printf("========================================\n");
  printf("        PROCESS STATISTICS\n");
  printf("========================================\n");

  printf("PID   Start   FirstCPU   End   CPU   Response   Turnaround   Waiting\n");
  printf("---------------------------------------------------------------------\n");

  for(i = 0; i < 4; i++){

    if(wait_info(&info) < 0){
      printf("[SCHEDTEST] wait_info failed\n");
      continue;
    }

    response = info.first_run_time - info.start_time;
    turnaround = info.end_time - info.start_time;
    waiting = turnaround - info.cpu_time;

    total_response += response;
    total_turnaround += turnaround;
    total_waiting += waiting;

    printf("%d    %d       %d        %d    %d      %d          %d          %d\n",
           info.pid,
           info.start_time,
           info.first_run_time,
           info.end_time,
           info.cpu_time,
           response,
           turnaround,
           waiting);
  }

  /*
   * Average scheduling metrics.
   */
  printf("\n");
  printf("========================================\n");
  printf("           AVERAGE RESULTS\n");
  printf("========================================\n");

  printf("Average Response Time   = %d ticks\n",
         total_response / 4);

  printf("Average Turnaround Time = %d ticks\n",
         total_turnaround / 4);

  printf("Average Waiting Time    = %d ticks\n",
         total_waiting / 4);

  printf("\n");
  printf("========================================\n");
  printf("        MLFQ SCHEDULER TEST DONE\n");
  printf("========================================\n");

  exit(0);
}

