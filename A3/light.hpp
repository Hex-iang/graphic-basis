/**********************************************************************
 * Light class
 **********************************************************************/
#pragma once 
#include "vector.hpp"

class Light
{
public:
  Point   source;
  Vector  ambient;
  Vector  diffuse;
  Vector  specular;
  float   decay_a;
  float   decay_b;
  float   decay_c;

  Light(){};

  Light(const Point & ctr, const Vector & light_ambient, const Vector & light_diffuse, const Vector & light_specular, const float & a, const float & b, const float & c):
    source(ctr), ambient(light_ambient), diffuse(light_diffuse), 
    specular(light_specular), decay_a(a), decay_b(b), decay_c(c)
  {}

  float attenuation(const float &d_L, const float &d_L2)
  {
    return 1 / (decay_a + decay_b * d_L + decay_c * d_L2);
  }
};
