#include "lib.h"

int
main(void)
{
  struct main_run_data data = {0};
  main_run(&data);
  pthread_join(data.thread, NULL);

  printf("Ran for %zu frames!\n", data.total_frames);
  return EXIT_SUCCESS;
}
