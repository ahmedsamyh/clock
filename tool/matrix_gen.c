#include "lib_gen.c"
#include <stdio.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

static const char* output_filename = "matrix.h";

int main(void){
  prepare_header_guard_begin(output_filename, "MATRIX_H");
  prepare_header_guard_end(output_filename, "MATRIX_H");
  return 0;
}
