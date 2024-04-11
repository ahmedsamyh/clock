#include <clock/clock_timer.h>

void Timer_run(Timer* timer, float delta) {
  timer->time += delta;
}

bool Alarm_on_alarm(Alarm* alarm, float delta) {
  Timer_run((Timer*)alarm, delta);

  if (alarm->time >= alarm->alarm_time) {
    alarm->time -= alarm->alarm_time;
    return true;
  }

  return false;
}
