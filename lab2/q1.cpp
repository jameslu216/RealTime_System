#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <unistd.h>



  FILE *fp=fopen("data.txt","w");
void setSchedulingPolicy (int policy, int priority)
{
    sched_param sched;
    sched_getparam(0, &sched);
    sched.sched_priority = priority;
    if (sched_setscheduler(0, policy, &sched)) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }
}
void workload_1ms (void)
{
        //int repeat = 100000; // tune this for the right amount of workload
        int repeat = 2000; // tune this for the right amount of workload
        int count[100]={1};

        for (int i = 0; i <= repeat; i++)
        {
          for (int j=0;j<100;j++)
          count[j]=j;
          for(int j=0;j<99;j++ )
          {
            if(count[j+1]>count[j])
          {
            int tmp=count[j+1];
            count[j+1]=count[j];
            count[j]=tmp;
          }
        }
        }
}
void pinCPU (int cpu_number)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);

    CPU_SET(cpu_number, &mask);

    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1)
    {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}
void *firstthread(void *pnum_times)
{

    pinCPU(0);
    int period = 50000; // unit: microsecond
    int delta;
    int execTime=1;
    setSchedulingPolicy (SCHED_FIFO, 99);
    while(1)
    {
        fprintf(fp,"doing thread1\n");
        std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
        for (int j = 0; j < execTime; j++)
        {
            workload_1ms ();
        }
        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
        delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        if(delta > period) {
          std::cout << "Task with period = " << period << " and worst case exec time = " << execTime << " exceed period." << std::endl;
          fflush(stdout);
          continue;
        } else {
          std::cout << "response time of task with period = " << period << " and worst case exec time = " << execTime << ", workload = " << delta << " us" << std::endl;
          fflush(stdout);
          usleep((period-delta));
        }
    }
}

void *secondthread(void *pnum_times)
{

  pinCPU(0);
  int period = 100000; // unit: microsecond
  int delta;
  int execTime=50;
  setSchedulingPolicy (SCHED_FIFO, 97);
  while(1)
  {
      fprintf(fp,"doing thread2\n");
      std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
      for (int j = 0; j < execTime; j++)
      {
          workload_1ms ();
      }
      std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
      delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
      if(delta > period) {
        std::cout << "Task with period = " << period << " and worst case exec time = " << execTime << " exceed period." << std::endl;
        fflush(stdout);
        continue;
      }
      else {
        std::cout << "response time of task with period = " << period << " and worst case exec time = " << execTime << ", workload = " << delta << " us" << std::endl;
        fflush(stdout);
        usleep((period-delta));
      }
  }
}
void *thirdthread(void *pnum_times)
{

  pinCPU(0);
  int period = 200000; // unit: microsecond
  int delta;
  int execTime=10;
  setSchedulingPolicy (SCHED_FIFO, 98);
  while(1)
  {
      fprintf(fp,"doing thread3\n");
      std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
      for (int j = 0; j < execTime; j++)
      {
          workload_1ms ();
      }
      std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
      delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
      if(delta > period) {
        std::cout << "Task with period = " << period << " and worst case exec time = " << execTime << " exceed period." << std::endl;
        fflush(stdout);
        continue;
      }
      else {
        std::cout << "response time of task with period = " << period << " and worst case exec time = " << execTime << ", workload = " << delta << " us" << std::endl;
        fflush(stdout);
        usleep((period-delta));
      }
  }

}


int main(void)
{
  if(!fp) printf("File can not be open\n");
  int r1,r2,r3;
  pthread_t thread1, thread2,thread3;
  if (pthread_create(&thread1,NULL,firstthread,(void *) &r1) != 0)
	perror("pthread_create"), exit(1);
  if (pthread_create(&thread2,NULL,secondthread,(void *) &r2) != 0)
	perror("pthread_create"), exit(1);
  if (pthread_create(&thread3,NULL,thirdthread,(void *) &r3) != 0)
  perror("pthread_create"), exit(1);
  if (pthread_join(thread1, NULL) != 0)
	perror("pthread_join"),exit(1);
  if (pthread_join(thread2, NULL) != 0)
	perror("pthread_join"),exit(1);
  if (pthread_join(thread3, NULL) != 0)
	perror("pthread_join"),exit(1);
    return 0;
}
