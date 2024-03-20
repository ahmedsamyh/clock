#include <clock/clock_color.h>

Color hex_to_color(int color) {
  return (Color) {
    .r = ((color >> (8 * 0)) & 0xFF) / 255.f,
    .g = ((color >> (8 * 1)) & 0xFF) / 255.f,
    .b = ((color >> (8 * 2)) & 0xFF) / 255.f,
    .a = ((color >> (8 * 3)) & 0xFF) / 255.f,
  };
}

Color color_alpha(Color col, float alpha) {
  return (Color) {
    col.r,
    col.g,
    col.b,
    alpha
  };
}
