#ifndef _CLOCK_TIMER_H_
#define _CLOCK_TIMER_H_

#include <stdbool.h>

typedef struct Timer Timer;
typedef struct Alarm Alarm;

#define TIMER_MEMBERS \
  float time

struct Timer {
  TIMER_MEMBERS;
};

void Timer_run(Timer* timer, float delta);

struct Alarm {
  TIMER_MEMBERS;
  float alarm_time;
};

bool Alarm_on_alarm(Alarm* alarm, float delta);

#endif /* _CLOCK_TIMER_H_ */
