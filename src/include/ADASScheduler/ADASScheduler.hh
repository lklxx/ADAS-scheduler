#include "schedule.hh"

class ADASScheduler {
public:
  ADASScheduler() {}

  void parse_input_data(std::string input_data) {
    std::cout << input_data << std::endl;
  }

  template <class Scheduler>
  void simulated_annealing(int exec_time) {
    init_sol();
    curr_sch = best_sch = Scheduler::schedule_synthesis(curr_sol);
  }

  void output_result(std::string output_file) {
    std::cout << output_file << std::endl;
  }

private:
  void init_sol() { }

private:
  Solution curr_sol, best_sol;
  Schedule curr_sch, best_sch;
  int curr_cost, best_cost;
};
