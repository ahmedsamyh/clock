#ifndef _CLOCK_KEY_H_
#define _CLOCK_KEY_H_

#include <stdbool.h>

typedef struct Key Key;

struct Key {
  uint32_t key;
  bool held;
  bool prev_state;
  bool just_pressed;
  bool released;
  bool pressed;
};

#endif /* _CLOCK_KEY_H_ */
