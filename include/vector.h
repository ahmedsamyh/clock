#ifndef _VECTOR_H_
#define _VECTOR_H_

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
float v2f_mag(Vector2f v);
float v2f_mag2(Vector2f v);
float v2f_dist(Vector2f v);
float v2f_dist2(Vector2f v);

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
float v2i_mag(Vector2i v);
float v2i_mag2(Vector2i v);
float v2i_dist(Vector2i v);
float v2i_dist2(Vector2i v);

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

#endif //_VECTOR_H_
//////////////////////////////////////////////////
#ifdef VECTOR_IMPLEMENTATION

// Vector2f --------------------------------------------------

Vector2f v2f_add(Vector2f v1, Vector2f v2){
  return (Vector2f){
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
  };
}
Vector2f v2f_sub(Vector2f v1, Vector2f v2){
  return (Vector2f){
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
  };
}
Vector2f v2f_mul(Vector2f v1, Vector2f v2){
  return (Vector2f){
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
  };
}
Vector2f v2f_div(Vector2f v1, Vector2f v2){
  return (Vector2f){
    .x = v1.x / v2.x,
    .y = v1.y / v2.y,
  };
}
Vector2f v2f_adds(Vector2f v1, float num){
  return (Vector2f){
    .x = v1.x + num,
    .y = v1.y + num,
  };
}
Vector2f v2f_subs(Vector2f v1, float num){
  return (Vector2f){
    .x = v1.x - num,
    .y = v1.y - num,
  };
}
Vector2f v2f_muls(Vector2f v1, float num){
  return (Vector2f){
    .x = v1.x * num,
    .y = v1.y * num,
  };
}
Vector2f v2f_divs(Vector2f v1, float num){
  return (Vector2f){
    .x = v1.x / num,
    .y = v1.y / num,
  };
}
float v2f_mag(Vector2f v){
  return sqrtf((v.x*v.x) + (v.y*v.y));
}
float v2f_mag2(Vector2f v){
  return ((v.x*v.x) + (v.y*v.y));
}
float v2f_dist(Vector2f v){
  return sqrtf((v.x*v.x) + (v.y*v.y));
}
float v2f_dist2(Vector2f v){
  return ((v.x*v.x) + (v.y*v.y));
}

// Vector3f --------------------------------------------------

Vector3f v3f_add(Vector3f v1, Vector3f v2){
  return (Vector3f){
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
    .z = v1.z + v2.z,
  };
}
Vector3f v3f_sub(Vector3f v1, Vector3f v2){
  return (Vector3f){
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
    .z = v1.z - v2.z,
  };
}
Vector3f v3f_mul(Vector3f v1, Vector3f v2){
  return (Vector3f){
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
    .z = v1.z * v2.z,
  };
}
Vector3f v3f_div(Vector3f v1, Vector3f v2){
  return (Vector3f){
    .x = v1.x / v2.x,
    .y = v1.y / v2.y,
    .z = v1.z / v2.z,
  };
}
Vector3f v3f_adds(Vector3f v1, float num){
  return (Vector3f){
    .x = v1.x + num,
    .y = v1.y + num,
    .z = v1.z + num,
  };
}
Vector3f v3f_subs(Vector3f v1, float num){
  return (Vector3f){
    .x = v1.x - num,
    .y = v1.y - num,
    .z = v1.z - num,
  };
}
Vector3f v3f_muls(Vector3f v1, float num){
  return (Vector3f){
    .x = v1.x * num,
    .y = v1.y * num,
    .z = v1.z * num,
  };
}
Vector3f v3f_divs(Vector3f v1, float num){
  return (Vector3f){
    .x = v1.x / num,
    .y = v1.y / num,
    .z = v1.z / num,
  };
}
float v3f_mag(Vector3f v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float v3f_mag2(Vector3f v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float v3f_dist(Vector3f v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float v3f_dist2(Vector3f v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}

// Vector4f --------------------------------------------------

Vector4f v4f_add(Vector4f v1, Vector4f v2){
  return (Vector4f){
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
    .z = v1.z + v2.z,
    .w = v1.w + v2.w,
  };
}
Vector4f v4f_sub(Vector4f v1, Vector4f v2){
  return (Vector4f){
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
    .z = v1.z - v2.z,
    .w = v1.w - v2.w,
  };
}
Vector4f v4f_mul(Vector4f v1, Vector4f v2){
  return (Vector4f){
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
    .z = v1.z * v2.z,
    .w = v1.w * v2.w,
  };
}
Vector4f v4f_div(Vector4f v1, Vector4f v2){
  return (Vector4f){
    .x = v1.x / v2.x,
    .y = v1.y / v2.y,
    .z = v1.z / v2.z,
    .w = v1.w / v2.w,
  };
}
Vector4f v4f_adds(Vector4f v1, float num){
  return (Vector4f){
    .x = v1.x + num,
    .y = v1.y + num,
    .z = v1.z + num,
    .w = v1.w + num,
  };
}
Vector4f v4f_subs(Vector4f v1, float num){
  return (Vector4f){
    .x = v1.x - num,
    .y = v1.y - num,
    .z = v1.z - num,
    .w = v1.w - num,
  };
}
Vector4f v4f_muls(Vector4f v1, float num){
  return (Vector4f){
    .x = v1.x * num,
    .y = v1.y * num,
    .z = v1.z * num,
    .w = v1.w * num,
  };
}
Vector4f v4f_divs(Vector4f v1, float num){
  return (Vector4f){
    .x = v1.x / num,
    .y = v1.y / num,
    .z = v1.z / num,
    .w = v1.w / num,
  };
}
float v4f_mag(Vector4f v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
float v4f_mag2(Vector4f v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
float v4f_dist(Vector4f v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
float v4f_dist2(Vector4f v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}

// Vector2i --------------------------------------------------

Vector2i v2i_add(Vector2i v1, Vector2i v2){
  return (Vector2i){
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
  };
}
Vector2i v2i_sub(Vector2i v1, Vector2i v2){
  return (Vector2i){
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
  };
}
Vector2i v2i_mul(Vector2i v1, Vector2i v2){
  return (Vector2i){
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
  };
}
Vector2i v2i_div(Vector2i v1, Vector2i v2){
  return (Vector2i){
    .x = v1.x / v2.x,
    .y = v1.y / v2.y,
  };
}
Vector2i v2i_adds(Vector2i v1, int num){
  return (Vector2i){
    .x = v1.x + num,
    .y = v1.y + num,
  };
}
Vector2i v2i_subs(Vector2i v1, int num){
  return (Vector2i){
    .x = v1.x - num,
    .y = v1.y - num,
  };
}
Vector2i v2i_muls(Vector2i v1, int num){
  return (Vector2i){
    .x = v1.x * num,
    .y = v1.y * num,
  };
}
Vector2i v2i_divs(Vector2i v1, int num){
  return (Vector2i){
    .x = v1.x / num,
    .y = v1.y / num,
  };
}
float v2i_mag(Vector2i v){
  return sqrtf((v.x*v.x) + (v.y*v.y));
}
float v2i_mag2(Vector2i v){
  return ((v.x*v.x) + (v.y*v.y));
}
float v2i_dist(Vector2i v){
  return sqrtf((v.x*v.x) + (v.y*v.y));
}
float v2i_dist2(Vector2i v){
  return ((v.x*v.x) + (v.y*v.y));
}

// Vector3i --------------------------------------------------

Vector3i v3i_add(Vector3i v1, Vector3i v2){
  return (Vector3i){
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
    .z = v1.z + v2.z,
  };
}
Vector3i v3i_sub(Vector3i v1, Vector3i v2){
  return (Vector3i){
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
    .z = v1.z - v2.z,
  };
}
Vector3i v3i_mul(Vector3i v1, Vector3i v2){
  return (Vector3i){
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
    .z = v1.z * v2.z,
  };
}
Vector3i v3i_div(Vector3i v1, Vector3i v2){
  return (Vector3i){
    .x = v1.x / v2.x,
    .y = v1.y / v2.y,
    .z = v1.z / v2.z,
  };
}
Vector3i v3i_adds(Vector3i v1, int num){
  return (Vector3i){
    .x = v1.x + num,
    .y = v1.y + num,
    .z = v1.z + num,
  };
}
Vector3i v3i_subs(Vector3i v1, int num){
  return (Vector3i){
    .x = v1.x - num,
    .y = v1.y - num,
    .z = v1.z - num,
  };
}
Vector3i v3i_muls(Vector3i v1, int num){
  return (Vector3i){
    .x = v1.x * num,
    .y = v1.y * num,
    .z = v1.z * num,
  };
}
Vector3i v3i_divs(Vector3i v1, int num){
  return (Vector3i){
    .x = v1.x / num,
    .y = v1.y / num,
    .z = v1.z / num,
  };
}
float v3i_mag(Vector3i v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float v3i_mag2(Vector3i v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float v3i_dist(Vector3i v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
float v3i_dist2(Vector3i v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}

// Vector4i --------------------------------------------------

Vector4i v4i_add(Vector4i v1, Vector4i v2){
  return (Vector4i){
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
    .z = v1.z + v2.z,
    .w = v1.w + v2.w,
  };
}
Vector4i v4i_sub(Vector4i v1, Vector4i v2){
  return (Vector4i){
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
    .z = v1.z - v2.z,
    .w = v1.w - v2.w,
  };
}
Vector4i v4i_mul(Vector4i v1, Vector4i v2){
  return (Vector4i){
    .x = v1.x * v2.x,
    .y = v1.y * v2.y,
    .z = v1.z * v2.z,
    .w = v1.w * v2.w,
  };
}
Vector4i v4i_div(Vector4i v1, Vector4i v2){
  return (Vector4i){
    .x = v1.x / v2.x,
    .y = v1.y / v2.y,
    .z = v1.z / v2.z,
    .w = v1.w / v2.w,
  };
}
Vector4i v4i_adds(Vector4i v1, int num){
  return (Vector4i){
    .x = v1.x + num,
    .y = v1.y + num,
    .z = v1.z + num,
    .w = v1.w + num,
  };
}
Vector4i v4i_subs(Vector4i v1, int num){
  return (Vector4i){
    .x = v1.x - num,
    .y = v1.y - num,
    .z = v1.z - num,
    .w = v1.w - num,
  };
}
Vector4i v4i_muls(Vector4i v1, int num){
  return (Vector4i){
    .x = v1.x * num,
    .y = v1.y * num,
    .z = v1.z * num,
    .w = v1.w * num,
  };
}
Vector4i v4i_divs(Vector4i v1, int num){
  return (Vector4i){
    .x = v1.x / num,
    .y = v1.y / num,
    .z = v1.z / num,
    .w = v1.w / num,
  };
}
float v4i_mag(Vector4i v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
float v4i_mag2(Vector4i v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
float v4i_dist(Vector4i v){
  return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
float v4i_dist2(Vector4i v){
  return ((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
}
#endif // VECTOR_IMPLEMENTATION
