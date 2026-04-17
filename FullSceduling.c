#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_JOBS 10
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

void reset_jobs(struct job *jobs[]) {
  for (int i = 0; i < NUM_JOBS; i++) {
    jobs[i]->current_cycle = 0;
    jobs[i]->total_time = 0;
    jobs[i]->response_time = 0;
    jobs[i]->isDone = 0;
    jobs[i]->responded = 0;
    jobs[i]->queue = 0;
    jobs[i]->cycles_in_queue = 0;
  }
}

void run_sjf(struct job *jobs[]) {
  int time = 0;
  int completed = 0;

  while (completed < NUM_JOBS) {
    int chosen = -1;

    for (int i = 0; i < NUM_JOBS; i++) {
      if (!jobs[i]->isDone && jobs[i]->arrival_time <= time) {
        if (chosen == -1 ||
            jobs[i]->total_cycles < jobs[chosen]->total_cycles)
          chosen = i;
      }
    }

    if (chosen == -1) {
      int next = -1;
      for (int i = 0; i < NUM_JOBS; i++)
        if (!jobs[i]->isDone &&
           (next == -1 || jobs[i]->arrival_time < jobs[next]->arrival_time))
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
}

void print_stats(struct job *jobs[], const char *name) {
  printf("\n--- %s ---\n", name);
  printf("job\ttotal\tresponse\n");

  double mean_tt = 0, mean_rt = 0;

  for (int i = 0; i < NUM_JOBS; i++) {
    printf("%d\t%d\t%d\n",
      jobs[i]->id,
      jobs[i]->total_time,
      jobs[i]->response_time);

    mean_tt += jobs[i]->total_time;
    mean_rt += jobs[i]->response_time;
  }

  mean_tt /= NUM_JOBS;
  mean_rt /= NUM_JOBS;

  double var_tt = 0, var_rt = 0;
  for (int i = 0; i < NUM_JOBS; i++) {
    var_tt += pow(jobs[i]->total_time - mean_tt, 2);
    var_rt += pow(jobs[i]->response_time - mean_rt, 2);
  }

  var_tt /= NUM_JOBS;
  var_rt /= NUM_JOBS;

  printf("mean TT: %.2f | std: %.2f\n", mean_tt, sqrt(var_tt));
  printf("mean RT: %.2f | std: %.2f\n", mean_rt, sqrt(var_rt));
}

void run_fifo(struct job *jobs[]) {
  int time = 0;

  for (int i = 0; i < NUM_JOBS - 1; i++)
    for (int j = 0; j < NUM_JOBS - i - 1; j++)
      if (jobs[j]->arrival_time > jobs[j+1]->arrival_time) {
        struct job *tmp = jobs[j];
        jobs[j] = jobs[j+1];
        jobs[j+1] = tmp;
      }

  for (int i = 0; i < NUM_JOBS; i++) {
    if (time < jobs[i]->arrival_time)
      time = jobs[i]->arrival_time;

    jobs[i]->response_time = time - jobs[i]->arrival_time;
    time += jobs[i]->total_cycles;
    jobs[i]->total_time = time - jobs[i]->arrival_time;
    jobs[i]->isDone = 1;
  }
}

void run_rr(struct job *jobs[]) {
  int time = 0, jobI = 0;

  while (1) {
    int done = 1;
    for (int i = 0; i < NUM_JOBS; i++)
      if (!jobs[i]->isDone) done = 0;
    if (done) break;

    if (jobs[jobI]->isDone || jobs[jobI]->arrival_time > time) {
      int found = 0;
      for (int c = 0; c < NUM_JOBS; c++) {
        int idx = (jobI + c) % NUM_JOBS;
        if (!jobs[idx]->isDone && jobs[idx]->arrival_time <= time) {
          jobI = idx;
          found = 1;
          break;
        }
      }
      if (!found) {
        int next = -1;
        for (int i = 0; i < NUM_JOBS; i++)
          if (!jobs[i]->isDone &&
             (next == -1 || jobs[i]->arrival_time < next))
            next = jobs[i]->arrival_time;
        time = next;
        continue;
      }
    }

    jobs[jobI]->current_cycle++;

    if (!jobs[jobI]->responded)
      jobs[jobI]->responded = 1;

    for (int i = 0; i < NUM_JOBS; i++) {
      if (!jobs[i]->isDone && jobs[i]->arrival_time <= time)
        jobs[i]->total_time++;
      if (!jobs[i]->responded && jobs[i]->arrival_time <= time)
        jobs[i]->response_time++;
    }

    if (jobs[jobI]->current_cycle == jobs[jobI]->total_cycles)
      jobs[jobI]->isDone = 1;

    time++;
    jobI = (jobI + 1) % NUM_JOBS;
  }
}

void run_mlfq(struct job *jobs[]) {
  int quantums[NUM_QUEUES] = {10,20,40};
  int queue_ptr[NUM_QUEUES] = {0};
  int time = 0;

  while (1) {
    int done = 1;
    for (int i = 0; i < NUM_JOBS; i++)
      if (!jobs[i]->isDone) done = 0;
    if (done) break;

    int chosen = -1, q = -1;

    for (int i = 0; i < NUM_QUEUES && chosen == -1; i++) {
      for (int o = 0; o < NUM_JOBS; o++) {
        int idx = (queue_ptr[i] + o) % NUM_JOBS;
        if (!jobs[idx]->isDone &&
            jobs[idx]->queue == i &&
            jobs[idx]->arrival_time <= time) {
          chosen = idx;
          q = i;
          break;
        }
      }
    }

    if (chosen == -1) {
      int next = -1;
      for (int i = 0; i < NUM_JOBS; i++)
        if (!jobs[i]->isDone &&
           (next == -1 || jobs[i]->arrival_time < next))
          next = jobs[i]->arrival_time;
      time = next;
      continue;
    }

    jobs[chosen]->current_cycle++;
    jobs[chosen]->cycles_in_queue++;

    if (!jobs[chosen]->responded)
      jobs[chosen]->responded = 1;

    for (int i = 0; i < NUM_JOBS; i++) {
      if (!jobs[i]->isDone && jobs[i]->arrival_time <= time)
        jobs[i]->total_time++;
      if (!jobs[i]->responded && jobs[i]->arrival_time <= time)
        jobs[i]->response_time++;
    }

    time++;

    if (jobs[chosen]->current_cycle == jobs[chosen]->total_cycles) {
      jobs[chosen]->isDone = 1;
    } else if (jobs[chosen]->cycles_in_queue == quantums[q]) {
      if (q < NUM_QUEUES - 1)
        jobs[chosen]->queue++;
      jobs[chosen]->cycles_in_queue = 0;
    }

    queue_ptr[q] = (chosen + 1) % NUM_JOBS;
  }
}

void load_workload1(struct job *jobs[]) {
  static struct job j[10] = {
    {1,0,42,0,0,0,0,0,0,0},{2,0,15,0,0,0,0,0,0,0},
    {3,0,87,0,0,0,0,0,0,10},{4,0,23,0,0,0,0,0,0,0},
    {5,0,65,0,0,0,0,0,0,20},{6,0,12,0,0,0,0,0,0,0},
    {7,0,90,0,0,0,0,0,0,50},{8,0,34,0,0,0,0,0,0,0},
    {9,0,58,0,0,0,0,0,0,75},{10,0,19,0,0,0,0,0,0,0}
  };
  for(int i=0;i<10;i++) jobs[i]=&j[i];
}

void load_workload2(struct job *jobs[]) {
  static struct job j[10] = {
    {1,0,10,0,0,0,0,0,0,0},{2,0,95,0,0,0,0,0,0,40},
    {3,0,33,0,0,0,0,0,0,0},{4,0,76,0,0,0,0,0,0,60},
    {5,0,21,0,0,0,0,0,0,0},{6,0,55,0,0,0,0,0,0,0},
    {7,0,18,0,0,0,0,0,0,90},{8,0,44,0,0,0,0,0,0,0},
    {9,0,67,0,0,0,0,0,0,120},{10,0,29,0,0,0,0,0,0,0}
  };
  for(int i=0;i<10;i++) jobs[i]=&j[i];
}

void load_workload3(struct job *jobs[]) {
  static struct job j[10] = {
    {1,0,60,0,0,0,0,0,0,0},{2,0,14,0,0,0,0,0,0,0},
    {3,0,72,0,0,0,0,0,0,25},{4,0,8,0,0,0,0,0,0,0},
    {5,0,49,0,0,0,0,0,0,0},{6,0,91,0,0,0,0,0,0,80},
    {7,0,27,0,0,0,0,0,0,0},{8,0,36,0,0,0,0,0,0,110},
    {9,0,53,0,0,0,0,0,0,0},{10,0,17,0,0,0,0,0,0,0}
  };
  for(int i=0;i<10;i++) jobs[i]=&j[i];
}

void load_workload4(struct job *jobs[]) {
  static struct job j[10] = {
    {1,0,25,0,0,0,0,0,0,0},{2,0,70,0,0,0,0,0,0,0},
    {3,0,13,0,0,0,0,0,0,0},{4,0,88,0,0,0,0,0,0,45},
    {5,0,31,0,0,0,0,0,0,0},{6,0,52,0,0,0,0,0,0,95},
    {7,0,47,0,0,0,0,0,0,0},{8,0,19,0,0,0,0,0,0,0},
    {9,0,66,0,0,0,0,0,0,130},{10,0,11,0,0,0,0,0,0,0}
  };
  for(int i=0;i<10;i++) jobs[i]=&j[i];
}

void load_workload5(struct job *jobs[]) {
  static struct job j[10] = {
    {1,0,77,0,0,0,0,0,0,0},{2,0,22,0,0,0,0,0,0,0},
    {3,0,39,0,0,0,0,0,0,15},{4,0,84,0,0,0,0,0,0,0},
    {5,0,16,0,0,0,0,0,0,0},{6,0,58,0,0,0,0,0,0,70},
    {7,0,93,0,0,0,0,0,0,0},{8,0,28,0,0,0,0,0,0,0},
    {9,0,45,0,0,0,0,0,0,105},{10,0,12,0,0,0,0,0,0,0}
  };
  for(int i=0;i<10;i++) jobs[i]=&j[i];
}

int main() {
  struct job *jobs[NUM_JOBS];

  void (*loads[5])(struct job *[]) = {
    load_workload1, load_workload2,
    load_workload3, load_workload4,
    load_workload5
  };

  for (int w = 0; w < 5; w++) {
    printf("\n========== Workload %d ==========\n", w+1);

    loads[w](jobs);
    reset_jobs(jobs);
    run_fifo(jobs);
    print_stats(jobs, "FIFO");

    loads[w](jobs);
    reset_jobs(jobs);
    run_sjf(jobs);
    print_stats(jobs, "SJF");

    loads[w](jobs);
    reset_jobs(jobs);
    run_rr(jobs);
    print_stats(jobs, "Round Robin");

    loads[w](jobs);
    reset_jobs(jobs);
    run_mlfq(jobs);
    print_stats(jobs, "MLFQ");
  }

  return 0;
}
