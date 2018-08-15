#include "../lib/lib.h"
#include "../lib/expose_for_test.h"

int tests_run = 0;
struct free_queue_unit free_queue[SIZE_FREE_QUEUE];
struct free_queue_unit * free_queue_first = free_queue;
struct free_queue_unit * free_queue_last = free_queue+SIZE_FREE_QUEUE-1;
struct free_queue_unit * free_queue_current = NULL;

void
free_queue_deallocate(struct free_queue_unit * unit)
{
  if (unit->data != NULL) {
    unit->dealloc_function(free_queue_current->data);
    unit->data = NULL;
  }
}

void
free_queue_add(void * data, dealloc_function dealloc_function)
{
  if (free_queue_current == NULL) {
    free_queue_current = free_queue_first;
  } else if (free_queue_current+1 > free_queue_last) {
    free_queue_current = free_queue_first;
  }
  if (free_queue_current->data != NULL) {
    free_queue_deallocate(free_queue_current);
  }
  free_queue_current->data = data;
  free_queue_current->dealloc_function = dealloc_function;
  free_queue_current++;
}

void
free_queue_process(void)
{
  if (free_queue_current == NULL) {
    return;
  }
  free_queue_current = free_queue_first;
  while(free_queue_current <= free_queue_last) {
    free_queue_deallocate(free_queue_current);
    free_queue_current++;
  }
  free_queue_current = NULL;
}

void
free_queue_pop(void)
{
  if (free_queue_current == NULL) {
    return;
  }
  free_queue_deallocate(free_queue_current);
  free_queue_current--;
}

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
  for(size_t i=0; i<SIZE_CLICK_BUFFER+10; i++) {
    click_save(0.0f, 0.0f);
    mu_assert(click_buffer_current <= click_buffer_last,
        "click_buffer_current (%p) is beyond click_buffer_last (%p)\n",
        (void*)click_buffer_current, (void*)click_buffer_last);
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
test_base64_encode()
{
  size_t len_out = 0;
  for (size_t i=0; i<LEN(examples_base64_wiki); i++) {
    const char * text = examples_base64_wiki[i][0];
    const char * correct = examples_base64_wiki[i][1];
    char * encoded = base64_encode(text, strlen(text), &len_out);
    free_queue_add(encoded, free);
    mu_assert(len_out == strlen(correct),
      "Encoding has wrong length: %zu, should be %zu.\n",
      len_out, strlen(correct));
    mu_assert(strcmp(encoded, correct) == 0,
      "Encoded string: \n\n  %s\n\nDid not match correct encoding:\n\n  %s\n",
      encoded, correct);
    free_queue_pop();
  }
  return true;
}

bool
test_base64_decode()
{
  size_t len_out = 0;
  for (size_t i=0; i<LEN(examples_base64_wiki); i++) {
    const char * correct = examples_base64_wiki[i][0];
    const char * encoded = examples_base64_wiki[i][1];
    char * decoded = base64_decode(encoded, strlen(encoded), &len_out);
    free_queue_add(decoded, free);
    mu_assert(len_out == strlen(correct),
      "Text (correct: %s) has wrong length: %zu, should be %zu.\n",
      correct, len_out, strlen(correct));
    mu_assert(strcmp(decoded, correct) == 0,
      "Decoded string: \n\n  %s\n\nDid not match correct text:\n\n  %s\n",
      decoded, correct);
    free_queue_pop();
  }
  return true;
}

bool
test_render_to_png()
{
  int error_render = render_to_png(test_filename_png);
  mu_assert(error_render == 0,
      "render_to_png(%s) returned with error code: %d\n", test_filename_png,
      error_render);
  int error_file_read = file_read(test_filename_png, NULL, NULL);
  mu_assert(error_file_read == 0,
      "file_read(%s) returned error code: %d\n", test_filename_png,
      error_file_read);
  return true;
}

bool
test_rmmkdir()
{
  int error_rmmkdir = rmmkdir(test_dir_output);
  mu_assert(error_rmmkdir == 0,
      "rmmkdir(%s) returned error-code: %d\n", test_dir_output, error_rmmkdir);
  struct stat info = {0};
  mu_assert(stat(test_dir_output, &info) == 0, "Could not access path %s.",
      test_dir_output);
  mu_assert(S_ISDIR(info.st_mode), "%s is not a directory.\n",
      test_dir_output);
  return true;
}

bool
test_data_compare()
{
  mu_assert(false, "%s\n", "Seeded fault test_data_compare.");
  return true;
}

bool
test_path_concat()
{
  const char * correct = "this" PATH_CONCAT_SEPARATOR "is" \
    PATH_CONCAT_SEPARATOR "correct";
  char * path = PATH_CONCAT("this","is","correct");
  mu_assert(path != NULL, "Returned path was NULL and not: <%s>.", correct);
  free_queue_add(path, free);
  mu_assert(strcmp(path, correct) == 0,
      "<%s> was not equal to expected path: <%s>.\n",
      path, correct);
  free_queue_process();
  return true;
}


bool
test_file_read_and_write()
{
  char * path_file = PATH_CONCAT(test_dir_output, test_file_write_filename);
  free_queue_add(path_file, free);
  int error = file_write(path_file, test_file_write_string);
  mu_assert(error == 0, "file_write(%s, %s) returned error: %d\n", path_file,
      test_file_write_string, error);
  char * text_read = NULL;
  size_t size_read = 0;
  error = file_read(path_file, &text_read, &size_read);
  if (text_read != NULL) {
    free_queue_add(text_read, free);
  }
  mu_assert(error == 0, "file_read(%s): returned error: %d\n", path_file,
      error);
  mu_assert(size_read == strlen(test_file_write_string),
      "Wrong data size returned by file_read(%s), expected %zu, got %zu.\n",
      path_file, size_read, strlen(test_file_write_string));
  mu_assert(strcmp(text_read, test_file_write_string) == 0,
      "Read string <%s> was not expected string <%s>.\n", text_read,
      test_file_write_string);
  free_queue_process();
  return true;
}

bool
test_png_read()
{
  const char * filename = test_data_png_transparency;
  int error = file_read_png(filename);
  mu_assert(error == 0, "file_read_png(%s) returned error: %d\n",
      filename, error);
  return true;
}

bool
test_init() {
  mu_run_test(test_rmmkdir);
  rmmkdir(test_dir_output);
  mu_run_test(test_path_concat);
  return true;
}

bool
test_cases() {
  mu_run_test(test_init_lib);
  mu_run_test(test_run_main_for_5);
  mu_run_test(test_polygon);
  mu_run_test(test_allocate_mesh_data);
  mu_run_test(test_v3_str_500);
  mu_run_test(test_obj_translate);
  mu_run_test(test_click_save_overrun);
  mu_run_test(test_polygon_from_clicks);
  mu_run_test(test_click_rewind_overrun);
  mu_run_test(test_base64_encode);
  mu_run_test(test_base64_decode);
  mu_run_test(test_file_read_and_write);
  mu_run_test(test_png_read);
  mu_run_test(test_render_to_png);
  mu_run_test(test_data_compare);
  return true;
}

int
main(void) {
  bool success = test_init() && test_cases();
  if (!success) {
    ERR_PRINT();
    printf("\n");
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
