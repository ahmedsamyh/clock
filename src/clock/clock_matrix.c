#include <clock/clock_matrix.h>
#include <assert.h>

Matrix4 Mat4_identity(void){
  return (Matrix4){
    .m = {{1.f, 0.f, 0.f, 0.f},
	  {0.f, 1.f, 0.f, 0.f},
	  {0.f, 0.f, 1.f, 0.f},
	  {0.f, 0.f, 0.f, 1.f}}
  };
}

Matrix4 Mat4_translate(Matrix4 mat, Vector3f by) {
  Matrix4 translation_mat = {
    .m = {{1,   0,   0, by.x},
	  {0,   1,   0, by.y},
	  {0,   0,   1, by.z},
	  {0,   0,   0, 1   }}
  };

  return Mat4_mul(mat, translation_mat);
}

Matrix4 Mat4_scale(Matrix4 mat, Vector3f by) {
  Matrix4 scale_mat = {
    .m = {{by.x, 0,    0,    0},
	  {0,    by.y, 0,    0},
	  {0,    0,    by.z, 0},
	  {0,    0,    0,    1}}
  };

  return Mat4_mul(mat, scale_mat);
}

Matrix4 Mat4_rotate_x(Matrix4 mat, float deg) {
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix4 m = {
    .m = {{1, 0,          0        , 0},
	  {0, cos(rad), -sin(rad), 0},
          {0, sin(rad),  cos(rad), 0},
          {0, 0,          0,         1}}
  };

  return Mat4_mul(mat, m);
}

Matrix4 Mat4_rotate_y(Matrix4 mat, float deg) {
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix4 m = {
    .m = {{cos(rad),  0, sin(rad), 0},
	  {0,          1, 0        , 0},
	  {-sin(rad), 0, cos(rad), 0},
	  {0,          0, 0        , 1}}
  };
  return Mat4_mul(mat, m);
}

Matrix4 Mat4_rotate_z(Matrix4 mat, float deg) {
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix4 m = {
    .m = {{cos(rad), -sin(rad), 0, 0},
	  {sin(rad),  cos(rad), 0, 0},
	  {0,          0,         1, 0},
	  {0,          0,         0, 1}}
  };
  return Mat4_mul(mat, m);
}


// TODO: actually try to understand what is going on, this is from a pseudocode i found online
Matrix4 Mat4_mul(Matrix4 m1, Matrix4 m2) {
  Matrix4 res = {0};

  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      for (size_t k = 0; k < 4; ++k) {
	res.m[i][j] += m1.m[i][k] * m2.m[k][j];
      }
    }
  }

  return res;
}

Matrix4 Mat4_transpose(Matrix4 mat) {
  Matrix4 res = {0};
  for (size_t row = 0; row < 4; ++row) {
    for (size_t col = 0; col < 4; ++col) {
      res.m[col][row] = mat.m[row][col];
    }
  }

  return res;
}

Matrix4 Mat4_screen_to_clip_projection(Vector2f size) {
  assert(size.x > 0.f && size.y > 0.f);
  Matrix4 m = {
    .m = {{2.f/size.x, 0,          0, -1},
	  {0,          2.f/size.y, 0, -1},
	  {0,          0,          1,  0},
	  {0,          0,          0,  1}}
  };
  return m;
}

Vector4f Mat4_translate_vector(Vector4f v, Vector3f by){
  Vector4f r = {0};

  Matrix4 translation_mat = {
    .m = {{1.f, 0.f, 0.f, by.x},
	  {0.f, 1.f, 0.f, by.y},
	  {0.f, 0.f, 1.f, by.z},
	  {0.f, 0.f, 0.f, 1.f}}
  };

  return Mat4_vector_mul(translation_mat, v);
}

Vector4f Mat4_scale_vector(Vector4f v, Vector3f by){
  Vector4f r = {0};

  Matrix4 translation_mat = {
    .m = {{by.x, 0.f, 0.f, 0.f},
	  {0.f, by.y, 0.f, 0.f},
	  {0.f, 0.f, by.z, 0.f},
	  {0.f, 0.f, 0.f, 1.f}}
  };

  return Mat4_vector_mul(translation_mat, v);
}

Vector4f Mat4_rotate_x_vector(Vector4f v, float deg){
  float rad = deg2rad(deg);

  Matrix3 m = {
    .m = {{1, 0,          0        },
	  {0, cos(rad), -sin(rad)},
          {0, sin(rad),  cos(rad)}}
  };

  return Mat3_vector_mul(m, v);
}

Vector4f Mat4_rotate_y_vector(Vector4f v, float deg){
  float rad = deg2rad(deg);

  Matrix3 m = {
    .m = {{cos(rad),  0, sin(rad)},
	  {0,          1, 0        },
	  {-sin(rad), 0, cos(rad)}}
  };
  return Mat3_vector_mul(m, v);
}

Vector4f Mat4_rotate_z_vector(Vector4f v, float deg){
  Vector4f r = {0};
  float rad = deg2rad(deg);

  Matrix3 m = {
    .m = {{cos(rad), -sin(rad), 0},
	  {sin(rad),  cos(rad), 0},
	  {0,          0,         1}}
  };
  return Mat3_vector_mul(m, v);
}

Vector4f Mat4_vector_mul(Matrix4 m, Vector4f v){
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

Vector4f Mat3_vector_mul(Matrix3 m, Vector4f v){
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
