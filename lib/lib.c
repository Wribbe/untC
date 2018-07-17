#include "lib.h"

char BUFF_ERROR[SIZE_BUFF_ERROR];

struct info_window_and_context MAIN_CONTEXT = {
  800,
  600,
  "HELLO WORLD",
  3,
  3,
  GLFW_OPENGL_CORE_PROFILE,
};

GLFWwindow *
window_create(struct info_window_and_context * context) {
  if (context == NULL) {
    context = &MAIN_CONTEXT;
  }
  GLFWwindow * window = glfwCreateWindow(context->height,
      context->width,
      context->title,
      NULL,
      NULL);
  return window;
}


bool
init_lib(GLFWwindow ** window)
{
  if (!glfwInit()) {
    ERR_WRITE("%s\n", "Could not initialize GLFW");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAIN_CONTEXT.GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MAIN_CONTEXT.GL_MINOR);
  glfwWindowHint(GLFW_OPENGL_PROFILE, MAIN_CONTEXT.GL_PROFILE);

  GLFWwindow * local_window = window_create(NULL);

  if (local_window == NULL) {
    ERR_WRITE("%s\n", "Could not initialize window");
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(local_window);
  if (gl3wInit()) {
    ERR_WRITE("%s\n", "Could not initialize gl3w");
    glfwTerminate();
    return false;
  }
  if (window != NULL) {
    *window = local_window;
  } else {
    glfwTerminate();
  }
  return true;
}

void *
main_runner(void * data)
{
  struct main_run_data * runner_data = (struct main_run_data *)(data);

  pthread_mutex_lock(&runner_data->mutex);

  size_t num_frames = 0;
  size_t max_frames = runner_data->max_frames;
  GLFWwindow * window = runner_data->window;

  pthread_mutex_unlock(&runner_data->mutex);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    glfwSwapBuffers(window);
    num_frames++;
    if (max_frames > 0 && num_frames >= max_frames) {
      break;
    }
  }
  runner_data->total_frames = num_frames;
  return NULL;
}

void
main_run(struct main_run_data * data)
{
  pthread_mutex_lock(&data->mutex);
  pthread_create(&data->thread, NULL, main_runner, data);
  pthread_mutex_unlock(&data->mutex);
}

void
main_wait(void)
{
}
