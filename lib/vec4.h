// vec4.h

#ifndef VEC4_H
#define VEC4_H

#include <cmath>

struct vec4
{
  float x, y, z, w;
  vec4(float x, float y, float z, float w):x(x),y(y),z(z),w(w){}
  vec4():x(0.0f),y(0.0f),z(0.0f),w(0.0f){}
  vec4 operator+(const vec4&v)const{return vec4(x+v.x, y+v.y, z+v.z, w+v.w);}
  vec4 operator-(const vec4&v)const{return vec4(x-v.x, y-v.y, z-v.z, w-v.w);}
  vec4 operator*(const vec4&v)const{return vec4(x*v.x, y*v.y, z*v.z, w*v.w);}
  vec4 operator/(const vec4&v)const{return vec4(x/v.x, y/v.y, z/v.z, w/v.w);}

  vec4& operator+=(const vec4&v){x+=v.x;y+=v.y;z+=v.z;w+=v.w;return*this;}
  vec4& operator-=(const vec4&v){x-=v.x;y-=v.y;z-=v.z;w-=v.w;return*this;}
  vec4& operator*=(const vec4&v){x*=v.x;y*=v.y;z*=v.z;w*=v.w;return*this;}
  vec4& operator/=(const vec4&v){x/=v.x;y/=v.y;z/=v.z;w/=v.w;return*this;}

  vec4 operator-()const{return vec4(-x,-y,-z,-w);}
  vec4 operator*(float f)const{return vec4(x*f,y*f,z*f,w*f);}
  vec4 operator/(float f)const{return vec4(x/f,y/f,z/f,w*f);}

  vec4& operator*=(float f){x*=f;y*=f;z*=f;w*=f;return*this;}
  vec4& operator/=(float f){x/=f;y/=f;z/=f;w/=f;return*this;}

  float dot(const vec4&v)const{return x*v.x+y*v.y+z*v.z+w*v.w;}
  float length()const{if (dot(*this)<0.001)return 1;return std::sqrt(dot(*this));}
  vec4& normalize(){float l=length();x/=l;y/=l;z/=l;w/=l;return*this;}
};
static inline vec4 operator*(float s, const vec4& v){vec4 r(v); r *= s; return r;}
static inline vec4 abs(const vec4& v){return vec4(fabsf(v.x),fabsf(v.y),fabsf(v.z),fabsf(v.w));}
static inline float dot(const vec4 &a, const vec4 &b){return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;}

#endif
