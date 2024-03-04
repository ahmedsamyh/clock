#ifndef _MATRIX_H_
#define _MATRIX_H_
#include <clock/clock_vector.h>
#include <clock/clock_math.h>
#include <math.h>

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
