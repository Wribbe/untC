struct v3 *
click_rewind(struct v3 * pointer, size_t num_points);

extern struct v3 * click_buffer_current;
extern struct v3 * click_buffer_last;
extern struct v3 * click_buffer_start;

const char base64_test_src[] = "This is a text that should be used for testing\n"
"encoding and decoding of base64 functionality.\n";

const char base64_test_correct[] = "IlRoaXMgaXMgYSB0ZXh0IHRoYXQgc2hvdWxkIGJlIHVzZWQgZm9yIHRlc3RpbmdcbiIKImVuY29kaW5nIGFuZCBkZWNvZGluZyBvZiBiYXNlNjQgZnVuY3Rpb25hbGl0eS5cbiI=";
