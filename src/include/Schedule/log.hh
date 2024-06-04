#pragma once

struct TimeSegment {
  TimeSegment(int tid, int from, int to) : tid(tid), from(from), to(to) {}
  int tid;
  int from;
  int to;
};

using Log = std::vector<TimeSegment>;

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
