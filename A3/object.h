/**********************************************************************
 * Object class
 * An abstract class for polymorphism
 **********************************************************************/
#pragma once
#include "ray.hpp"

class Object 
{
public:
  // material property used in Phong model

  Object(){}

  virtual ~Object() {}

  virtual bool intersect(const Ray &, float *) = 0;
  virtual Vector normal(const Point &) = 0;

  // // getter function for derived class variable 
  virtual RGB   ambient(const Point &)      const = 0;
  virtual RGB   diffuse(const Point &)      const = 0;
  virtual RGB   specular(const Point &)     const = 0;
  virtual float shineness(const Point &)    const = 0;
  virtual float reflection(const Point &)   const = 0;
  virtual float transparency(const Point &) const = 0;
  virtual float transmission(const Point &) const = 0;
};
