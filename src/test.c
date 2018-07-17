#include "lib.h"

int tests_run = 0;

int foo = 7;
int bar = 4;

bool
test_init_lib()
{
  return init_lib(NULL);
}

bool
test_foo_bar()
{
  mu_assert(foo==bar, "foo %d, was not equal to bar %d!\n", foo, bar);
  return true;
}

bool
all_tests() {
  mu_run_test(test_foo_bar);
  return true;
}

int
main(void) {
  bool success = all_tests();
  if (success) {
    printf("NO ERRORS!\n");
    return EXIT_SUCCESS;
  } else {
    printf("ERRORS!\n");
    ERR_PRINT();
    return EXIT_FAILURE;
  }
}
