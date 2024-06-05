#include <cassert>
#include <climits>
#include <fstream>
#include <numeric>

#include "schedule.hh"

class ADASScheduler {
public:
  ADASScheduler() {}

  void parse_input_data(std::string input_data) {
    std::ifstream ifs(input_data);
    ifs >> curr_sol.core_num;

    int task_num;
    ifs >> task_num;
    for (int i = 0; i < task_num; i++) {
      Task t;
      t.index = i;
      ifs >> t.core >> t.release >> t.offset
          >> t.time >> t.period >> t.deadline >> t.jitter;
      curr_sol.tasks.push_back(t);
    }

    int task_chain_num;
    ifs >> task_chain_num;
    for (int i = 0; i < task_chain_num; i++) {
      TaskChain tc;
      tc.index = i;
      ifs >> task_num;
      for (int j = 0; j < task_num; j++) {
        int task_id;
	ifs >> task_id;
	tc.tasks.push_back(curr_sol.tasks[task_id]);
      }
      ifs >> tc.deadline >> tc.priority;
      curr_sol.task_chains.push_back(tc);
    }

    print_sol(curr_sol);
    best_sol = curr_sol;
  }

  template <typename Scheduler>
  void simulated_annealing(Scheduler scheduler, int exec_time) {
    init_sol();
    curr_sch = scheduler(curr_sol);
    calculate_cost(curr_sch);
    best_sch = curr_sch;
  }

  void output_result(std::string output_file) {
    std::cout << output_file << std::endl;
  }

  void print_task(Task &t) {
    std::cout << "task " << t.index << ": (";
    std::cout << t.core << ", " << t.release << ", " << t.offset << ", "
              << t.time << ", " << t.period << ", " << t.deadline << "," << t.jitter << ")\n";
  }

  void print_task_chain(TaskChain &tc) {
    std::cout << "task chain " << tc.index << ": ([\n";
    for (auto t : tc.tasks) {
      std::cout << "  ";
      print_task(t);
    }
    std::cout << "], " << tc.deadline << ", " << tc.priority << ")\n";
  }

  void print_sol(Solution &sol) {
    std::cout << "Tasks:\n";
    for (auto t : sol.tasks) {
      print_task(t);
    }
    std::cout << "\nTask Chains:\n";
    for (auto tc : sol.task_chains) {
      print_task_chain(tc);
    }
    std::cout << "\nhyper period: " << sol.hyper_period;
    std::cout << "\nmax offset: " << sol.max_offset << "\n\n";
  }

private:
  void init_sol() {
    curr_sol.hyper_period = 1;
    curr_sol.max_offset = 0;
    for (auto &t : curr_sol.tasks) {
      curr_sol.hyper_period = std::lcm(curr_sol.hyper_period, t.period);
      curr_sol.max_offset = std::max(curr_sol.max_offset, t.offset);
      t.sch_core = t.core;
      t.sch_offset = t.offset;
      t.sch_deadline = t.deadline;
    }

    std::vector<int> core_util(curr_sol.core_num, 0);
    for (auto &t : curr_sol.tasks) {
      if (t.sch_core == -1) {
        continue;
      }
      core_util[t.sch_core] += t.time * (curr_sol.hyper_period / t.period);
      assert(core_util[t.sch_core] <= curr_sol.hyper_period);
    }
    for (auto &t : curr_sol.tasks) {
      if (t.sch_core != -1) {
        continue;
      }
      int min_util_core = std::distance(std::begin(core_util),
                                        std::ranges::min_element(core_util));
      t.sch_core = min_util_core;
      core_util[min_util_core] += t.time * (curr_sol.hyper_period / t.period);
    }

    print_sol(curr_sol);
  }

  void calculate_cost(Schedule &sch) {
    std::vector<Log> task_logs(sch.tasks.size());

    for (auto log : sch.logs) {
      for (auto ts : log) {
        if (ts.tid != -1) {
          task_logs[ts.tid].push_back(ts);
        }
      }
    }

    for (size_t tid = 0; tid < sch.tasks.size(); tid++) {
      int cur = 0;
      for (auto ts : task_logs[tid]) {
        if (cur == 0) {
          sch.tasks[tid].start_time.push_back(ts.from);
        }
        int exec = ts.to - ts.from;
        int time = sch.tasks[tid].time;
        int from = ts.from;
        while (cur + exec >= time) {
          int t = time - cur;
          sch.tasks[tid].finish_time.push_back(from + t);
          from += t;
          exec -= t;
          cur = 0;
          if (exec > 0) {
            sch.tasks[tid].start_time.push_back(from);
          }
        }
        cur += exec;
      }

      std::cout << "Task " << tid << ":\n";
      for (size_t i = 0; i < sch.tasks[tid].start_time.size(); i++) {
        std::cout << "(" << sch.tasks[tid].start_time[i]
                  << ", " << sch.tasks[tid].finish_time[i] << "), ";
      }
      std::cout << std::endl;
    }

    for (auto t : sch.tasks) {
      int max = 0;
      int min = INT_MAX;

      for (size_t i = 0; i < t.start_time.size(); i++) {
        int offset = t.start_time[i] - t.period * i;
	max = std::max(max, offset);
	min = std::min(min, offset);
      }
      t.sch_jitter = max - min;

      max = 0;
      min = INT_MAX;
      for (size_t i = 0; i < t.finish_time.size(); i++) {
        int offset = t.finish_time[i] - t.period * i;
        max = std::max(max, offset);
        min = std::min(min, offset);
      }
      t.sch_jitter = std::max(t.sch_jitter, max - min);
      if (max > t.deadline) {
        float violation = std::min(max - t.deadline, t.deadline);
        sch.cost.deadline_t += violation / t.deadline / sch.tasks.size();
      }
      if (t.sch_jitter > 0) {
        t.sch_jitter = std::min(t.sch_jitter, t.jitter);
        sch.cost.jitter += static_cast<float>(t.sch_jitter) / t.jitter / sch.tasks.size();
      }
    }

    sync_tasks(sch);

    for (auto tc : sch.task_chains) {
      auto first = tc.tasks[0];
      std::vector<int> next(sch.tasks.size(), 0);

      for (size_t i = 0; i < first.start_time.size() / 2; i++) {
        int start = first.start_time[i];
        tc.start_time.push_back(start);
        int cur = start;
        for (auto t : tc.tasks) {
          int tid = t.index;
          while (sch.tasks[tid].start_time[next[tid]] < cur) {
            next[tid]++;
            if (static_cast<size_t>(next[tid]) == sch.tasks[tid].start_time.size()) {
              sch.tasks[tid].start_time.push_back(INT_MAX);
              sch.tasks[tid].finish_time.push_back(INT_MAX);
              break;
            }
          }
          cur = sch.tasks[tid].finish_time[next[tid]];
        }
        tc.finish_time.push_back(cur);
      }

      std::cout << "Task Chain " << tc.index << ":\n";
      for (size_t i = 0; i < tc.start_time.size(); i++) {
        std::cout << "(" << tc.start_time[i]
                  << ", " << tc.finish_time[i] << "), ";
      }
      std::cout << std::endl;

      int max_latency = 0;
      for (size_t i = 0; i < tc.start_time.size(); i++) {
        max_latency = std::max(max_latency, tc.finish_time[i] - tc.start_time[i]);
      }
      if (max_latency > tc.deadline) {
        float violation = std::min(max_latency - tc.deadline, tc.deadline);
        sch.cost.deadline_tc += violation / tc.deadline / sch.task_chains.size();
      } else {
        sch.cost.latency += tc.priority * max_latency / tc.deadline / sch.task_chains.size();
      }
    }

    sch.cost.final_cost = w1 + sch.cost.deadline_tc * w2 +
                          sch.cost.deadline_t * w3 + sch.cost.jitter * w4;
    if (sch.cost.final_cost == w1) {
      sch.cost.final_cost *= sch.cost.latency;
    }

    std::cout << "cost: " << std::endl;
    std::cout << "  latency: " << sch.cost.latency << std::endl;
    std::cout << "  deadline_t: " << sch.cost.deadline_t << std::endl;
    std::cout << "  deadline_tc: " << sch.cost.deadline_tc << std::endl;
    std::cout << "  jitter: " << sch.cost.jitter << std::endl;
    std::cout << "  final_cost: " << sch.cost.final_cost << std::endl;
  }

  void sync_tasks(Schedule &sch) {
    for (auto &tc : sch.task_chains) {
      for (auto &t : tc.tasks) {
        t = sch.tasks[t.index];
      }
    }
  }

private:
  Solution curr_sol, best_sol;
  Schedule curr_sch, best_sch;
  float w1 = 10000;
  float w2 = 40000;
  float w3 = 10000;
  float w4 = 60000;
};
