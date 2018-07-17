#include "lib.h"

int tests_run = 0;

bool
test_init_lib()
{
  return init_lib(NULL);
}

bool
test_run_main_for_5()
{
  struct main_run_data data = {0};
  size_t max_frames = 5;
  render_set(&data, RENDER_DISABLE_RENDERING);
  data.max_frames = max_frames;
  main_run(&data);
  main_wait(&data);
  mu_assert(data.total_frames == max_frames,
      "Number of run frames, %zu, was not equal to requested %zu!\n",
      max_frames, data.total_frames);
  return true;
}

bool
test_polygon()
{
  GLfloat correct[] = {
    0.0f,  0.5f, 0.0f,
   -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
  };
  GLfloat data[9];
  struct v3 ps[] = {
    {{{0.0, 0.5, 0.0}}},
    {{{-0.5, -0.5, 0.0}}},
    {{{0.5, -0.5, 0.0}}},
  };
  polygon(data, ps, 3);
  for (size_t i=0; i<9; i++) {
    mu_assert(data[i] == correct[i], "%f was not equal %f for index: %zu.\n",
        data[i], correct[i], i);
  }
  return true;
}

bool
test_allocate_mesh_data()
{
  size_t num_floats = 300;
  size_t size = sizeof(GLfloat) * num_floats;
  GLfloat data[num_floats];
  return true;
}

bool
all_tests() {
  mu_run_test(test_init_lib);
  mu_run_test(test_run_main_for_5);
  mu_run_test(test_polygon);
  mu_run_test(test_allocate_mesh_data);
  return true;
}

int
main(void) {
  bool success = all_tests();
  printf("Ran %d tests ", tests_run);
  if (!success) {
    printf("with Errors!:\n");
    ERR_PRINT();
    return EXIT_FAILURE;
  } else {
    printf("with no errors, all passed.\n");
    return EXIT_SUCCESS;
  }
}
