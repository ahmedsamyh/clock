#include <rpg/2.5d.h>

Vector2f two_5d_to_2d(Vector3f v) {
  return (Vector2f) {
    .x = v.x,
    .y = v.y - v.z
  };
}

void change_height(Vector3f* v, float height) {
  if (height < 0.f) height = 0.f;
  v->z = height;
}
