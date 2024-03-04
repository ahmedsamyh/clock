#include <clock/clock_matrix.h>

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
