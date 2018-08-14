#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "png.h"

#define SIZE_BUFF_ERROR 512
extern char BUFF_ERROR[SIZE_BUFF_ERROR];
#define SIZE_DATA_MESHES 128
extern GLfloat * DATA_MESHES[SIZE_DATA_MESHES];
extern size_t SIZE_MESHES[SIZE_DATA_MESHES];
#define SIZE_GL_BUFFERS 30
extern GLuint GL_BUFFERS[SIZE_GL_BUFFERS];
#define SIZE_GL_VERTEX_ATTRIBS 30
extern GLuint GL_VERTEX_ATTRIBS[SIZE_GL_VERTEX_ATTRIBS];
#define SIZE_STRING_RING_BUFFER 512
extern char STRING_RING_BUFFER[SIZE_STRING_RING_BUFFER];

#define ERR_PRINT(void) printf("[!][%s:%d:%s]: %s", \
    __FILE__,__LINE__,__func__,BUFF_ERROR); \
    BUFF_ERROR[0] = '\0'
#define ERR_WRITE(fmt, ...) \
  snprintf(BUFF_ERROR, SIZE_BUFF_ERROR, fmt, __VA_ARGS__);
#define OpenGLInfo(void) \
  printf("OpenGL: %s, GLSL: %s\n", glGetString(GL_VERSION), \
      glGetString(GL_SHADING_LANGUAGE_VERSION));
#define STATUS(fmt, ...) printf("[.]: " fmt, __VA_ARGS__);

#define LEN(x) sizeof(x)/sizeof(x[0])
#define UNUSED(x) (void)(x)

#define SIZE_PATH_CONCAT_DEFAULT 128
#define PATH_CONCAT_SEPARATOR "/"
#define PATH_CONCAT_SENTINEL NULL

#define PATH_CONCAT(root, ...) path_concat(root, __VA_ARGS__, \
    PATH_CONCAT_SENTINEL)
#define MKDIR_DEFAULT_MODE 0755

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
  size_t flags;
};

struct v3 {
  union {
    GLfloat f[3];
    struct {
      GLfloat x;
      GLfloat y;
      GLfloat z;
    };
  };
};

struct v4 {
  union {
    GLfloat f[4];
    struct {
      GLfloat x;
      GLfloat y;
      GLfloat z;
      GLfloat w;
    };
  };
};

struct m4 {
  union {
    GLfloat f[4][4];
    struct {
      struct v4 x;
      struct v4 y;
      struct v4 z;
      struct v4 w;
    };
  };
};

extern struct m4 m4_eye;

#define SIZE_M4_TRANSFORMATION 3
extern struct m4 M4_TRANSFORMATION[SIZE_M4_TRANSFORMATION];

#define SIZE_CLICK_BUFFER 256
extern struct v3 CLICK_BUFFER[SIZE_CLICK_BUFFER];

struct obj_render {
  size_t id_vao;
  size_t id_program;
  size_t id_transformation;
};

bool
init_lib(GLFWwindow ** window);

void
main_run(struct main_run_data * data);

void
main_wait(struct main_run_data * data);

void
polygon(GLfloat * data, struct v3 * point_list, size_t num_points);

void
polygon_from_clicks(GLfloat * data, size_t num_points);

enum RENDER_SETTING_FLAGS {
  RENDER_DISABLE_RENDERING,
  NUM_FLAGS,
};

bool
render_get(struct main_run_data * data, size_t flags);

void
render_set(struct main_run_data * data, size_t flags);

extern const char * source_shader_default_vert;
extern const char * source_shader_default_frag;

GLfloat *
mesh_set_data(size_t index, GLfloat * value);

void *
mesh_set_data_copy(size_t index, GLfloat * value, size_t size);

void
mesh_set_size(size_t index, size_t size);

void *
mesh_data_allocate(size_t index, size_t size);

void
mesh_data_deallocate(size_t index);

size_t
mesh_size(size_t index);

GLfloat *
mesh_data(size_t index);

void
obj_translate(size_t id_transformation, struct v3 * delta);

struct v3
obj_pos(size_t id_transformation);

const char *
v3_str(struct v3 * v3);

bool
v3_eq(struct v3 * v1, struct v3 * v2);

void
obj_transfomation_reset(size_t id_transformation);

struct m4 *
transformation_get(size_t id_transformation);

struct v3 *
click_save(GLfloat x, GLfloat y);

char *
base64_encode(const char * src, size_t len_in, size_t * len_out);

char *
base64_decode(const char * src, size_t len_in, size_t * len_out);

int
render_to_png(const char * filname);

int
file_read(const char * filename, char ** dest, size_t * size_read);

int
file_write(const char * filename, const char * data);

int
rmmkdir(const char * path_dir);

char *
path_concat(const char * root, ...);
