#include <stdlib.h>
#include <stdio.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

int
main(void)
{

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow * window = glfwCreateWindow(800, 600, "HELLO WORLD!", NULL, NULL);

  if (window == NULL) {
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  if (gl3wInit()) {
    exit(EXIT_FAILURE);
  }

  printf("OpenGL: %s, GLSL: %s\n", glGetString(GL_VERSION),
      glGetString(GL_SHADING_LANGUAGE_VERSION));

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
}
