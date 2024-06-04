#include <climits>
#include <queue>

#include "log.hh"
#include "schedule.hh"

struct Event {
  Event(int tid, int time) : tid(tid), time(time) {}
  int tid;
  int time;
};

struct EventPrior {
  bool operator()(const Event l, const Event r) const {
    if (l.time != r.time) {
      return l.time > r.time;
    }
    return l.tid > r.tid;
  }
};

using EventQueue = std::priority_queue<Event, std::vector<Event>, EventPrior>;

Schedule EDFScheduler(Solution sol) {
  Schedule sch = sol;

  int task_num = sch.tasks.size();
  std::vector<int> exec_time(task_num);
  std::vector<int> deadline(task_num);

  for (int core = 0; core < sch.core_num; core++) {
    std::vector<Task> core_tasks;
    for (auto t : sch.tasks) {
      if (t.core == core) {
        core_tasks.push_back(t);
      }
    }

    EventQueue events;
    int sim_length = sch.hyper_period * 2 + sch.max_offset;
    for (auto t : core_tasks) {
      int start = t.offset;
      while (start < sim_length) {
        events.push(Event(t.index, start));
        start += t.period;
      }
    }

    /* Start Simulation */
    std::fill(exec_time.begin(), exec_time.end(), 0);
    std::fill(deadline.begin(), deadline.end(), INT_MAX);
    std::vector<TimeSegment> sch_log;
    int from = 0;

    while (!events.empty()) {
      /* Find all candidate tasks  */
      while (events.top().time == from) {
        Event e = events.top();
        events.pop();
        int tid = e.tid;
        Task t = sch.tasks[tid];
        exec_time[tid] = t.time; // TODO: what if exec_time[tid] != 0 ?
        deadline[tid] = e.time - t.offset + t.deadline;

        if (events.empty()) {
          break;
        }
      }

      /* Find the task with earliest deadline */
      int ed = INT_MAX - 1;	// earliest deadline
      int et = -1;		// task with earliest deadline
      for (int tid = 0; tid < task_num; tid++) {
        if (exec_time[tid] == 0) {
          continue;
        }
        if (deadline[tid] < ed) {
          ed = deadline[tid];
          et = tid;
        }
      }

      /* Run the task (or idle) until the next event */
      int to = events.empty() ? INT_MAX : events.top().time;
      if (et != -1) {
        to = std::min(to, from + exec_time[et]);
        exec_time[et] -= to - from;
      }
      sch_log.push_back(TimeSegment(et, from, to));
      from = to;
    }

    /* End Simulation */
    std::cout << "Core " << core << std::endl;
    print_log(sch_log);
  }

  return sch;
}
