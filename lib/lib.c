#include "lib.h"

char BUFF_ERROR[SIZE_BUFF_ERROR];
GLfloat * DATA_MESHES[SIZE_DATA_MESHES];
GLuint GL_BUFFERS[SIZE_GL_BUFFERS];
GLuint GL_VERTEX_ATTRIBS[SIZE_GL_VERTEX_ATTRIBS];

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

void
init_opengl_buffers(void);

void *
main_runner(void * data)
{
  struct main_run_data * runner_data = (struct main_run_data *)(data);

  pthread_mutex_lock(&runner_data->mutex);

  if(!init_lib(&runner_data->window)) {
    ERR_WRITE("%s\n", "Could not initialize libraries");
    return NULL;
  }

  size_t num_frames = 0;
  size_t max_frames = runner_data->max_frames;
  GLFWwindow * window = runner_data->window;

  pthread_mutex_unlock(&runner_data->mutex);

  if (!render_get(runner_data, RENDER_HIDE_GEOMETRY)) {
    init_opengl_buffers();
  }

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    if (!render_get(data, RENDER_HIDE_GEOMETRY)) {
      glfwSwapBuffers(window);
    }
    num_frames++;
    if (max_frames > 0 && num_frames >= max_frames) {
      break;
    }
  }
  runner_data->total_frames = num_frames;
  glfwTerminate();
  glfwDestroyWindow(runner_data->window);
  return NULL;
}

void
main_run(struct main_run_data * data)
{
  if (!data->pthread_init) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->cond, NULL);
    data->pthread_init = true;
  }
  if (data->window == NULL) {
    data->window = window_create(NULL);
  }
  pthread_mutex_lock(&data->mutex);
  pthread_create(&data->thread, NULL, main_runner, data);
  pthread_mutex_unlock(&data->mutex);
}

void
main_wait(struct main_run_data * data)
{
  pthread_join(data->thread, NULL);
}

void
polygon(GLfloat * data, struct v3 * point_list, size_t num_points)
{
  for (size_t i=0; i<num_points; i++) {
    for (size_t j= 0; j<3; j++) {
      data[i*3+j] = point_list[i].f[j];
    }
  }
}

bool
render_get(struct main_run_data * data, size_t flags) {
  return data->flags & flags;
}

void
render_set(struct main_run_data * data, size_t flags) {
  data->flags |= flags;
}

void
render_unset(struct main_run_data * data, size_t flags) {
  data->flags &= flags;
}

void
init_opengl_buffers(void)
{
}
