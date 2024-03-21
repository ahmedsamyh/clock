#ifndef _CLOCK_MOUSE_H_
#define _CLOCK_MOUSE_H_

#include <stdbool.h>

#define MOUSE_BUTTONS_COUNT 4

typedef struct Mouse Mouse;

typedef enum {
  MOUSE_BUTTON_LEFT,
  MOUSE_BUTTON_RIGHT,
  MOUSE_BUTTON_MIDDLE,
  MOUSE_BUTTON_COUNT,
} Mouse_button;

struct Mouse {
  uint32_t button;
  bool held;
  bool prev_state;
  bool just_pressed;
  bool released;
  bool pressed;
};

#endif /* _CLOCK_MOUSE_H_ */
