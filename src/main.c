#include <math.h>
#include <clock.h>
#define VECTOR_IMPLEMENTATION
#include <clock_vector.h>

int main(void){
  if (init_window(1280, 720, "GLFW Window") < 0) return 1;

  // triangle {x,y,z,w}
  Vectorf vertices[]  = {
    {-1.f, -1.f, 0.f, 1.f}, // bottom-left
    { 1.f, -1.f, 0.f, 1.f}, // top-middle
    { 0.f,  1.f, 0.f, 1.f} // bottom-right
  };
  const size_t vertex_count = ARRAY_LEN(vertices);

  GLuint vao[1] = {0};

  glGenVertexArrays(1, vao);
  glBindVertexArray(vao[0]);

  GLuint vbo[2] = {0};

  // generate VBO and initialize them with vertex data
  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vectorf) * vertex_count, vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vectorf), NULL);

  int shader = create_shader(default_vert_shader, default_frag_shader);
  if (shader < 0){
    quit(window, 1);
  }

  glUseProgram(shader);

  // game loop
  while (!glfwWindowShouldClose(window)){
    begin_draw();
    clear(COLOR_BLACK);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);

    end_draw();
  }

  glUseProgram(0);
  glDeleteProgram(shader);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDeleteBuffers(1, vbo);
  glDeleteVertexArrays(1, vao);

  quit(window, 0);

  return 0;
}
