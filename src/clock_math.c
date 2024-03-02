#include <clock_math.h>

float deg2rad(float deg){
  return (deg / 360.f)*TWO_PI;
}

float rad2deg(float rad){
 return (rad / TWO_PI)*360.f;
}
