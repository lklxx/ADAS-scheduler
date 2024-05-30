#include "cmdline.h"

int main(int argc, char *argv[]) {
  cmdline::parser a;
  a.parse_check(argc, argv);

  return 0;
}
