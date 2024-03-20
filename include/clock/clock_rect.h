#ifndef _CLOCK_RECT_H_
#define _CLOCK_RECT_H_

#include <clock/clock_vector.h>
#include <stdbool.h>

typedef struct {
  Vector2f pos;
  Vector2f size;
} Rect;

bool Rect_contains_point(Rect r1, Vector2f p);
bool Rect_contains_rect(Rect r1, Rect r2);
bool Rect_intersects_rect(Rect r1, Rect r2);
bool Rect_resolve_rect_collision(Rect* rect1, const Rect rect2);

#endif /* _CLOCK_RECT_H_ */
