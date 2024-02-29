#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct {
  float x;
  float y;
  float z;
  float w;
} Vectorf;

#define vf_fmt "(%.2f, %.2f, %.2f, %.2f)"
#define vf_arg(vf) vf.x, vf.y, vf.z, vf.w

Vectorf vf_add(Vectorf v1, Vectorf v2);
Vectorf vf_adds(Vectorf v1, float num);
Vectorf vf_sub(Vectorf v1, Vectorf v2);
Vectorf vf_subs(Vectorf v1, float num);
Vectorf vf_mult(Vectorf v1, Vectorf v2);
Vectorf vf_mults(Vectorf v1, float num);
Vectorf vf_div(Vectorf v1, Vectorf v2);
Vectorf vf_divs(Vectorf v1, float num);
float vf_mag(const Vectorf v);
float vf_magSq(const Vectorf v);

#endif /* _VECTOR_H_ */

#ifdef VECTOR_IMPLEMENTATION

Vectorf vf_add(Vectorf v1, Vectorf v2) {
  return (Vectorf){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}
Vectorf vf_adds(Vectorf v1, float num) {
  return (Vectorf){v1.x + num, v1.y + num, v1.z + num, v1.w + num};
}
Vectorf vf_sub(Vectorf v1, Vectorf v2) {
  return (Vectorf){v1.x - v2.x, v1.y - v2.y};
}
Vectorf vf_subs(Vectorf v1, float num) {
  return (Vectorf){v1.x - num, v1.y - num, v1.z - num, v1.w - num};
}
Vectorf vf_mult(Vectorf v1, Vectorf v2) {
  return (Vectorf){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
}
Vectorf vf_mults(Vectorf v1, float num) {
  return (Vectorf){v1.x * num, v1.y * num, v1.z * num, v1.w * num};
}
Vectorf vf_div(Vectorf v1, Vectorf v2) {
  return (Vectorf){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w};
}
Vectorf vf_divs(Vectorf v1, float num) {
  return (Vectorf){v1.x / num, v1.y / num, v1.z / num, v1.w / num};
}
float vf_mag(const Vectorf v) { return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)); }
float vf_magSq(const Vectorf v) { return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)); }

#endif
