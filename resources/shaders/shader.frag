#version 460 core

in vec2 v_texcoord;
in vec4 v_color;

uniform sampler2D tex;
uniform sampler2D screen;

out vec4 frag_color;

void main(void) {
  vec4 screen_color = texture(screen, v_texcoord);
  frag_color = screen_color;

  // float brightness = screen_color.r + screen_color.g + screen_color.b;
  // if (brightness <= 0.0) {
  //   frag_color = texture(tex, v_texcoord);
  // } else {
  //   // frag_color = mix(screen_color, texture(tex, v_texcoord), 0.5);
  //   frag_color = vec4(1.0, 0.0, 0.0, 1.0);
  // }
}
