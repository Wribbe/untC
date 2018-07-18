#include "lib.h"
#include "expose_for_test.h"

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
  size_t mesh_index = 0;
  GLfloat data[num_floats];
  for (size_t i=0; i<num_floats; i++) {
    data[i] = i;
  }
  GLfloat * allocated_data = mesh_data_allocate(mesh_index, size);
  mu_assert(allocated_data != NULL, "%s\n", "allocated_data was NULL.");
  mu_assert(mesh_size(mesh_index) == size, "%s\n",
      "Size of allocated data does not match stored size.");
  GLfloat * index_pointer = mesh_set_data_copy(mesh_index, data, size);
  for (size_t i=0; i<num_floats; i++) {
    mu_assert(index_pointer[i] == data[i],
        "Data at index_pointer[%zu] (%f), did not match data[%zu] (%f)\n",
        i, index_pointer[i], i, data[i]);
  }
  /* Deallocate data. */
  mesh_data_deallocate(mesh_index);
  /* Destroy references to allocated memory. */
  index_pointer = NULL;
  mesh_set_data(mesh_index, NULL);
  return true;
}

bool
test_obj_translate()
{
  GLfloat delta = 0.5f;
  size_t id_transformation = 0;
  struct v3 v3_delta = {{{delta, delta, delta}}};
  obj_translate(id_transformation, &v3_delta);
  struct v3 pos_moved = obj_pos(id_transformation);
  mu_assert(v3_eq(&v3_delta, &pos_moved), "v3 %s was not equal to v3 %s.",
      v3_str(&v3_delta), v3_str(&pos_moved));
  obj_transfomation_reset(id_transformation);
  struct v3 pos_zero = {{{0,0,0}}};
  struct v3 pos_current = obj_pos(id_transformation);
  mu_assert(v3_eq(&pos_current, &pos_zero), "v3 %s was not reset.",
      v3_str(&pos_current));
  return true;
}

bool
test_v3_str_500()
{
  size_t num_prints = 500;
  struct v3 v = {{{1,2,3}}};
  for (size_t i=0; i<num_prints; i++) {
    v3_str(&v);
  }
  return true;
}

bool
test_polygon_from_clicks()
{
  GLfloat correct[] = {
     0.0f, 0.5f, 0.0f,
    -0.5f,-0.5f, 0.0f,
     0.5f,-0.5f, 0.0f,
  };

  click_save(correct[0], correct[1]);
  click_save(correct[1*3+0], correct[1*3+1]);
  click_save(correct[2*3+0], correct[2*3+1]);

  size_t num_points = 3;
  size_t num_elements = num_points*3;
  GLfloat data[num_elements];
  memset(data, 0, sizeof(GLfloat)*num_elements);
  polygon_from_clicks(data, num_points);
  for (size_t i=0; i<num_elements; i++) {
    mu_assert(data[i] == correct[i],
        "data[%zu] not equal correct[%zu]; %f != %f.\n",
        i, i, data[i], correct[i]);
  }
  return true;
}

bool
test_click_save_overrun()
{
  for(size_t i=0; i<SIZE_CLICK_BUFFER+100e3; i++) {
    click_save(0.0f, 0.0f);
  }
  return true;
}

bool
test_click_rewind_overrun()
{
  struct v3 * current = click_buffer_start;
  current = click_rewind(current, 1);
  mu_assert(current == click_buffer_last,
      "Rewound click should be at last position (%p), but was on (%p).",
      (void*) click_buffer_last, (void*)current);
  struct v3 * correct = click_buffer_start + 3;
  current = correct;
  current = click_rewind(current, 3*SIZE_CLICK_BUFFER);
  mu_assert(current == correct,
      "Rewound click did not loop around correctly, is %p, should be %p.",
      (void*)current, (void *)correct);
  return true;
}

bool
all_tests() {
  mu_run_test(test_init_lib);
  mu_run_test(test_run_main_for_5);
  mu_run_test(test_polygon);
  mu_run_test(test_allocate_mesh_data);
  mu_run_test(test_v3_str_500);
  mu_run_test(test_obj_translate);
  mu_run_test(test_click_save_overrun);
  mu_run_test(test_polygon_from_clicks);
  mu_run_test(test_click_rewind_overrun);
  return true;
}

int
main(void) {
  bool success = all_tests();
  if (!success) {
    ERR_PRINT();
  }
  printf("Ran %d tests", tests_run);
  if (!success) {
    printf(" with Errors! ^\n");
    return EXIT_FAILURE;
  } else {
    printf(", ALL PASSED.\n");
    return EXIT_SUCCESS;
  }
}
