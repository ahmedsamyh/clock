#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texcoord;

uniform mat4 model;

out vec2 v_texcoord;
out vec4 v_color;

void main(void){
  gl_Position = position;
  v_texcoord = texcoord;
  v_color = color;
}
