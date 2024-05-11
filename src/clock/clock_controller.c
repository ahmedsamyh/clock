#include <clock/clock_controller.h>

#define SET_PREV_STATE(btn) ctrler->buttons[CONTROLLER_##btn].prev_state = ctrler->buttons[CONTROLLER_##btn].held
#define SET_HELD(btn, xinput_btn) ctrler->buttons[CONTROLLER_##btn].held = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_##xinput_btn)
#define SET_PRESSED(btn) ctrler->buttons[CONTROLLER_##btn].pressed = ((!ctrler->buttons[CONTROLLER_##btn].prev_state) && (ctrler->buttons[CONTROLLER_##btn].held))
#define SET_RELEASED(btn) ctrler->buttons[CONTROLLER_##btn].released = ((ctrler->buttons[CONTROLLER_##btn].prev_state) && (!ctrler->buttons[CONTROLLER_##btn].held))

#define BUTTONS_DO(action)			\
  action(DPAD_UP);				\
  action(DPAD_DOWN);				\
  action(DPAD_LEFT);				\
  action(DPAD_RIGHT);				\
  action(BUTTON_START);				\
  action(BUTTON_SELECT);			\
  action(BUTTON_L3);				\
  action(BUTTON_R3);				\
  action(BUTTON_L1);				\
  action(BUTTON_R1);				\
  action(BUTTON_A);				\
  action(BUTTON_B);				\
  action(BUTTON_X);				\
  action(BUTTON_Y)

#define BUTTONS_DO_EX(action)			\
  action(DPAD_UP, DPAD_UP);			\
  action(DPAD_DOWN, DPAD_DOWN);			\
  action(DPAD_LEFT, DPAD_LEFT);			\
  action(DPAD_RIGHT, DPAD_RIGHT);		\
  action(BUTTON_START, START);			\
  action(BUTTON_SELECT, BACK);			\
  action(BUTTON_L3, LEFT_THUMB);		\
  action(BUTTON_R3, RIGHT_THUMB);		\
  action(BUTTON_L1, LEFT_SHOULDER);		\
  action(BUTTON_R1, RIGHT_SHOULDER);		\
  action(BUTTON_A, A);				\
  action(BUTTON_B, B);				\
  action(BUTTON_X, X);				\
  action(BUTTON_Y, Y)

void setup_controller(Controller* ctrler, XINPUT_STATE xinput_state) {
  ctrler->left_thumb.x  = (float)xinput_state.Gamepad.sThumbLX / (float)(SHRT_MAX);
  ctrler->left_thumb.y  = (float)xinput_state.Gamepad.sThumbLY / (float)(SHRT_MAX) * -1.f;
  ctrler->right_thumb.x = (float)xinput_state.Gamepad.sThumbRX / (float)(SHRT_MAX);
  ctrler->right_thumb.y = (float)xinput_state.Gamepad.sThumbRY / (float)(SHRT_MAX) * -1.f;

  ctrler->left_trigger = (float)xinput_state.Gamepad.bLeftTrigger / 255.f;
  ctrler->right_trigger = (float)xinput_state.Gamepad.bRightTrigger / 255.f;

  BUTTONS_DO(SET_PREV_STATE);
  BUTTONS_DO_EX(SET_HELD);
  BUTTONS_DO(SET_PRESSED);
  BUTTONS_DO(SET_RELEASED);
}
