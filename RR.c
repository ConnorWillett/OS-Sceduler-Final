#include <stdio.h>
#include <stdlib.h>

struct job {
  int id;
  int current_cycle;
  int total_cycles;
  int total_time;     
  int response_time; 
  int isDone;
  int responded;
  int arrival_time;
};

int main()
{
  struct job j1 = {.id=1, .current_cycle=0, .total_cycles=100, .total_time=0, .response_time=0, .isDone=0, .responded=0, .arrival_time=  0};
  struct job j2 = {.id=2, .current_cycle=0, .total_cycles= 20, .total_time=0, .response_time=0, .isDone=0, .responded=0, .arrival_time=  0};
  struct job j3 = {.id=3, .current_cycle=0, .total_cycles= 10, .total_time=0, .response_time=0, .isDone=0, .responded=0, .arrival_time=  0};
  struct job j4 = {.id=4, .current_cycle=0, .total_cycles= 30, .total_time=0, .response_time=0, .isDone=0, .responded=0, .arrival_time=  0};
  struct job j5 = {.id=5, .current_cycle=0, .total_cycles= 50, .total_time=0, .response_time=0, .isDone=0, .responded=0, .arrival_time=  0};
  struct job j6 = {.id=6, .current_cycle=0, .total_cycles= 15, .total_time=0, .response_time=0, .isDone=0, .responded=0, .arrival_time=100}; 

  struct job *jobs[] = {&j1, &j2, &j3, &j4, &j5, &j6};
  int length = sizeof(jobs) / sizeof(jobs[0]);

  int time = 0;
  int jobI = 0;   

  while (1) {
    int alldone = 1;
    for (int i = 0; i < length; i++)
      if (!jobs[i]->isDone) { alldone = 0; break; }
    if (alldone) break;

    if (jobs[jobI]->isDone || jobs[jobI]->arrival_time > time) {
      int found = 0;
      for (int c = 0; c < length; c++) {
        int idx = (jobI + c) % length;
        if (!jobs[idx]->isDone && jobs[idx]->arrival_time <= time) {
          jobI = idx;
          found = 1;
          break;
        }
      }
      if (!found) {
        int next_time = -1;
        for (int i = 0; i < length; i++)
          if (!jobs[i]->isDone && (next_time == -1 || jobs[i]->arrival_time < next_time))
            next_time = jobs[i]->arrival_time;
        time = next_time;
        continue;
      }
    }

    jobs[jobI]->current_cycle++;
    jobs[jobI]->responded = 1;

    for (int i = 0; i < length; i++) {
      if (!jobs[i]->isDone && jobs[i]->arrival_time <= time)
        jobs[i]->total_time++;
      if (!jobs[i]->responded && jobs[i]->arrival_time <= time)
        jobs[i]->response_time++;
    }

    if (jobs[jobI]->current_cycle == jobs[jobI]->total_cycles)
      jobs[jobI]->isDone = 1;

    time++;

    jobI = (jobI + 1) % length;
  }

  printf("job\ttotal time\tresponse time\n");
  for (int i = 0; i < length; i++)
    printf("%d\t%d\t\t%d\n", jobs[i]->id, jobs[i]->total_time, jobs[i]->response_time);

  return 0;
}
