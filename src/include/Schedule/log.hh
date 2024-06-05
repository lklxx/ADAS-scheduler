#include <iomanip>
#include "schedule.hh"

void print_logs(std::vector<Log> &logs) {
  for (size_t core = 0; core < logs.size(); core++) {
    std::cout << "Core " << core << std::endl;
    auto log = logs[core];
    for (auto ts : log) {
      if (ts.tid == -1) {
        std::cout << "Idle";
      } else {
        std::cout << "Task " << ts.tid;
      }
      std::cout << "\tfrom " << ts.from << "\tto " << ts.to << std::endl;
    }
    std::cout << std::endl;
  }
}

void print_sch_blocks(Log log, int sim_length) {
  std::cout << "|";
  int cur_time = 0;
  auto repeat_str = [](std::string str, int n) {
    for (int i = 0; i < n; i++) std::cout << str;
  };
  for (size_t i = 0; i < log.size(); i++) {
    auto ts = log[i];
    repeat_str("-", ts.from - cur_time);
    repeat_str("█", ts.to - ts.from);
    cur_time = ts.to;
  }
  repeat_str("-", sim_length - cur_time);
  std::cout << "|";
}

void print_timeline(int sim_length) {
  if (sim_length >= 100) {
    std::cout << "           ";
    for (int t = 0; t <= sim_length; t += 5) {
      if (t >= 100) {
        std::cout << t / 100;
      } else {
        std::cout << " ";
      }
      std::cout << "    ";
    }
    std::cout << std::endl;
  }
  if (sim_length >= 10) {
    std::cout << "           ";
    for (int t = 0; t <= sim_length; t += 5) {
      if (t >= 10) {
        std::cout << (t / 10) % 10;
      } else {
        std::cout << " ";
      }
      std::cout << "    ";
    }
    std::cout << std::endl;
  }
  std::cout << "           ";
  for (int t = 0; t <= sim_length; t += 5) {
    if (t % 10) {
      std::cout << 5;
    } else {
      std::cout << 0;
    }
    std::cout << "    ";
  }
  std::cout << std::endl;
}

void visualize_logs(Schedule &sch, std::ostream &out) {
  int core_num = sch.core_num;
  int sim_length = sch.hyper_period * 2 + sch.max_offset;
  std::vector<std::vector<int>> tid_lists(core_num);
  std::vector<Log> task_logs(sch.tasks.size());

  for (auto t : sch.tasks) {
    tid_lists[t.sch_core].push_back(t.index);
  }

  for (auto log : sch.logs) {
    for (auto ts : log) {
      if (ts.tid != -1) {
        task_logs[ts.tid].push_back(ts);
      }
    }
  }

  for (int core = 0; core < core_num; core++) {
    std::cout << "Core " << core << ":\n\n";
    for (int tid : tid_lists[core]) {
      std::cout << std::setw(10) << std::left
                << ("Task " + std::to_string(tid));
      print_sch_blocks(task_logs[tid], sim_length);
      std::cout << std::endl;
    }
    print_timeline(sim_length);
    std::cout << std::endl;
  }
}
