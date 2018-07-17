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
  data.max_frames = max_frames;
  main_run(&data);
  main_wait(&data);
  mu_assert(data.total_frames == max_frames,
      "Number of run frames, %zu, was not equal to requested %zu!\n",
      max_frames, data.total_frames);
  return true;
}

bool
all_tests() {
  mu_run_test(test_init_lib);
  mu_run_test(test_run_main_for_5);
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
