#include <cassert>
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
          >> t.time >> t.period >> t.deadline;
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
    curr_sch = best_sch = scheduler(curr_sol);
  }

  void output_result(std::string output_file) {
    std::cout << output_file << std::endl;
  }

  void print_task(Task &t) {
    std::cout << "task " << t.index << ": (";
    std::cout << t.core << ", " << t.release << ", " << t.offset << ", "
              << t.time << ", " << t.period << ", " << t.deadline << ")\n";
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
  }

private:
  void init_sol() {
    curr_sol.hyper_period = 1;
    for (auto &t : curr_sol.tasks) {
      curr_sol.hyper_period = std::lcm(curr_sol.hyper_period, t.period);
    }

    std::vector<int> core_util(curr_sol.core_num, 0);
    for (auto &t : curr_sol.tasks) {
      if (t.core == -1) {
        continue;
      }
      core_util[t.core] += t.time * (curr_sol.hyper_period / t.period);
      assert(core_util[t.core] <= curr_sol.hyper_period);
    }
    for (auto &t : curr_sol.tasks) {
      if (t.core != -1) {
        continue;
      }
      int min_util_core = std::distance(std::begin(core_util),
                                        std::ranges::min_element(core_util));
      t.core = min_util_core;
      core_util[min_util_core] += t.time * (curr_sol.hyper_period / t.period);
    }

    print_sol(curr_sol);
  }

private:
  Solution curr_sol, best_sol;
  Schedule curr_sch, best_sch;
  int curr_cost, best_cost;
};
