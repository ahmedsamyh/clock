#include "lib_gen.c"
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>
#include <stdio.h>
#define STRING_VIEW_IMPLEMENTATION
#include <string_view.h>
#include <assert.h>

static const char* header_filename = "clock_vector.h";
static const char* source_filename = "clock_vector.c";
#define GUARD_NAME "CLOCK_VECTOR_H"

void declare_arithmetic_function(FILE* out, const char* return_type, const char* prefix, const char* func, const char* full_name){
  fprintf(out, "%s %s_%s(%s v1, %s v2);\n", return_type, prefix, func, full_name, full_name);
}

void declare_scalar_arithmetic_function(FILE* out, const char* return_type, const char* prefix, const char* func, const char* full_name, String_view type){
  fprintf(out, "%s %s_%ss(%s v, "SV_FMT" num);\n", return_type, prefix, func, full_name, SV_ARG(type));
}

void declare_getter_function(FILE* out, const char* return_type, const char* prefix, const char* func, const char* full_name){
  fprintf(out, "%s %s_%s(%s v);\n", return_type, prefix, func, full_name);
}

void define_arithmetic_function(FILE* out, const char* return_type, const char* prefix, const char* func, const char* full_name, String_view members, char op){
  fprintf(out, "%s %s_%s(%s v1, %s v2){\n", return_type, prefix, func, full_name, full_name);
  fprintf(out, "  return (%s){\n", full_name);


  while (members.count > 0){
    String_view member = sv_lpop_until_char(&members, ' ');
    sv_trim(&members);
    fprintf(out, "    ."SV_FMT" = ", SV_ARG(member));

    fprintf(out, "v1."SV_FMT" %c v2."SV_FMT, SV_ARG(member), op, SV_ARG(member));
    fprintf(out, ",\n");
  }

  fprintf(out, "  };\n");
  fprintf(out, "}\n");
}

void define_scalar_arithmetic_function(FILE* out, const char* return_type, const char* prefix, const char* func, const char* full_name, String_view type, String_view members, char op){
  fprintf(out, "%s %s_%ss(%s v1, "SV_FMT" num){\n", return_type, prefix, func, full_name, SV_ARG(type));
  fprintf(out, "  return (%s){\n", full_name);


  while (members.count > 0){
    String_view member = sv_lpop_until_char(&members, ' ');
    sv_trim(&members);
    fprintf(out, "    ."SV_FMT" = ", SV_ARG(member));

    fprintf(out, "v1."SV_FMT" %c num", SV_ARG(member), op);
    fprintf(out, ",\n");
  }

  fprintf(out, "  };\n");
  fprintf(out, "}\n");
}

// define he function body for a getter function.
// there sould be only one parameter
//
// format: {m}     -> member
//         |x|     -> x can be anything that will be put between each member format (will be not put after the last member)
//
// the format will be repeated for each member
//
// eg: "return sqrt({m}*{m} |+|);"
//
//     Vector2f {x, y}  -> "return sqrt({x}*{x} + {y}*{y});"
//


void define_getter_function_base(FILE* out, const char* param_name, String_view members, String_view format){
  assert(0 && "Not implemented!");
  int cursor = 0;
  bool done = false;
  String_view og_members = members;
  size_t members_count = 0;

  // count members
  while (members.count > 0){
    String_view member = sv_lpop_until_char(&members, ' ');
    sv_trim(&members);
    members_count++;
  }

  log_f(LOG_INFO, "members_count: %u", members_count);

  members = og_members;

  int members_outputted = 0;
  bool inside_between_member = false;

  bool output_member = false;
  while (!done){
    char c = format.data[cursor];
    switch (c){
    case '{': {
      output_member = true;
      cursor++;
    } break;
    case '|': {
      c = format.data[cursor];
      while (c != '|'){
	if (c == '\0'){
	  log_f(LOG_ERROR, "Unclosed `|` in format!\n");
	  exit(1);
	}
	if (members_outputted < members_count){
	  fputc(c, out);
	}
	cursor++;
      }
      cursor++;
    } break;
    default: {
      if (output_member){
	String_view member = sv_lpop_until_char(&members, ' ');
	sv_trim(&member);
	fprintf(out, SV_FMT, SV_ARG(member));
	cursor += member.count+1;
	/* printf("char after member '%c'\n", format.data[cursor]); */
	output_member = false;
	members_outputted++;
	if (members_outputted < members_count){
	  cursor = 0;
	}
      } else {
	fputc(c, out);
	cursor++;
      }
    }
    }
    /* printf("cursor: %d | '%c'\n", cursor, format.data[cursor]); */
    if (cursor >= format.count-1){
      done = true;
    }
  }
}

void define_mag_function(FILE* out, const char* return_type, const char* prefix, const char* func, const char* full_name, String_view type, String_view members, bool squared){
  fprintf(out, "%s %s_%s(%s v){\n", return_type, prefix, func, full_name);
  fprintf(out, "  return ");
  if (!squared){
    fprintf(out, "sqrtf");
  }
  fprintf(out, "(");
  size_t c = 0;
  while (members.count > 0){
    String_view member = sv_lpop_until_char(&members, ' ');
    sv_trim(&members);
    fprintf(out, "(v."SV_FMT"*v."SV_FMT")", SV_ARG(member), SV_ARG(member));

    if (members.count != 0){ // therefore this is not the last member
      fprintf(out, " + ");
    }
  }
  fprintf(out, ");\n");
  fprintf(out, "}\n");
}

void declare_vector(String_view format){
  String_view name = sv_lpop_until_char(&format, ':');
  sv_lremove(&format, 1); // remove :
  sv_trim(&format);
  sv_trim(&name);
  String_view type = sv_lpop_until_char(&format, ':');
  sv_lremove(&format, 1); // remove :
  sv_trim(&format);
  sv_trim(&type);
  String_view members = sv_lpop_until_char(&format, ':');
  String_view members_copy = members;
  sv_lremove(&format, 1); // remove :
  sv_trim(&format);
  sv_trim(&members);

  /* printf("name:    '"SV_FMT"'\n", SV_ARG(name)); */
  /* printf("type:    '"SV_FMT"'\n", SV_ARG(type)); */
  /* printf("members: '"SV_FMT"'\n", SV_ARG(members)); */

  FILE* out = fopen(header_filename, "a");
  assert(out != NULL);

  fprintf(out, "\n// "SV_FMT"%c --------------------------------------------------\n", SV_ARG(name), *type.data);
  // struct
  fprintf(out, "typedef struct {\n");
  while (members_copy.count > 0){
    String_view member = sv_lpop_until_char(&members_copy, ' ');
    sv_trim(&members_copy);
    sv_lremove(&member, 1);
    sv_rremove(&member, 1);

    if (sv_contains_char(member, '|')){
      fprintf(out, "  union {\n");

      while (member.count > 0){
	String_view member_name = sv_lpop_until_char(&member, '|');
	sv_lremove(&member, 1);
	sv_trim(&member);
	fprintf(out, "    "SV_FMT" "SV_FMT";\n", SV_ARG(type), SV_ARG(member_name));
      }

      fprintf(out, "  };\n");
    } else {
      fprintf(out, "  "SV_FMT" "SV_FMT";\n", SV_ARG(type), SV_ARG(member));
    }
  }
  fprintf(out, "} "SV_FMT, SV_ARG(name));
  // suffix (suffix with the first character of the type for now)
  fprintf(out, "%c;\n", *type.data);

  char prefix[4] = {0};
  char c = tolower(*name.data);
  snprintf(prefix, 4, "%c%c%c", c, *(name.data+name.count-1), *type.data);

  char* full_name = (char*)malloc(sizeof(char)*name.count+2);
  snprintf(full_name, name.count+2, SV_FMT"%c", SV_ARG(name), *type.data);


  fprintf(out, "\n");

  // function declarations
  declare_arithmetic_function(out, full_name, prefix, "add", full_name);
  declare_arithmetic_function(out, full_name, prefix, "sub", full_name);
  declare_arithmetic_function(out, full_name, prefix, "mul", full_name);
  declare_arithmetic_function(out, full_name, prefix, "div", full_name);

  declare_scalar_arithmetic_function(out, full_name, prefix, "add", full_name, type);
  declare_scalar_arithmetic_function(out, full_name, prefix, "sub", full_name, type);
  declare_scalar_arithmetic_function(out, full_name, prefix, "mul", full_name, type);
  declare_scalar_arithmetic_function(out, full_name, prefix, "div", full_name, type);

  declare_getter_function(out, "float", prefix, "mag", full_name);
  declare_getter_function(out, "float", prefix, "mag2", full_name);
  declare_getter_function(out, "float", prefix, "dist", full_name);
  declare_getter_function(out, "float", prefix, "dist2", full_name);

  free(full_name);

  fclose(out);
}

void define_vector(String_view format){
  String_view name = sv_lpop_until_char(&format, ':');
  sv_lremove(&format, 1); // remove :
  sv_trim(&format);
  sv_trim(&name);
  String_view type = sv_lpop_until_char(&format, ':');
  sv_lremove(&format, 1); // remove :
  sv_trim(&format);
  sv_trim(&type);
  String_view members = sv_lpop_until_char(&format, ':');
  String_view members_copy = members;
  sv_lremove(&format, 1); // remove :
  sv_trim(&format);
  sv_trim(&members);

  char* full_name = (char*)malloc(sizeof(char)*name.count+2);
  snprintf(full_name, name.count+2, SV_FMT"%c", SV_ARG(name), *type.data);

  FILE* out = fopen(source_filename, "a");
  assert(out != NULL);

  fprintf(out, "\n// "SV_FMT"%c --------------------------------------------------\n", SV_ARG(name), *type.data);
  fprintf(out, "\n");

  char prefix[4] = {0};
  char c = tolower(*name.data);
  snprintf(prefix, 4, "%c%c%c", c, *(name.data+name.count-1), *type.data);

  // function definitions
  define_arithmetic_function(out, full_name, prefix, "add", full_name, members, '+');
  define_arithmetic_function(out, full_name, prefix, "sub", full_name, members, '-');
  define_arithmetic_function(out, full_name, prefix, "mul", full_name, members, '*');
  define_arithmetic_function(out, full_name, prefix, "div", full_name, members, '/');

  define_scalar_arithmetic_function(out, full_name, prefix, "add", full_name, type, members, '+');
  define_scalar_arithmetic_function(out, full_name, prefix, "sub", full_name, type, members, '-');
  define_scalar_arithmetic_function(out, full_name, prefix, "mul", full_name, type, members, '*');
  define_scalar_arithmetic_function(out, full_name, prefix, "div", full_name, type, members, '/');

  define_mag_function(out, "float", prefix, "mag", full_name, type, members, false);
  define_mag_function(out, "float", prefix, "mag2", full_name, type, members, true);
  define_mag_function(out, "float", prefix, "dist", full_name, type, members, false);
  define_mag_function(out, "float", prefix, "dist2", full_name, type, members, true);

  free(full_name);

  fclose(out);
}

int main(void){
  prepare_header_begin(header_filename, GUARD_NAME);
  const char* types[] = {
    "float",
    "double",
    "int",
  };
#define buff_size 1024
  char buff[buff_size] = {0};
  for (size_t i = 0; i < ARRAY_LEN(types); ++i){
    snprintf(buff, buff_size, "Vector2 : %s : {x|u} {y|v}", types[i]); declare_vector(SV(buff));
    snprintf(buff, buff_size, "Vector3 : %s : {x} {y} {z}", types[i]); declare_vector(SV(buff));
    snprintf(buff, buff_size, "Vector4 : %s : {x|r} {y|g} {z|b} {w|a}", types[i]); declare_vector(SV(buff));
  }
  prepare_header_end(header_filename, GUARD_NAME);
  prepare_source_header(source_filename, header_filename);
  for (size_t i = 0; i < ARRAY_LEN(types); ++i){
    snprintf(buff, buff_size, "Vector2 : %s : x y", types[i]); define_vector(SV(buff));
    snprintf(buff, buff_size, "Vector3 : %s : x y z", types[i]); define_vector(SV(buff));
    snprintf(buff, buff_size, "Vector4 : %s : x y z w", types[i]); define_vector(SV(buff));
  }

  log_f(LOG_INFO, "Output: -> %s", header_filename);
  log_f(LOG_INFO, "Output: -> %s", source_filename);

  return 0;
}
