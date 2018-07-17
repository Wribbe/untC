#include "lib.h"

int
main(void)
{
  struct main_run_data data = {0};
  main_run(&data);
  main_wait(&data);

  printf("Ran for %zu frames!\n", data.total_frames);
  return EXIT_SUCCESS;
}
