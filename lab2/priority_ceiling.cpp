#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


struct Threadtask {
  int period, execTime, priority;
};

void setSchedulingPolicy (int newPolicy, int priority) {
    sched_param sched;
    int oldPolicy;
    if (pthread_getschedparam(pthread_self(), &oldPolicy, &sched)) {
        perror("pthread_getschedparam()");
        exit(EXIT_FAILURE);
    }
    sched.sched_priority = priority;
    if (pthread_setschedparam(pthread_self(), newPolicy, &sched)) {
        perror("pthread_setschedparam()");
        exit(EXIT_FAILURE);
    }
}

void workload_1ms (void)
{
        //int repeat = 100000; // tune this for the right amount of workload
        int repeat = 2500; // tune this for the right amount of workload
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
static void pinCPU(int cpu_number) {
    cpu_set_t mask;
    CPU_ZERO(&mask);

    CPU_SET(cpu_number, &mask);

    if(sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}

pthread_mutex_t mutex_section_1 = PTHREAD_MUTEX_INITIALIZER;
constexpr int TaskNum = 4;
constexpr int MAXPR=99;
constexpr int MINPR=96;
constexpr int MILLI_TO_MICRO=1000;
Threadtask task[TaskNum] = {
  {50, 1, MAXPR},
  {100, 50, MINPR},
  {200, 10, 98},
  {150, 20, 97} //priority set by RM algorithm
};

void *threadTask(void *task_) {
  pinCPU(0);
  Threadtask *task=(Threadtask *)task_;
  setSchedulingPolicy(SCHED_FIFO, ((Threadtask *)task)->priority);
  int priority = task->priority;
  int period = task->period;
  int execTime = task->execTime;
  int delta;
  while(1) {
    std::cout<<"doing priority"<<task->priority<<std::endl;
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    if(task->priority==MAXPR||task->priority==MINPR)
    {
    std::cout<<"Lock critial section"<<std::endl;
        pthread_mutex_lock(&mutex_section_1);
        for(int i = 0;i < execTime;++i) {
          workload_1ms();
        }
        pthread_mutex_unlock(&mutex_section_1);
    std::cout<<"Unlock critial section"<<std::endl;
    }
    else
    {
      for(int i = 0;i < execTime;++i) {
        workload_1ms();
    }
    }
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    if(delta > period * MILLI_TO_MICRO) {
      std::cout << "out of time" << " period = " << period << ",worst executiontime = " << execTime << " exceed period." << std::endl;
      fflush(stdout);
      continue;
    } else
    {
      std::cout << "In time"<<" period = " << period << " worst executiontime = " << execTime << ", workload = " << delta/1000 << std::endl;
      fflush(stdout);
      usleep(period * MILLI_TO_MICRO-delta);
    }
  }
}


int main() {
  pthread_mutexattr_t mutexattr_prioceiling;
  int mutex_protocol, high_prio;
  high_prio = sched_get_priority_max(SCHED_FIFO);
  pthread_mutexattr_init(&mutexattr_prioceiling);
  pthread_mutexattr_getprotocol(&mutexattr_prioceiling,
                                 &mutex_protocol);
  pthread_mutexattr_setprotocol(&mutexattr_prioceiling,
                                 PTHREAD_PRIO_PROTECT);
  pthread_mutexattr_setprioceiling(&mutexattr_prioceiling,
                                    high_prio);
  pthread_mutex_init (&mutex_section_1, &mutexattr_prioceiling);

  pthread_t pthread[TaskNum];
  for(int i = 0;i < TaskNum;++i) {
    if(pthread_create(&pthread[i], NULL, threadTask, (void *)&task[i])) {
      perror("pthread_create()");
      exit(1);
    }
  }

  for(int i = 0;i < TaskNum;++i) {
    if (pthread_join(pthread[i], NULL) != 0) {
      perror("pthread_join()");
      exit(1);
    }
  }

  return 0;
}
