// matrix.h

#ifndef MATRIX_H
#define MATRIX_H

#include "vec3.h"
#include "vec4.h"

struct matrix
{
  float m[16];
  matrix();
  matrix(const float *s);
  matrix(const vec4 &v0, const vec4 &v1, const vec4 &v2, const vec4 &v3);
  vec4 operator*(const vec4 &v);
  vec3 operator*(const vec3 &v);
};

#endif
