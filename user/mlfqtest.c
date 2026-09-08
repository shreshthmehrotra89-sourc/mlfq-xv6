#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int x = 0;

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
  int pid1;
  int pid2;
  int pid3;
  int i;

  printf("\n");
  printf("========================================\n");
  printf("        MLFQ COMPREHENSIVE TEST\n");
  printf("========================================\n");
  printf("\n");

  /*
   * TEST 1:
   * Create a CPU-bound process.
   *
   * A new process should enter Q0.
   */
  printf("[TEST 1] Creating CPU-bound process A\n");

  pid1 = fork();

  if(pid1 < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid1 == 0){

    printf("[A] Started: pid=%d\n", getpid());
    printf("[A] New process should start in Q0\n");

    /*
     * TEST 2:
     * CPU-bound process should consume complete
     * time slices and move:
     *
     * Q0 -> Q1 -> Q2 -> Q3
     */
    printf("[A] Starting CPU work for demotion test\n");

    cpu_work(30000000);

    printf("[A] CPU work finished\n");

    /*
     * TEST 3:
     * Create another process while A is at a
     * lower priority.
     *
     * B starts in Q0.
     */
    printf("[A] Creating higher-priority process B\n");

    pid2 = fork();

    if(pid2 < 0){
      printf("[A] fork B failed\n");
      exit(1);
    }

    if(pid2 == 0){

      printf("[B] Started: pid=%d\n", getpid());
      printf("[B] New process should be in Q0\n");

      /*
       * Keep B CPU-bound.
       *
       * Since B is in a higher-priority queue,
       * it should run before A whenever B is
       * RUNNABLE.
       */
      while(1)
        x++;

    }

    /*
     * A continues CPU-bound.
     *
     * If B is in Q0 and A is in a lower queue,
     * A should be preempted when B becomes
     * runnable.
     */
    printf("[A] Continuing CPU work\n");

    while(1)
      x++;
  }

  /*
   * The original parent creates another process.
   *
   * This gives us another CPU-bound process
   * for observing same-priority round robin.
   */
  printf("[PARENT] Creating CPU-bound process C\n");

  pid3 = fork();

  if(pid3 < 0){
    printf("fork C failed\n");
    exit(1);
  }

  if(pid3 == 0){

    printf("[C] Started: pid=%d\n", getpid());

    /*
     * C is also CPU-bound.
     *
     * It will eventually be demoted and can
     * participate in round-robin at Q3.
     */
    for(i = 0; i < 30000000; i++)
      x++;

    printf("[C] Finished initial CPU work\n");

    while(1)
      x++;
  }

  /*
   * Parent exits so it does not interfere with
   * scheduler behavior.
   */
  printf("[PARENT] Test processes are running\n");
  printf("[PARENT] Use Ctrl+P to inspect queues\n");
  printf("\n");

  exit(0);
}