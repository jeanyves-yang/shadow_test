// vec2.h

#ifndef VEC2_H
#define VEC2_H

struct vec2
{
  float x, y;
  vec2(float x, float y):x(x),y(y){}
  vec2():x(0.0f),y(0.0f){}
};

#endif
