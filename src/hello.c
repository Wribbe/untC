#include "lib.h"

int
main(void)
{

  GLFWwindow * window;

  if (!init_lib(&window)) {
    ERR_PRINT();
    return EXIT_FAILURE;
  }

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
}
