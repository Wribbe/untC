#include "lib.h"

int
main(void)
{

  GLFWwindow * window;

  if (!init_lib(&window)) {
    ERR_PRINT();
    return EXIT_FAILURE;
  }

  struct main_run_data data = MAIN_RUN_DATA();
  main_run(&data);
  pthread_join(data.thread, NULL);

  printf("Ran for %zu frames!\n", data.total_frames);
  return EXIT_SUCCESS;
}
