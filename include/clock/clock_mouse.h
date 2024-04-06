#ifndef _CLOCK_MOUSE_H_
#define _CLOCK_MOUSE_H_

#include <stdbool.h>
#include <clock/clock_key.h>

#define MOUSE_BUTTONS_COUNT 4

typedef struct Mouse Mouse;

typedef enum {
  MOUSE_BUTTON_LEFT,
  MOUSE_BUTTON_RIGHT,
  MOUSE_BUTTON_MIDDLE,
  MOUSE_BUTTON_COUNT,
} Mouse_button;

typedef enum {
  MOUSE_STATE_PRESSED,
  MOUSE_STATE_RELEASED,
  MOUSE_STATE_HELD,
  MOUSE_STATE_COUNT,
} Mouse_state;

struct Mouse {
  uint32_t button;
  bool held;
  bool prev_state;
  bool just_pressed;
  bool released;
  bool pressed;
};

#endif /* _CLOCK_MOUSE_H_ */
