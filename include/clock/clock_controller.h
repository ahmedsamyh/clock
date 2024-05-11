#ifndef _CLOCK_CONTROLLER_H_
#define _CLOCK_CONTROLLER_H_

#include <windows.h>
#include <xinput.h>
#include <clock/clock_vector.h>
#include <stdbool.h>

typedef struct Controller Controller;
typedef struct Controller_button_state Controller_button_state;
typedef enum Controller_button Controller_button;
// NOTE: Very poorly named...
typedef enum Controller_state Controller_state;

enum Controller_state {
  CONTROLLER_STATE_PRESSED,
  CONTROLLER_STATE_HELD,
  CONTROLLER_STATE_RELEASED,
  CONTROLLER_STATE_COUNT,
};

struct Controller_button_state {
  bool prev_state, pressed, held, released;
};

enum Controller_button {
  CONTROLLER_DPAD_UP,		// XINPUT_GAMEPAD_DPAD_UP
  CONTROLLER_DPAD_DOWN,		// XINPUT_GAMEPAD_DPAD_DOWN
  CONTROLLER_DPAD_LEFT,		// XINPUT_GAMEPAD_DPAD_LEFT
  CONTROLLER_DPAD_RIGHT,	// XINPUT_GAMEPAD_DPAD_RIGHT
  CONTROLLER_BUTTON_START,	// XINPUT_GAMEPAD_START
  CONTROLLER_BUTTON_SELECT,	// XINPUT_GAMEPAD_BACK
  CONTROLLER_BUTTON_L3,		// XINPUT_GAMEPAD_LEFT_THUMB
  CONTROLLER_BUTTON_R3,		// XINPUT_GAMEPAD_RIGHT_THUMB
  CONTROLLER_BUTTON_L1,		// XINPUT_GAMEPAD_LEFT_SHOULDER
  CONTROLLER_BUTTON_R1,		// XINPUT_GAMEPAD_RIGHT_SHOULDER
  CONTROLLER_BUTTON_A,		// XINPUT_GAMEPAD_A
  CONTROLLER_BUTTON_B,		// XINPUT_GAMEPAD_B
  CONTROLLER_BUTTON_X,		// XINPUT_GAMEPAD_X
  CONTROLLER_BUTTON_Y,		// XINPUT_GAMEPAD_Y
  CONTROLLER_BUTTON_COUNT,
};

struct Controller {
  Vector2f left_thumb;
  Vector2f right_thumb;
  float left_trigger;
  float right_trigger;
  Controller_button_state buttons[CONTROLLER_BUTTON_COUNT];
};

void setup_controller(Controller* ctrler, XINPUT_STATE xinput_state);

#endif /* _CLOCK_CONTROLLER_H_ */
