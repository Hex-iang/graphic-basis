/**********************************************************************
 * Light class
 **********************************************************************/
#pragma once 
#include "vector.hpp"

class Light
{
public:
  Point   source;
  RGB     ambient;
  RGB     diffuse;
  RGB     specular;
  float   decay_a;
  float   decay_b;
  float   decay_c;

  Light(){};

  Light(const Point & ctr, const RGB & light_ambient, const RGB & light_diffuse, const RGB & light_specular, const float & a, const float & b, const float & c):
    source(ctr), ambient(light_ambient), diffuse(light_diffuse), 
    specular(light_specular), decay_a(a), decay_b(b), decay_c(c)
  {}

  float attenuation(const float &d_L, const float &d_L2)
  {
    return 1 / (decay_a + decay_b * d_L + decay_c * d_L2);
  }
};
