#include "lib.h"

int
main(void)
{

  GLFWwindow * window;

  if (!init_lib(&window)) {
    ERR_PRINT();
    return EXIT_FAILURE;
  }

  main_run(window, 0);
  return EXIT_SUCCESS;
}
