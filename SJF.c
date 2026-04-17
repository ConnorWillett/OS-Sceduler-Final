#include <stdio.h>
#include <stdlib.h>

struct job {
  int id;
  int total_cycles;
  int total_time;     
  int response_time; 
  int isDone;
  int arrival_time;
};

int main()
{
  struct job j1 = {.id=1, .total_cycles=100, .total_time=0, .response_time=0, .isDone=0, .arrival_time=  0};
  struct job j2 = {.id=2, .total_cycles= 20, .total_time=0, .response_time=0, .isDone=0, .arrival_time=  0};
  struct job j3 = {.id=3, .total_cycles= 10, .total_time=0, .response_time=0, .isDone=0, .arrival_time=  0};
  struct job j4 = {.id=4, .total_cycles= 30, .total_time=0, .response_time=0, .isDone=0, .arrival_time=  0};
  struct job j5 = {.id=5, .total_cycles= 50, .total_time=0, .response_time=0, .isDone=0, .arrival_time=  0};
  struct job j6 = {.id=6, .total_cycles= 15, .total_time=0, .response_time=0, .isDone=0, .arrival_time=100}; 

  struct job *jobs[] = {&j1, &j2, &j3, &j4, &j5, &j6};
  int length = sizeof(jobs) / sizeof(jobs[0]);

  int time = 0;
  int completed = 0;

  while (completed < length) {
    int chosen = -1;
    for (int i = 0; i < length; i++) {
      if (!jobs[i]->isDone && jobs[i]->arrival_time <= time) {
        if (chosen == -1 || jobs[i]->total_cycles < jobs[chosen]->total_cycles)
          chosen = i;
      }
    }

    if (chosen == -1) {
      int next = -1;
      for (int i = 0; i < length; i++)
        if (!jobs[i]->isDone && (next == -1 || jobs[i]->arrival_time < jobs[next]->arrival_time))
          next = i;
      time = jobs[next]->arrival_time;
      continue;
    }

    jobs[chosen]->response_time = time - jobs[chosen]->arrival_time;
    time += jobs[chosen]->total_cycles;
    jobs[chosen]->total_time = time - jobs[chosen]->arrival_time;
    jobs[chosen]->isDone = 1;
    completed++;
  }

  printf("job\ttotal time\tresponse time\n");
  for (int i = 0; i < length; i++)
    printf("%d\t%d\t\t%d\n", jobs[i]->id, jobs[i]->total_time, jobs[i]->response_time);

  return 0;
}
