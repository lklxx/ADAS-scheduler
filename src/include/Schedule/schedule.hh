#pragma once

struct Task {
  int index;

  // Input constraints
  int core;			// -1 if unspecified
  int release;
  int offset;			// only used for init_sol()
  int time;
  int period;
  int deadline;

  // Solution result
  int sch_core;
  int sch_offset;

  // Schedule result
  std::vector<int> start_time;
  std::vector<int> finish_time;
  int jitter;
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

struct Solution {
  std::vector<Task> tasks;
  std::vector<TaskChain> task_chains;
  int core_num;			// 0 ~ core_num - 1
  float cost;			// only used in Schedule

  int hyper_period = -1;	// lcm of periods
  int max_offset = -1;
  std::vector<Log> logs;	// only used in Schedule
};

typedef Solution Schedule;
