#ifndef _LIB_GEN_H_
#define _LIB_GEN_H_
#include <commonlib.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void prepare_header_begin(const char* header_filename, const char* guard_name){
  FILE* out = fopen(header_filename, "w");
  assert(out != NULL);

  fprintf(out, "#ifndef _%s_\n", guard_name);
  fprintf(out, "#define _%s_\n", guard_name);

  fclose(out);
  log_f(LOG_INFO, "Prepared output header!");
}

void prepare_header_end(const char* header_filename, const char* guard_name){
  FILE* out = fopen(header_filename, "a");
  assert(out != NULL);

  fprintf(out, "\n#endif //_%s_\n", guard_name);

  fclose(out);
  log_f(LOG_INFO, "Prepared output footer!");
}

void prepare_source_header(const char* source_filename, const char* header_filename){
  FILE* out = fopen(source_filename, "w");
  assert(out != NULL);

  fprintf(out, "#include <%s>\n", header_filename);

  fclose(out);
  log_f(LOG_INFO, "Prepared source header!");
}

#endif /* _LIB_GEN_H_ */
