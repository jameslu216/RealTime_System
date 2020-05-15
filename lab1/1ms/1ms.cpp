#include <sched.h>
#include <stdio.h>

#include <iostream>
#include <chrono>
#include <cmath>

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
        int repeat = 17; // tune this for the right amount of workload
        int count[100]={};

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

int main (void)
{
        setSchedulingPolicy (SCHED_FIFO, 99);
        std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
        workload_1ms ();
        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
        const int delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cout << delta << std::endl;

        return 0;
}
