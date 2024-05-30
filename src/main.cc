#include "cmdline.h"
#include "ADASScheduler.hh"

int main(int argc, char *argv[]) {
  cmdline::parser parser;
  parser.parse_check(argc, argv);

  ADASScheduler scheduler;

  return 0;
}
