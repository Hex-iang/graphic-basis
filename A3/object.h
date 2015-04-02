/**********************************************************************
 * Object class
 * An abstract class for polymorphism
 **********************************************************************/
#pragma once
#include "ray.hpp"
#include "vector.hpp"
#include "matrix.hpp"

class Object 
{
public:
  // material property used in Phong model
  Object(){}

  virtual ~Object() {}

  virtual bool intersect(const Ray &, Intersection &) = 0;
  virtual Vector normal(const Point &)      const = 0;


  // virtual RGB   ambient(const Point &)      const = 0;
  // virtual RGB   diffuse(const Point &)      const = 0;
  // virtual RGB   specular(const Point &)     const = 0;
  // virtual float shineness(const Point &)    const = 0;
  // virtual float reflection(const Point &)   const = 0;
  // virtual float transparency(const Point &) const = 0;
  // virtual float transmission(const Point &) const = 0;
  // getter function for derived class variable 
  virtual RGB   ambient(const Point &q)      const { return RGB();           }
  virtual RGB   diffuse(const Point &q)      const { return RGB();           }
  virtual RGB   specular(const Point &q)     const { return RGB();           }
  virtual float shineness(const Point &q)    const { return float(0.0);      }
  virtual float reflection(const Point &q)   const { return float(0.0);      }
  virtual float transparency(const Point &q) const { return float(0.0);      }
  virtual float transmission(const Point &q) const { return float(0.0);      }
};
