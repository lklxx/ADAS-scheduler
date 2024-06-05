#pragma once

struct Task {
  int index;

  // Input constraints
  int core;			// -1 if unspecified
  int release;
  int offset;			// only used for init_sch()
  int time;
  int period;
  int deadline;
  int jitter;

  // Solution result
  int sch_core;
  int sch_offset;
  int sch_deadline;

  // Schedule result
  std::vector<int> start_time;
  std::vector<int> finish_time;
  int sch_jitter;
};

struct TaskChain {
  int index;

  // Input constraints
  std::vector<Task> tasks;
  int deadline;
  float priority;

  // Schedule result
  int period;			// lcm of task periods
  std::vector<int> start_time;
  std::vector<int> finish_time;
};

struct TimeSegment {
  TimeSegment(int tid, int from, int to) : tid(tid), from(from), to(to) {}
  int tid;
  int from;
  int to;
};

using Log = std::vector<TimeSegment>;

struct Schedule {
  std::vector<Task> tasks;
  std::vector<TaskChain> task_chains;
  int core_num;			// 0 ~ core_num - 1
  struct {
    float latency = 0;
    float deadline_t = 0;
    float deadline_tc = 0;
    float jitter = 0;
    float final_cost = 0;
  } cost;

  int hyper_period = -1;	// lcm of periods
  int max_offset = -1;
  std::vector<Log> logs;
  std::vector<int> core_violations;
  int jitter_violations = 0;
};
