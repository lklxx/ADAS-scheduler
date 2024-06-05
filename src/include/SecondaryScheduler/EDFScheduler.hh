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

void EDFScheduler(Schedule &sch) {
  int task_num = sch.tasks.size();
  std::vector<int> exec_time(task_num);
  std::vector<int> deadline(task_num);
  std::vector<std::queue<int>> remain_time(task_num);
  std::vector<std::queue<int>> next_deadline(task_num);

  for (int core = 0; core < sch.core_num; core++) {
    std::vector<Task> core_tasks;
    for (auto t : sch.tasks) {
      if (t.sch_core == core) {
        core_tasks.push_back(t);
      }
    }

    EventQueue events;
    int sim_length = sch.hyper_period * 2 + sch.max_offset;
    for (auto t : core_tasks) {
      int start = t.sch_offset;
      while (start < sim_length) {
        events.push(Event(t.index, start));
        start += t.period;
      }
    }
    events.push(Event(-1, INT_MAX));

    /* Start Simulation */
    std::fill(exec_time.begin(), exec_time.end(), 0);
    std::fill(deadline.begin(), deadline.end(), INT_MAX);
    Log log;
    int from = 0;

    while (true) {
      /* Find all candidate tasks  */
      while (events.top().time == from) {
        Event e = events.top();
        events.pop();
        int tid = e.tid;
        Task t = sch.tasks[tid];
        if (exec_time[tid] == 0) {
          deadline[tid] = e.time - t.sch_offset + t.sch_deadline;
        } else {
          remain_time[tid].push(remain_time[tid].empty() ? exec_time[tid] : t.time);
          next_deadline[tid].push(e.time - t.sch_offset + t.sch_deadline);
        }
        exec_time[tid] += t.time;

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
        int exec = to - from;
        exec_time[et] -= exec;
        while (!remain_time[et].empty()) {
          if (exec >= remain_time[et].front()) {
            exec -= remain_time[et].front();
            remain_time[et].pop();
            deadline[et] = next_deadline[et].front();
            next_deadline[et].pop();
          } else {
            remain_time[et].front() -= exec;
            break;
          }
        }
      } else if (to == INT_MAX) {
        break;
      }
      log.push_back(TimeSegment(et, from, to));
      from = to;
    }

    /* End Simulation */
    sch.logs.push_back(log);
  }

  print_logs(sch.logs);
  visualize_logs(sch, std::cout);
}
