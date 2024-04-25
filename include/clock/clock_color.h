#ifndef _CLOCK_COLOR_H_
#define _CLOCK_COLOR_H_

#include <clock/clock_vector.h>
// Color
typedef Vector4f Color; // 0.0..1.0

#define COLOR_BLACK            (Color){0, 0, 0, 1}
#define COLOR_BLANK            (Color){0, 0, 0, 0}
#define COLOR_WHITE            (Color){1, 1, 1, 1}
#define COLOR_RED              (Color){1, 0, 0, 1}
#define COLOR_GREEN            (Color){0, 1, 0, 1}
#define COLOR_BLUE             (Color){0, 0, 1, 1}
#define COLOR_MAGENTA          (Color){1, 0, 1, 1}
#define COLOR_YELLOW           (Color){1, 1, 0, 1}
#define COLOR_CYAN             (Color){0, 1, 1, 1}
#define COLOR_GOLD             (Color){1, 0.84f, 0, 1}
#define COLOR_ORANGE           (Color){1, 0.64f, 0, 1}
#define COLOR_GRAY             color_from_hex(0xFFBEBEBE)
#define COLOR_MIDNIGHT_BLUE    color_from_hex(0xFF701919)
#define COLOR_NAVY             color_from_hex(0xFF800000)
#define COLOR_GREEN_YELLOW     color_from_hex(0XFF2FFFAD)
#define COLOR_GRAY12           color_from_hex(0XFF1F1F1F)

// hex should be in format 0xAAGGBBRR
Color color_from_hex(int color);

Color color_alpha(Color col, float alpha);

#endif /* _CLOCK_COLOR_H_ */
