#include "lib.h"

int
main(void)
{

  GLFWwindow * window;

  if (!init_lib(&window)) {
    ERR_PRINT();
    return EXIT_FAILURE;
  }

  size_t total_frames = 0;
  main_run(window, 0, &total_frames);
  printf("Ran for %zu frames!\n", total_frames);
  return EXIT_SUCCESS;
}
