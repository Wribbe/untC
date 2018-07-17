#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define SIZE_BUFF_ERROR 512
extern char BUFF_ERROR[SIZE_BUFF_ERROR];

#define mu_assert(test, message, ...) do { if (!(test)) { \
  snprintf(BUFF_ERROR, SIZE_BUFF_ERROR, message, __VA_ARGS__); \
  return false; }} while (0)
#define mu_run_test(test) do { bool success = test(); tests_run++; \
  if (!success) { return success; }} while (0)

#define ERR_PRINT(void) printf("%s\n", BUFF_ERROR);
#define ERR_WRITE(fmt, ...) \
  snprintf(BUFF_ERROR, SIZE_BUFF_ERROR, fmt, __VA_ARGS__);
#define OpenGLInfo(void) \
  printf("OpenGL: %s, GLSL: %s\n", glGetString(GL_VERSION), \
      glGetString(GL_SHADING_LANGUAGE_VERSION));

struct info_window_and_context {
  size_t height;
  size_t width;
  const char * title;
  int GL_MINOR;
  int GL_MAJOR;
  int GL_PROFILE;
};
extern struct info_window_and_context MAIN_CONTEXT;

struct main_run_data {
  GLFWwindow * window;
  size_t max_frames;
  size_t total_frames;
  bool pthread_init;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_t thread;
};

bool
init_lib(GLFWwindow ** window);

void
main_run(struct main_run_data * data);

void
main_wait(struct main_run_data * data);
