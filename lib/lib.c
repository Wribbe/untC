#include "lib.h"

char BUFF_ERROR[SIZE_BUFF_ERROR] = {0};
GLfloat * DATA_MESHES[SIZE_DATA_MESHES] = {0};
size_t SIZE_MESHES[SIZE_DATA_MESHES] = {0};
GLuint GL_BUFFERS[SIZE_GL_BUFFERS] = {0};
GLuint GL_VERTEX_ATTRIBS[SIZE_GL_VERTEX_ATTRIBS] = {0};
struct m4 M4_TRANSFORMATION[SIZE_M4_TRANSFORMATION] = {0};
struct m4 m4_eye = {{{
  {1.0f, 0.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
}}};

GLuint program_shader = 0;

const char * source_shader_default_vert = \
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"uniform mat4 transform;\n"
"\n"
"void main()\n"
"{\n"
" gl_Position = transform * vec4(pos, 1.0);\n"
"}\n";

const char * source_shader_default_frag = \
"#version 330 core\n"
"out vec4 frag_color;\n"
"\n"
"void main()\n"
"{\n"
" frag_color = vec4(1.0);\n"
"}\n";

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

void init_opengl_buffers(void);
void init_default_shaders(void);
void init_opengl_vertex_attributes(void);
void create_triangle(void);
void feed_data(GLenum VAO, GLenum VBO, size_t id_mesh, GLenum type);
void m4_set(size_t index, struct m4 * m4);

GLenum
vert_attrib(size_t index)
{
  return GL_VERTEX_ATTRIBS[index];
}

GLenum
buffer(size_t index)
{
  return GL_BUFFERS[index];
}

size_t
mesh_size(size_t index)
{
  return SIZE_MESHES[index];
}

GLfloat *
mesh_data(size_t index)
{
  return DATA_MESHES[index];
}

struct obj_render obj_render = {0};

void
init_data(void)
{
  for (size_t i=0; i<SIZE_M4_TRANSFORMATION; i++) {
    m4_set(i, &m4_eye);
  }
}

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

  /* Initialize data structures. */
  init_data();

  pthread_mutex_unlock(&runner_data->mutex);

  if (!render_get(runner_data, RENDER_DISABLE_RENDERING)) {
    init_opengl_buffers();
    init_default_shaders();
    init_opengl_vertex_attributes();
    create_triangle();
    feed_data(vert_attrib(0), buffer(0), 0, GL_STATIC_DRAW);
    glUseProgram(program_shader);

    GLuint location_transform = glGetUniformLocation(program_shader,
        "transform");
    M4_TRANSFORMATION[0].x.w += 0.5f;
    glUniformMatrix4fv(location_transform, 1, GL_TRUE, M4_TRANSFORMATION[0].f[0]);

    obj_render.id_vao = vert_attrib(0);
    obj_render.id_program = program_shader;
    obj_render.id_transformation = 0;
  }

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    if (!render_get(data, RENDER_DISABLE_RENDERING)) {
      glBindVertexArray(vert_attrib(0));
      glDrawArrays(GL_TRIANGLES, 0, 3);
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
  glGenBuffers(SIZE_GL_BUFFERS, GL_BUFFERS);
}

bool
compile_shader(const char * source, GLuint * shader_id, GLenum type)
{
  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &source, NULL);
  glCompileShader(id);
  int success = 0;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(id, SIZE_BUFF_ERROR, NULL, BUFF_ERROR);
  }
  *shader_id = id;
  return success;
}

bool
link_shader_program(GLuint id, GLuint sh1, GLuint sh2)
{
  glAttachShader(id, sh1);
  glAttachShader(id, sh2);
  glLinkProgram(program_shader);

  int succ = 0;
  glGetProgramiv(id, GL_LINK_STATUS, &succ);
  if (!succ) {
    glGetProgramInfoLog(id, SIZE_BUFF_ERROR, NULL, BUFF_ERROR);
    return succ;
  }
  return succ;
}

void
init_default_shaders(void)
{
  bool succ = false;
  GLuint vert = 0;
  GLuint frag = 0;

  succ = compile_shader(source_shader_default_vert, &vert, GL_VERTEX_SHADER);
  if (!succ) {
    ERR_PRINT();
    return;
  }
  succ = compile_shader(source_shader_default_frag, &frag, GL_FRAGMENT_SHADER);
  if (!succ) {
    ERR_PRINT();
    return;
  }
  STATUS("%s\n", "Shaders compiled successfully.");

  program_shader = glCreateProgram();
  if (!link_shader_program(program_shader, vert, frag)) {
    ERR_PRINT();
  }

  STATUS("%s\n", "Shader program linked successfully.");
  glDeleteShader(vert);
  glDeleteShader(frag);
  STATUS("%s\n", "Deleted compiled shaders.");
}

void
init_opengl_vertex_attributes(void)
{
  glGenVertexArrays(SIZE_GL_VERTEX_ATTRIBS, GL_VERTEX_ATTRIBS);
  glBindVertexArray(vert_attrib(0));
}

void
create_triangle(void)
{
  DATA_MESHES[0] = malloc(sizeof(GLfloat) * 9);
  if (DATA_MESHES[0] == NULL) {
    ERR_WRITE("%s\n", "Could not allocate memory for meshes");
    ERR_PRINT();
  }
  struct v3 ps[] = {
    {{{0.0, 0.5, 0.0}}},
    {{{-0.5, -0.5, 0.0}}},
    {{{0.5, -0.5, 0.0}}},
  };
  polygon(DATA_MESHES[0], ps, 3);
  SIZE_MESHES[0] = sizeof(GLfloat)*9;
}

void
feed_data(GLenum VAO, GLenum VBO, size_t id_mesh, GLenum type)
{
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh_size(id_mesh), mesh_data(id_mesh), type);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);
}

void
m4_set(size_t index, struct m4 * m4)
{
  for (size_t i=0; i<4; i++) {
    for (size_t j=0; j<4; j++) {
      M4_TRANSFORMATION[index].f[i][j] = m4->f[i][j];
    }
  }
}
