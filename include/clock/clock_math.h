#ifndef _MATH_H_
#define _MATH_H_

#include <clock/clock_vector.h>

#define PI (3.14159265359f)
#define TWO_PI (PI*2.f)

float deg2rad(float deg);
float rad2deg(float rad);

// 0..width, 0..height to -1..1, 1..-1 (y is flipped in gl space; y goes up)
// TODO: Should this be world_to.. or screen_to..
Vector3f screen_to_gl_space(Vector3f v, Vector2f screen_size);

#endif /* _MATH_H_ */
