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

char STRING_RING_BUFFER[SIZE_STRING_RING_BUFFER] = {0};
char * str_ring_start = STRING_RING_BUFFER;
char * str_ring_current = STRING_RING_BUFFER;
char * str_ring_end = STRING_RING_BUFFER + (SIZE_STRING_RING_BUFFER-1);

struct v3 CLICK_BUFFER[SIZE_CLICK_BUFFER] = {0};
struct v3 * click_buffer_start = CLICK_BUFFER;
struct v3 * click_buffer_current = CLICK_BUFFER;
struct v3 * click_buffer_last = CLICK_BUFFER+(SIZE_CLICK_BUFFER-1);

GLuint program_shader = 0;
GLuint program_screenquad = 0;
GLuint id_mesh_screenquad = 1;

enum id_vaos {
  id_vao_default,
  id_vao_screenquad,
};

enum id_vbos {
  id_vbo_default,
  id_vbo_screenquad,
};

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

const char * source_shader_screenquad_vert = \
"#version 330 core\n"
"layout (location = 0) in vec2 in_pos;\n"
"layout (location = 1) in vec2 in_texcoords;\n"
"\n"
"out vec2 vert_texcoords;\n"
"\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);\n"
"   vert_texcoords = in_texcoords;\n"
"}\n";

const char * source_shader_screenquad_frag = \
"#version 330 core\n"
"in vec2 vert_texcoords;\n"
"\n"
"uniform sampler2D texture_screen;\n"
"\n"
"out vec4 fragment_color;\n"
"\n"
"void main()\n"
"{\n"
"   fragment_color = texture(texture_screen, vert_texcoords);\n"
"   //fragment_color = vec4(vert_texcoords, 0.0f, 1.0f);\n"
"}\n";

GLfloat vertices_screenquad[] = {
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f, -1.0f,  1.0f, 0.0f,

  -1.0f,  1.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f,
};

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
VAO(size_t index)
{
  return GL_VERTEX_ATTRIBS[index];
}

GLenum
VBO(size_t index)
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

void
mesh_set_size(size_t index, size_t size)
{
  SIZE_MESHES[index] = size;
}

void mesh_data_deallocate(size_t index);

GLfloat *
mesh_set_data(size_t index, GLfloat * value)
{
  if (value == NULL) {
    DATA_MESHES[index] = NULL;
  } else {
    DATA_MESHES[index] = value;
  }
  return DATA_MESHES[index];
}

void *
mesh_set_data_copy(size_t index, GLfloat * value, size_t size)
{
  return memcpy(mesh_data(index), value, size);
}

struct obj_render obj_render = {0};

void obj_transfomation_reset(size_t id_transformation);

void
init_data(void)
{
  for (size_t i=0; i<SIZE_M4_TRANSFORMATION; i++) {
    obj_transfomation_reset(i);
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
    feed_data(VAO(0), VBO(0), 0, GL_STATIC_DRAW);
    glUseProgram(program_shader);

    GLuint location_transform = glGetUniformLocation(program_shader,
        "transform");
//    struct v3 move_triangle = {{{0.5f, 0.5f, 0.0f}}};
//    obj_translate(0, &move_triangle);
    glUniformMatrix4fv(location_transform, 1, GL_TRUE,
        M4_TRANSFORMATION[0].f[0]);
    glUseProgram(0);

//    mesh_data_allocate(id_mesh_screenquad, sizeof(vertices_screenquad));
//    for (size_t i=0; i<LEN(vertices_screenquad); i++) {
//      mesh_data(id_mesh_screenquad)[i] = vertices_screenquad[i];
//    }
    glBindVertexArray(VAO(id_vao_screenquad));
    glBindBuffer(GL_ARRAY_BUFFER, VBO(id_vbo_screenquad));
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_screenquad),
        &vertices_screenquad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
        (void*)(2*sizeof(GLfloat)));
    glBindVertexArray(0);

    obj_render.id_vao = VAO(0);
    obj_render.id_program = program_shader;
    obj_render.id_transformation = 0;

  }

  GLuint fbo = 0;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  GLuint texture_colorbuffer = 0;
  glGenTextures(1, &texture_colorbuffer);
  glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MAIN_CONTEXT.width,
      MAIN_CONTEXT.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
      texture_colorbuffer, 0);

  GLuint rbo = 0;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
      MAIN_CONTEXT.width, MAIN_CONTEXT.height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    ERR_WRITE("%s\n", "Framebuffer was not ready, aborting.");
    ERR_PRINT();
    return NULL;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    if (!render_get(data, RENDER_DISABLE_RENDERING)) {

      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glUseProgram(program_shader);
      glBindVertexArray(VAO(0));
      glDrawArrays(GL_TRIANGLES, 0, 3);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      glUseProgram(program_screenquad);
      GLuint location_u_texture_screen = glGetUniformLocation(
          program_screenquad,
          "texture_screen"
      );
      glUniform1i(location_u_texture_screen, 0);
      glBindVertexArray(VAO(id_mesh_screenquad));
      glDisable(GL_DEPTH_TEST);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_colorbuffer);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindTexture(GL_TEXTURE_2D, 0);

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
  glLinkProgram(id);

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
  STATUS("%s\n", "Default shaders compiled successfully.");

  program_shader = glCreateProgram();
  if (!link_shader_program(program_shader, vert, frag)) {
    ERR_PRINT();
    return;
  }

  STATUS("%s\n", "Default shader program linked successfully.");
  glDeleteShader(vert);
  glDeleteShader(frag);
  STATUS("%s\n", "Deleted compiled default shaders.");

  succ = compile_shader(source_shader_screenquad_vert, &vert, GL_VERTEX_SHADER);
  if (!succ) {
    ERR_PRINT();
    return;
  }
  succ = compile_shader(source_shader_screenquad_frag, &frag, GL_FRAGMENT_SHADER);
  if (!succ) {
    ERR_PRINT();
    return;
  }
  STATUS("%s\n", "Screen-quad shaders compiled successfully.");

  program_screenquad = glCreateProgram();
  if (!link_shader_program(program_screenquad, vert, frag)) {
    ERR_PRINT();
    return;
  }

  STATUS("%s\n", "Screen-quad shader program linked successfully.");
  glDeleteShader(vert);
  glDeleteShader(frag);
  STATUS("%s\n", "Deleted compiled default shaders.");
}


void
init_opengl_vertex_attributes(void)
{
  glGenVertexArrays(SIZE_GL_VERTEX_ATTRIBS, GL_VERTEX_ATTRIBS);
  glBindVertexArray(VAO(0));
}

void
mesh_data_deallocate(size_t index)
{
  if (mesh_data(index) != NULL) {
    free(mesh_data(index));
    mesh_set_data(index, NULL);
    mesh_set_size(index, 0);
  }
}


void *
mesh_data_allocate(size_t index, size_t size)
{
  if (mesh_data(index) != NULL) {
    mesh_data_deallocate(index);
  }
  GLfloat * data = malloc(size);
  if (data == NULL) {
    mesh_set_data(index, NULL);
    mesh_set_size(index, 0);
    return NULL;
  }
  mesh_set_data(index, data);
  mesh_set_size(index,size);
  return data;
}


void
create_triangle(void)
{
  GLfloat * data = mesh_data_allocate(0, sizeof(GLfloat)*9);
  if (data == NULL) {
    ERR_WRITE("%s\n", "Could not allocate memory for meshes");
    ERR_PRINT();
  }
  struct v3 ps[] = {
    {{{0.0, 0.5, 0.0}}},
    {{{-0.5, -0.5, 0.0}}},
    {{{0.5, -0.5, 0.0}}},
  };
  polygon(mesh_data(0), ps, 3);
}

void
feed_data(GLenum VAO, GLenum VBO, size_t id_mesh, GLenum type)
{
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh_size(id_mesh), mesh_data(id_mesh), type);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
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

struct m4 * transformation_get(size_t id_transformation);

void
obj_translate(size_t id_transformation, struct v3 * delta)
{
  struct m4 * t = transformation_get(id_transformation);
  t->x.w = delta->x;
  t->y.w = delta->y;
  t->z.w = delta->z;
}

struct v3
obj_pos(size_t id_transformation)
{
  struct m4 * t = transformation_get(id_transformation);
  return (struct v3){{{
    t->x.w,
    t->y.w,
    t->z.w,
  }}};
}

const char *
v3_str(struct v3 * v3)
{
  /* Format: { 0.0f, 0.0f, 0.0f } */
  size_t num_chars = 20;
  if (str_ring_current + num_chars > str_ring_end) {
    str_ring_current = str_ring_start;
  }
  snprintf(str_ring_current, num_chars, "{ %.1f, %.1f, %.1f }",
      v3->x, v3->y, v3->z);
  const char * return_str = str_ring_current;
  str_ring_current += num_chars;
  return return_str;
}

bool
v3_eq(struct v3 * v1, struct v3 * v2)
{
  for (size_t i=0; i<3; i++) {
    if (v1->f[i] != v2->f[i]) {
      return false;
    }
  }
  return true;
}

void
obj_transfomation_reset(size_t id_transformation)
{
  m4_set(id_transformation, &m4_eye);
}

struct m4 *
transformation_get(size_t id_transformation)
{
  return &M4_TRANSFORMATION[id_transformation];
}

struct v3 *
click_next(struct v3 * click_pointer)
{
  if (click_pointer+1 > click_buffer_last) {
    return click_buffer_start;
  }
  return click_pointer+1;
}

struct v3 *
click_rewind(struct v3 * pointer, size_t num_points)
{
  if (pointer - num_points < click_buffer_start) {
    /* Remove all points possible without looping around. */
    num_points -= (pointer-click_buffer_start);
    /* Loop around, subtract additional point. */
    pointer = click_buffer_last;
    num_points--;
    /* Call this function recursively. */
    return click_rewind(pointer, num_points);
  }
  return pointer - num_points;
}

struct v3 *
click_save(GLfloat x, GLfloat y)
{
  click_buffer_current->x = x;
  click_buffer_current->y = y;

  struct v3 * v3_ptr = click_buffer_current;
  click_buffer_current = click_next(click_buffer_current);
  return v3_ptr;
}

void
polygon_from_clicks(GLfloat * data, size_t num_points)
{
  struct v3 * current = click_rewind(click_buffer_current, num_points);
  for (size_t i=0; i<num_points; i++) {
    memcpy(data+(i*3), current->f, sizeof(struct v3));
    current = click_next(current);
  }
}

const char base64_encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklm"\
                                     "nopqrstuvwxyz0123456789+/";

char *
base64_encode(const char * src, size_t len_in, size_t * len_out)
{
  size_t num_bytes_in_chunk = 3;
  size_t num_chunks_full = len_in/num_bytes_in_chunk;
  size_t num_bytes_remaining = len_in%num_bytes_in_chunk;

  size_t num_base64_char_per_chunk = 4;
  size_t num_base64_char = num_chunks_full*num_base64_char_per_chunk;
  if (num_bytes_remaining > 0) {
    num_base64_char += num_base64_char_per_chunk;
  }
  if (len_out != NULL) {
    *len_out = num_base64_char;
  }

  char * ret = malloc(sizeof(char)*(num_base64_char+1));
  char * current_out = ret;
  const char * current_in = src;
  while(current_in < src+num_chunks_full*num_bytes_in_chunk) {
    /* First 6 bytes by shifting first 8 two steps to the right. */
    *current_out++ = base64_encoding_table[current_in[0] >> 2];
    /* Remaining 2 bytes + 4 bytes of next char. */
    *current_out++ = base64_encoding_table[(current_in[0] & 0x03) << 4 | current_in[1] >> 4];
    /* Remaining 4 bytes + 2 bytes of next char. */
    *current_out++ = base64_encoding_table[(current_in[1] & 0x0F) << 2 | current_in[2] >> 6];
    /* Remaining 6 bytes. */
    *current_out++ = base64_encoding_table[current_in[2] & 0x3F];
    /* Advance to next byte-chunk. */
    current_in += num_bytes_in_chunk;
  }
  size_t num_non_padding_bytes = num_base64_char_per_chunk-\
                                 (num_bytes_in_chunk-num_bytes_remaining);

  if (num_non_padding_bytes > 0) {
    *current_out++ = base64_encoding_table[current_in[0] >> 2];
  }
  if (num_non_padding_bytes > 1) {
    *current_out++ = base64_encoding_table[(current_in[0] & 0x03) << 4 | current_in[1] >> 4];
  }
  if (num_non_padding_bytes > 2) {
    *current_out++ = base64_encoding_table[(current_in[1] & 0x0F) << 2 | current_in[2] >> 6];
  }
  while(current_out < ret+num_base64_char) {
    *current_out = '=';
    current_out++;
  }
  ret[num_base64_char] = '\0';
  return ret;
}

bool bool_base64_decodetable_init = false;
#define SIZE_BASE64_DECODETABLE 256
#define BASE64_UNDEF 0
char base64_decode_table[SIZE_BASE64_DECODETABLE] = {BASE64_UNDEF};

char *
base64_decode(const char * src, size_t len_in, size_t * len_out)
{
  size_t len_decoded = len_in;
  size_t num_buffer_chars = 0;
  for (const char * srcp = src+len_in-1; *srcp == '='; srcp--) {
    len_decoded--;
    num_buffer_chars++;
  }
  len_decoded = (len_decoded / 4) * 3;

  if (!bool_base64_decodetable_init) {
    for (size_t i=0; i<sizeof(base64_encoding_table)-1; i++) {
      base64_decode_table[(int)base64_encoding_table[i]] = (char) i;
    }
    bool_base64_decodetable_init = true;
  }

  size_t num_partial_bytes = num_buffer_chars > 0 ? 3 - num_buffer_chars : 0;
  len_decoded += num_partial_bytes;

  if (len_out != NULL) {
    *len_out = len_decoded;
  }

  char * ret = malloc(sizeof(char)*(len_decoded+1));

  const char * char_in = src;
  char * char_out = ret;

  for (size_t i=0; i<len_decoded/3; i++) {
    *char_out++ = base64_decode_table[(int)char_in[0]] << 2 | base64_decode_table[(int)char_in[1]] >> 4;
    *char_out++ = base64_decode_table[(int)char_in[1]] << 4 | base64_decode_table[(int)char_in[2]] >> 2;
    *char_out++ = base64_decode_table[(int)char_in[2]] << 6 | base64_decode_table[(int)char_in[3]];
    char_in += 4;
  }

  if (num_partial_bytes > 0) {
    *char_out++ = base64_decode_table[(int)char_in[0]] << 2 | base64_decode_table[(int)char_in[1]] >> 4;
  }
  if (num_partial_bytes > 1) {
    *char_out++ = base64_decode_table[(int)char_in[1]] << 4 | base64_decode_table[(int)char_in[2]] >> 2;
  }

  ret[len_decoded] = '\0';

  return ret;
}

int
render_to_png(const char * filename)
{
  UNUSED(filename);
  return -1;
}

int
file_read(const char * filename, char ** dest, size_t * size_read)
{
  FILE * fh = fopen(filename, "r");
  int error = 0;
  if (fh == NULL) {
    ERR_WRITE("Could not open %s for reading.\n", filename);
    goto error;
  }
  error = fseek(fh, 0L, SEEK_END);
  if (error) {
    ERR_WRITE("fseek SEEK_END failed for %s.\n", filename);
    goto error;
  }
  size_t size_data = ftell(fh);
  rewind(fh);
  char * return_data = malloc(sizeof(char)*(size_data+1));
  if (return_data == NULL) {
    ERR_WRITE("Could not allocate memory for reading data from %s.\n",
        filename);
    goto error;
  }
  fread(return_data, size_data, 1, fh);
  fclose(fh);
  if(size_read != NULL) {
    *size_read = size_data;
  }
  return_data[size_data] = '\0';
  *dest = return_data;
  return 0;
error:
  if (fh != NULL) {
    fclose(fh);
  }
  return -1;
}

int
file_write(const char * filename, const char * data)
{
  FILE * fh = fopen(filename, "w");
  if (fh == NULL) {
    ERR_WRITE("Could not open %s, for writing.\n", filename);
    return -1;
  }
  size_t size_data = strlen(data);
  fwrite(data, size_data, 1, fh);
  fclose(fh);
  return 0;
}

int
wrapper_nftw_remove(const char * fpath, const struct stat * sb, int typeflag,
    struct FTW * ftwbuf)
{
  UNUSED(sb); UNUSED(typeflag); UNUSED(ftwbuf);
  return remove(fpath);
}

int
rmrf(const char * path_dir)
{
  /* Do post-order traversal and don't follow symlinks. */
  int flags = FTW_DEPTH | FTW_PHYS;
  int num_open_handlers = 20;
  return nftw(path_dir, wrapper_nftw_remove, num_open_handlers, flags);
}

int
rmmkdir(const char * path_dir)
{
  struct stat info = {0};
  int error = 0;
  bool path_accessible = stat(path_dir, &info) == 0;
  bool path_is_dir = path_accessible && S_ISDIR(info.st_mode);
  if (path_is_dir) {
    error = rmrf(path_dir);
    if (error) {
      ERR_WRITE("remove failed to remove %s\n", path_dir);
      return error;
    }
  }
  error = mkdir(path_dir, MKDIR_DEFAULT_MODE);
  if (error) {
    ERR_WRITE("mkdir failed to create %s\n", path_dir);
    return error;
  }
  return 0;
}


#define FITS_OR_REALLOC(current, additional, max, buffer, increment) \
  { \
    size_t new_len = current + additional; \
    if (new_len > max) { \
      max += increment;\
      char * larger_buffer = realloc(buffer, max); \
      if (larger_buffer == NULL) { \
        ERR_WRITE("Could not reallocate %zu bytes, current path: <%s>.\n", \
            max, buffer); \
        free(buffer); \
        return NULL; \
      } \
      buffer = larger_buffer; \
    } \
  };

char *
path_concat(const char * root, ...)
{
  char * ret = malloc(sizeof(char)*SIZE_PATH_CONCAT_DEFAULT);
  if (ret == NULL) {
    ERR_WRITE(
        "Could not allocate memory for path concatenation with root: %s\n",
        root);
    return NULL;
  }

  size_t len_current = strlen(root)+strlen(PATH_CONCAT_SEPARATOR);
  size_t len_max = SIZE_PATH_CONCAT_DEFAULT;

  FITS_OR_REALLOC(len_current, strlen(root), len_max, ret,
      SIZE_PATH_CONCAT_DEFAULT);
  const char * end = ret+len_max;
  char * current_out = ret;
  len_current = snprintf(current_out, end-current_out, "%s%s", root,
      PATH_CONCAT_SEPARATOR);
  current_out = ret + len_current;

  va_list list = {0};
  va_start(list, root);
  const char * current_token = NULL;
  for(;;) {
    current_token = va_arg(list, const char *);
    if (current_token == PATH_CONCAT_SENTINEL) {
      break;
    }
    size_t len_token = strlen(current_token)+strlen(PATH_CONCAT_SEPARATOR);
    FITS_OR_REALLOC(len_current, len_token, len_max, ret,
        SIZE_PATH_CONCAT_DEFAULT);
    end = ret+len_max;
    /* Re-align current_out in case there was a realloc. */
    current_out = ret + len_current;
    /* Add to current string buffer, update len_current. */
    len_current += snprintf(current_out, end-current_out, "%s%s",
        current_token, PATH_CONCAT_SEPARATOR);
    current_out = ret + len_current - 1;
  }
  va_end(list);
  char * snug_buffer = realloc(ret, sizeof(char)*len_current);
  if (snug_buffer == NULL) {
    ERR_WRITE("Failed to tighten the buffer from: %zu to %zu.\n",
        len_max, len_current);
    free(ret);
    return NULL;
  }
  ret = snug_buffer;
  ret[len_current-1] = '\0';
  return ret;
}
