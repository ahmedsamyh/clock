#ifndef _MATRIX_H_
#define _MATRIX_H_
#include <vector.h>
#include <clock_math.h>

typedef struct {
  //   [row][col]
  float m[4][4];
} Matrix4;

#define MAT4_FMT "\n%.2f, %.2f, %.2f, %.2f\n" "%.2f, %.2f, %.2f, %.2f\n" "%.2f, %.2f, %.2f, %.2f\n" "%.2f, %.2f, %.2f, %.2f\n"
#define MAT4_ARG(mat) mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3], mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3], mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]

Matrix4 Mat4_identity(void);
Vector4f Mat4_translate_vector(Vector4f v, Vector3f by);
Vector4f Mat4_scale_vector(Vector4f v, Vector3f by);
Vector4f Mat4_rotate_x_vector(Vector4f v, float deg);
Vector4f Mat4_rotate_y_vector(Vector4f v, float deg);
Vector4f Mat4_rotate_z_vector(Vector4f v, float deg);
Vector4f Mat4_mul(Matrix4 m, Vector4f v);

// TODO: Make a generic Matrix struct that can be of any number of rows and cols
// since this is a very stupid and lazy way to define matrices.

typedef struct {
  float m[3][3];
} Matrix3;

Vector4f Mat3_mul(Matrix3 m, Vector4f v);

#endif /* _MATRIX_H_ */
//////////////////////////////////////////////////
#ifdef MATRIX_IMPLEMENTATION
#include <math.h>

Matrix4 Mat4_identity(void){
  return (Matrix4){
    .m = {{1.f, 0.f, 0.f, 0.f},
	  {0.f, 1.f, 0.f, 0.f},
	  {0.f, 0.f, 1.f, 0.f},
	  {0.f, 0.f, 0.f, 1.f}}
  };
}

Vector4f Mat4_translate_vector(Vector4f v, Vector3f by){
  Vector4f r = {0};

  Matrix4 translation_mat = {
    .m = {{1.f, 0.f, 0.f, by.x},
	  {0.f, 1.f, 0.f, by.y},
	  {0.f, 0.f, 1.f, by.z},
	  {0.f, 0.f, 0.f, 1.f}}
  };

  return Mat4_mul(translation_mat, v);
}

Vector4f Mat4_scale_vector(Vector4f v, Vector3f by){
  Vector4f r = {0};

  Matrix4 translation_mat = {
    .m = {{by.x, 0.f, 0.f, 0.f},
	  {0.f, by.y, 0.f, 0.f},
	  {0.f, 0.f, by.z, 0.f},
	  {0.f, 0.f, 0.f, 1.f}}
  };

  return Mat4_mul(translation_mat, v);

}

Vector4f Mat4_rotate_x_vector(Vector4f v, float deg){
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix3 m = {
    .m = {{1, 0,          0        },
	  {0, cosf(rad), -sinf(rad)},
          {0, sinf(rad),  cosf(rad)}}
  };

  return Mat3_mul(m, v);
}

Vector4f Mat4_rotate_y_vector(Vector4f v, float deg){
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix3 m = {
    .m = {{cosf(rad),  0, sinf(rad)},
	  {0,          1, 0        },
	  {-sinf(rad), 0, cosf(rad)}}
  };
  return Mat3_mul(m, v);

}

Vector4f Mat4_rotate_z_vector(Vector4f v, float deg){
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix3 m = {
    .m = {{cosf(rad), -sinf(rad), 0},
	  {sinf(rad),  cosf(rad), 0},
	  {0,          0,         1}}
  };
  return Mat3_mul(m, v);
}



Vector4f Mat4_mul(Matrix4 m, Vector4f v){
  Vector4f res = {0};

  float vv[4] = {v.x, v.y, v.z, v.w};
  float vs[4] = {0};

  for (size_t j = 0; j < 4; ++j){
    for (size_t i = 0; i < 4; ++i){
      vs[j] += m.m[j][i] * vv[i];
    }
  }

  res.x = vs[0];
  res.y = vs[1];
  res.z = vs[2];
  res.w = vs[3];

  return res;
}

Vector4f Mat3_mul(Matrix3 m, Vector4f v){
  Vector4f res = {0};

  float vv[4] = {v.x, v.y, v.z, v.w};
  float vs[4] = {0};

  for (size_t j = 0; j < 3; ++j){
    for (size_t i = 0; i < 3; ++i){
      vs[j] += m.m[j][i] * vv[i];
    }
  }

  res.x = vs[0];
  res.y = vs[1];
  res.z = vs[2];
  res.w = v.w;

  return res;
}

#endif // MATRIX_IMPLEMENTATION
