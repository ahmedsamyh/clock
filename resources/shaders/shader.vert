#version 330 core

layout(location = 0) in vec4 in_pos;

void main(void){
  gl_Position = in_pos;
}
