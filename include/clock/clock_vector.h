#ifndef _CLOCK_VECTOR_H_
#define _CLOCK_VECTOR_H_
#include <math.h>

// Vector2f --------------------------------------------------
typedef struct {
  union {
    float x;
    float u;
  };
  union {
    float y;
    float v;
  };
} Vector2f;

Vector2f v2f_add(Vector2f v1, Vector2f v2);
Vector2f v2f_sub(Vector2f v1, Vector2f v2);
Vector2f v2f_mul(Vector2f v1, Vector2f v2);
Vector2f v2f_div(Vector2f v1, Vector2f v2);
Vector2f v2f_adds(Vector2f v, float num);
Vector2f v2f_subs(Vector2f v, float num);
Vector2f v2f_muls(Vector2f v, float num);
Vector2f v2f_divs(Vector2f v, float num);
float v2f_degree(Vector2f v);
float v2f_radian(Vector2f v);
float v2f_mag(Vector2f v);
float v2f_mag2(Vector2f v);
float v2f_dist(Vector2f v);
float v2f_dist2(Vector2f v);
Vector2f v2f_normalize(Vector2f v);
Vector2f v2f_lerp(Vector2f start, Vector2f end, float t);

// Vector3f --------------------------------------------------
typedef struct {
  float x;
  float y;
  float z;
} Vector3f;

Vector3f v3f_add(Vector3f v1, Vector3f v2);
Vector3f v3f_sub(Vector3f v1, Vector3f v2);
Vector3f v3f_mul(Vector3f v1, Vector3f v2);
Vector3f v3f_div(Vector3f v1, Vector3f v2);
Vector3f v3f_adds(Vector3f v, float num);
Vector3f v3f_subs(Vector3f v, float num);
Vector3f v3f_muls(Vector3f v, float num);
Vector3f v3f_divs(Vector3f v, float num);
float v3f_mag(Vector3f v);
float v3f_mag2(Vector3f v);
float v3f_dist(Vector3f v);
float v3f_dist2(Vector3f v);
Vector3f v3f_normalize(Vector3f v);
Vector3f v3f_lerp(Vector3f start, Vector3f end, float t);

// Vector4f --------------------------------------------------
typedef struct {
  union {
    float x;
    float r;
  };
  union {
    float y;
    float g;
  };
  union {
    float z;
    float b;
  };
  union {
    float w;
    float a;
  };
} Vector4f;

Vector4f v4f_add(Vector4f v1, Vector4f v2);
Vector4f v4f_sub(Vector4f v1, Vector4f v2);
Vector4f v4f_mul(Vector4f v1, Vector4f v2);
Vector4f v4f_div(Vector4f v1, Vector4f v2);
Vector4f v4f_adds(Vector4f v, float num);
Vector4f v4f_subs(Vector4f v, float num);
Vector4f v4f_muls(Vector4f v, float num);
Vector4f v4f_divs(Vector4f v, float num);
float v4f_mag(Vector4f v);
float v4f_mag2(Vector4f v);
float v4f_dist(Vector4f v);
float v4f_dist2(Vector4f v);
Vector4f v4f_normalize(Vector4f v);
Vector4f v4f_lerp(Vector4f start, Vector4f end, float t);

// Vector2d --------------------------------------------------
typedef struct {
  union {
    double x;
    double u;
  };
  union {
    double y;
    double v;
  };
} Vector2d;

Vector2d v2d_add(Vector2d v1, Vector2d v2);
Vector2d v2d_sub(Vector2d v1, Vector2d v2);
Vector2d v2d_mul(Vector2d v1, Vector2d v2);
Vector2d v2d_div(Vector2d v1, Vector2d v2);
Vector2d v2d_adds(Vector2d v, double num);
Vector2d v2d_subs(Vector2d v, double num);
Vector2d v2d_muls(Vector2d v, double num);
Vector2d v2d_divs(Vector2d v, double num);
float v2d_degree(Vector2d v);
float v2d_radian(Vector2d v);
float v2d_mag(Vector2d v);
float v2d_mag2(Vector2d v);
float v2d_dist(Vector2d v);
float v2d_dist2(Vector2d v);
Vector2d v2d_normalize(Vector2d v);
Vector2d v2d_lerp(Vector2d start, Vector2d end, float t);

// Vector3d --------------------------------------------------
typedef struct {
  double x;
  double y;
  double z;
} Vector3d;

Vector3d v3d_add(Vector3d v1, Vector3d v2);
Vector3d v3d_sub(Vector3d v1, Vector3d v2);
Vector3d v3d_mul(Vector3d v1, Vector3d v2);
Vector3d v3d_div(Vector3d v1, Vector3d v2);
Vector3d v3d_adds(Vector3d v, double num);
Vector3d v3d_subs(Vector3d v, double num);
Vector3d v3d_muls(Vector3d v, double num);
Vector3d v3d_divs(Vector3d v, double num);
float v3d_mag(Vector3d v);
float v3d_mag2(Vector3d v);
float v3d_dist(Vector3d v);
float v3d_dist2(Vector3d v);
Vector3d v3d_normalize(Vector3d v);
Vector3d v3d_lerp(Vector3d start, Vector3d end, float t);

// Vector4d --------------------------------------------------
typedef struct {
  union {
    double x;
    double r;
  };
  union {
    double y;
    double g;
  };
  union {
    double z;
    double b;
  };
  union {
    double w;
    double a;
  };
} Vector4d;

Vector4d v4d_add(Vector4d v1, Vector4d v2);
Vector4d v4d_sub(Vector4d v1, Vector4d v2);
Vector4d v4d_mul(Vector4d v1, Vector4d v2);
Vector4d v4d_div(Vector4d v1, Vector4d v2);
Vector4d v4d_adds(Vector4d v, double num);
Vector4d v4d_subs(Vector4d v, double num);
Vector4d v4d_muls(Vector4d v, double num);
Vector4d v4d_divs(Vector4d v, double num);
float v4d_mag(Vector4d v);
float v4d_mag2(Vector4d v);
float v4d_dist(Vector4d v);
float v4d_dist2(Vector4d v);
Vector4d v4d_normalize(Vector4d v);
Vector4d v4d_lerp(Vector4d start, Vector4d end, float t);

// Vector2i --------------------------------------------------
typedef struct {
  union {
    int x;
    int u;
  };
  union {
    int y;
    int v;
  };
} Vector2i;

Vector2i v2i_add(Vector2i v1, Vector2i v2);
Vector2i v2i_sub(Vector2i v1, Vector2i v2);
Vector2i v2i_mul(Vector2i v1, Vector2i v2);
Vector2i v2i_div(Vector2i v1, Vector2i v2);
Vector2i v2i_adds(Vector2i v, int num);
Vector2i v2i_subs(Vector2i v, int num);
Vector2i v2i_muls(Vector2i v, int num);
Vector2i v2i_divs(Vector2i v, int num);
float v2i_degree(Vector2i v);
float v2i_radian(Vector2i v);
float v2i_mag(Vector2i v);
float v2i_mag2(Vector2i v);
float v2i_dist(Vector2i v);
float v2i_dist2(Vector2i v);
Vector2i v2i_normalize(Vector2i v);

// Vector3i --------------------------------------------------
typedef struct {
  int x;
  int y;
  int z;
} Vector3i;

Vector3i v3i_add(Vector3i v1, Vector3i v2);
Vector3i v3i_sub(Vector3i v1, Vector3i v2);
Vector3i v3i_mul(Vector3i v1, Vector3i v2);
Vector3i v3i_div(Vector3i v1, Vector3i v2);
Vector3i v3i_adds(Vector3i v, int num);
Vector3i v3i_subs(Vector3i v, int num);
Vector3i v3i_muls(Vector3i v, int num);
Vector3i v3i_divs(Vector3i v, int num);
float v3i_mag(Vector3i v);
float v3i_mag2(Vector3i v);
float v3i_dist(Vector3i v);
float v3i_dist2(Vector3i v);
Vector3i v3i_normalize(Vector3i v);

// Vector4i --------------------------------------------------
typedef struct {
  union {
    int x;
    int r;
  };
  union {
    int y;
    int g;
  };
  union {
    int z;
    int b;
  };
  union {
    int w;
    int a;
  };
} Vector4i;

Vector4i v4i_add(Vector4i v1, Vector4i v2);
Vector4i v4i_sub(Vector4i v1, Vector4i v2);
Vector4i v4i_mul(Vector4i v1, Vector4i v2);
Vector4i v4i_div(Vector4i v1, Vector4i v2);
Vector4i v4i_adds(Vector4i v, int num);
Vector4i v4i_subs(Vector4i v, int num);
Vector4i v4i_muls(Vector4i v, int num);
Vector4i v4i_divs(Vector4i v, int num);
float v4i_mag(Vector4i v);
float v4i_mag2(Vector4i v);
float v4i_dist(Vector4i v);
float v4i_dist2(Vector4i v);
Vector4i v4i_normalize(Vector4i v);

#endif //_CLOCK_VECTOR_H_
