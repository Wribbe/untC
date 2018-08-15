struct v3 *
click_rewind(struct v3 * pointer, size_t num_points);

extern struct v3 * click_buffer_current;
extern struct v3 * click_buffer_last;
extern struct v3 * click_buffer_start;

const char * examples_base64_wiki[][2] = {
  {"any carnal pleasure.","YW55IGNhcm5hbCBwbGVhc3VyZS4="},
  {"any carnal pleasure","YW55IGNhcm5hbCBwbGVhc3VyZQ=="},
  {"any carnal pleasur","YW55IGNhcm5hbCBwbGVhc3Vy"},
  {"any carnal pleasu","YW55IGNhcm5hbCBwbGVhc3U="},
  {"any carnal pleas","YW55IGNhcm5hbCBwbGVhcw=="},
};

const char base64_test_text_long[] = "This is a text that should be used for testing\n"
"encoding and decoding of base64 functionality.\n";
const char base64_test_encoded_long[] = "IlRoaXMgaXMgYSB0ZXh0IHRoYXQgc2hvdWxkIGJlIHVzZWQgZm9yIHRlc3RpbmdcbiIKImVuY29kaW5nIGFuZCBkZWNvZGluZyBvZiBiYXNlNjQgZnVuY3Rpb25hbGl0eS5cbiI=";

typedef void (*dealloc_function)(void *);

struct free_queue_unit {
  void * data;
  dealloc_function dealloc_function;
};

void
free_queue_add(void * data, dealloc_function dealloc_function);

void
free_queue_process(void);

extern struct free_queue_unit free_queue[];
#define SIZE_FREE_QUEUE 256

#define mu_assert(test, message, ...) do { if (!(test)) { \
  snprintf(BUFF_ERROR, SIZE_BUFF_ERROR, "%s:%d:%s(): " \
      message,__FILE__,__LINE__,__func__,__VA_ARGS__); \
  free_queue_process(); \
  return false; }} while (0)
#define mu_run_test(test) do { bool success = test(); tests_run++; \
  if (!success) { return success; }} while (0)

const char * test_dir_output = "test_output";
const char * test_filename_png = "test.png";
const char * test_file_write_string = "THIS IS THE TEXT THAT\nSHOULD BE IN THE FILE!";
const char * test_file_write_filename = "test_file_write.txt";

const char * test_data_png_transparency =  "data/PNG_transparency_demonstration_1.png";
