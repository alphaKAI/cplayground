#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

TEST_CASE(test_parseDouble_1, {
  sds str = sdsnew("123");

  assert(parseDouble(str) == 123);
})

TEST_CASE(test_parseDouble_2, {
  sds str = sdsnew("123.456");
  double v = parseDouble(str);

  printf("v: %f\n", v);

  assert(v == 123.456);
})

TEST_CASE(test_read_text, {
  sds file_name = sdsnew("util.c");
  sds text = readText(file_name);

  int fd = open(file_name, O_RDONLY);
  struct stat fs;
  if (fstat(fd, &fs) < 0) {
    fprintf(stderr, "Failed to stat - %s", file_name);
    exit(EXIT_FAILURE);
  }
  size_t size = (size_t)fs.st_size;
  close(fd);

  size_t text_len = sdslen(text);

  assert(text_len == size);
})

void util_test(void) {
  test_parseDouble_1();
  test_parseDouble_2();
  test_read_text();

  printf("[util_test] All of tests are passed\n");
}
