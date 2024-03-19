#include <clock/clock_math.h>

float deg2rad(float deg){
  return (deg / 360.f)*TWO_PI;
}

float rad2deg(float rad){
 return (rad / TWO_PI)*360.f;
}

// TODO: This will be deceprated when we use the projection matrix
Vector3f screen_to_gl_space(Vector3f v, Vector2f screen_size) {
  // avoid divide-by-zero error
  if (screen_size.x == 0) screen_size.x = 1.f;
  if (screen_size.y == 0) screen_size.y = 1.f;

  float depth = screen_size.x;
  return (Vector3f) {
    .x = (v.x / screen_size.x)         * 2.f - 1.f, // -1..+1
    .y = (1.f - (v.y / screen_size.y)) * 2.f - 1.f, // +1..-1
    .z = (v.z / depth)                 * 2.f - 1.f, // -1..+1
  };
}
