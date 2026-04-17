#include <stdio.h>
#include <stdlib.h>

#define NUM_QUEUES 3

struct job {
  int id;
  int current_cycle;
  int total_cycles;
  int total_time;      
  int response_time;  
  int isDone;
  int responded;
  int queue;
  int cycles_in_queue;
  int arrival_time;
};

int main()
{
  struct job j1 = {.id=1, .current_cycle=0, .total_cycles=100, .total_time=0, .response_time=0, .isDone=0, .responded=0, .queue=0, .cycles_in_queue=0, .arrival_time=  0};
  struct job j2 = {.id=2, .current_cycle=0, .total_cycles= 20, .total_time=0, .response_time=0, .isDone=0, .responded=0, .queue=0, .cycles_in_queue=0, .arrival_time=  0};
  struct job j3 = {.id=3, .current_cycle=0, .total_cycles= 10, .total_time=0, .response_time=0, .isDone=0, .responded=0, .queue=0, .cycles_in_queue=0, .arrival_time=  0};
  struct job j4 = {.id=4, .current_cycle=0, .total_cycles= 30, .total_time=0, .response_time=0, .isDone=0, .responded=0, .queue=0, .cycles_in_queue=0, .arrival_time=  0};
  struct job j5 = {.id=5, .current_cycle=0, .total_cycles= 50, .total_time=0, .response_time=0, .isDone=0, .responded=0, .queue=0, .cycles_in_queue=0, .arrival_time=  0};
  struct job j6 = {.id=6, .current_cycle=0, .total_cycles= 15, .total_time=0, .response_time=0, .isDone=0, .responded=0, .queue=0, .cycles_in_queue=0, .arrival_time=100}; 

  struct job *jobs[] = {&j1, &j2, &j3, &j4, &j5, &j6};
  int length = sizeof(jobs) / sizeof(jobs[0]);

  int quantums[NUM_QUEUES] = {10, 20, 40};
  int queue_ptr[NUM_QUEUES] = {0, 0, 0};
  int time = 0;

  while (1) {
    int alldone = 1;
    for (int i = 0; i < length; i++)
      if (!jobs[i]->isDone) { alldone = 0; break; }
    if (alldone) break;

    int chosen = -1, chosen_q = -1;
    for (int q = 0; q < NUM_QUEUES && chosen == -1; q++) {
      for (int offset = 0; offset < length; offset++) {
        int idx = (queue_ptr[q] + offset) % length;
        if (!jobs[idx]->isDone && jobs[idx]->queue == q && jobs[idx]->arrival_time <= time) {
          chosen = idx;
          chosen_q = q;
          break;
        }
      }
    }

    if (chosen == -1) {
      int next_time = -1;
      for (int i = 0; i < length; i++)
        if (!jobs[i]->isDone && (next_time == -1 || jobs[i]->arrival_time < next_time))
          next_time = jobs[i]->arrival_time;
      time = next_time;
      continue;
    }

    jobs[chosen]->current_cycle++;
    jobs[chosen]->cycles_in_queue++;
    jobs[chosen]->responded = 1;

    for (int i = 0; i < length; i++) {
      if (!jobs[i]->isDone && jobs[i]->arrival_time <= time)
        jobs[i]->total_time++;
      if (!jobs[i]->responded && jobs[i]->arrival_time <= time)
        jobs[i]->response_time++;
    }

    time++;

    if (jobs[chosen]->current_cycle == jobs[chosen]->total_cycles) {
      jobs[chosen]->isDone = 1;
    } else if (jobs[chosen]->cycles_in_queue == quantums[chosen_q]) {
      if (chosen_q < NUM_QUEUES - 1)
        jobs[chosen]->queue++;
      jobs[chosen]->cycles_in_queue = 0;
    }

    queue_ptr[chosen_q] = (chosen + 1) % length;
  }

  printf("job\ttotal time\tresponse time\n");
  for (int i = 0; i < length; i++)
    printf("%d\t%d\t\t%d\n", jobs[i]->id, jobs[i]->total_time, jobs[i]->response_time);

  return 0;
}
