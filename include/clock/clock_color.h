#ifndef _CLOCK_COLOR_H_
#define _CLOCK_COLOR_H_

#include <clock/clock_vector.h>
// Color
typedef Vector4f Color; // 0.0..1.0

#define COLOR_BLACK (Color){0.f,0.f,0.f,1.f}
#define COLOR_WHITE (Color){1.f,1.f,1.f,1.f}
#define COLOR_RED   (Color){1.f,0.f,0.f,1.f}
#define COLOR_GREEN (Color){0.f,1.f,0.f,1.f}
#define COLOR_BLUE (Color){0.f,0.f,1.f,1.f}

// hex should be in format 0xAAGGBBRR
Color hex_to_color(int color);

Color color_alpha(Color col, float alpha);

#endif /* _CLOCK_COLOR_H_ */
