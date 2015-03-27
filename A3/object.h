/**********************************************************************
 * Object class
 * An abstract class for polymorphism
 **********************************************************************/
#pragma once
class Object 
{
public:
  // material property used in Phong model
  Vector mat_ambient;
  Vector mat_diffuse;
  Vector mat_specular;
  float mat_shineness;

  float reflection;
  float transparency; 

  Object(const Vector &amb, const Vector &dif, const Vector &spe, 
    const float &shine, const float &refl, const float &transp):
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), reflection(refl), transparency(transp)
  {}

  virtual ~Object() {};

  virtual bool intersect(const Point &, const Vector &, const float tmax, float *) = 0;
  virtual Vector normal(const Point &) = 0;
};
