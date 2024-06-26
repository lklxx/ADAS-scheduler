#include "cmdline.h"
#include "ADASScheduler.hh"
#include "EDFScheduler.hh"

int main(int argc, char *argv[]) {
  cmdline::parser parser;
  parser.add<std::string>("input", 'i', "input data with test case", true, "");
  parser.add<std::string>("output", 'o', "output file for scheduling result", false, "");
  parser.add<int>("time", 't', "execution time for simulated annealing", false, 15);
  parser.parse_check(argc, argv);

  ADASScheduler scheduler(EDFScheduler);
  scheduler.parse_input_data(parser.get<std::string>("input"));
  scheduler.find_optimal_schedule(parser.get<int>("time"));
  scheduler.output_result(parser.get<std::string>("output"));

  return 0;
}
