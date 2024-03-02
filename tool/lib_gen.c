#ifndef _LIB_GEN_H_
#define _LIB_GEN_H_
#include <commonlib.h>
#include <assert.h>

void prepare_header_guard_begin(const char* output_filename, const char* guard_name){
  FILE* out = fopen(output_filename, "w");
  assert(out != NULL);

  fprintf(out, "#ifndef _%s_\n", guard_name);
  fprintf(out, "#define _%s_\n", guard_name);

  fclose(out);
  log_f(LOG_INFO, "Prepared output header!");
}

void prepare_header_guard_end(const char* output_filename, const char* guard_name){
  FILE* out = fopen(output_filename, "a");
  assert(out != NULL);

  fprintf(out, "\n#endif //_%s_\n", guard_name);

  fclose(out);
  log_f(LOG_INFO, "Prepared output footer!");
}

#endif /* _LIB_GEN_H_ */
