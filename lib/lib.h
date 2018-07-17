#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define SIZE_BUFF_ERROR 512
extern char BUFF_ERROR[SIZE_BUFF_ERROR];

#define mu_assert(test, message, ...) do { if (!(test)) { \
  snprintf(BUFF_ERROR, SIZE_BUFF_ERROR, message, __VA_ARGS__); \
  return false; }} while (0)
#define mu_run_test(test) do { bool success = test(); tests_run++; \
  if (!success) { return success; }} while (0)

#define ERR_PRINT(void) printf("%s\n", BUFF_ERROR);
