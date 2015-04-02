/**********************************************************************
 * Ray class & Ray intersection class
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include "global.h" 
class Ray
{
public:
  Point origin;
  Vector direction;
  mutable float tmax;
  mutable float tmin;
  unsigned triangleId;  

  Vector invdir;        // precomputed value for ray-box intersection
  int sign[3];          // precomputed value for ray-box intersection

  Ray(const Vector &orig, const Vector &dir, const float far = float(1000.0), const float near = float(EPSILON)):
  origin(orig), direction(dir), tmax(far), tmin(near)
  {
    invdir = float(1) / dir;
    // sign structure is used for excluding the zero value case
    sign[0] = (invdir.x < 0);
    sign[1] = (invdir.y < 0);
    sign[2] = (invdir.z < 0);
  }

  Point intersectPoint(const float &t) const
  { 
    return origin + direction * t; 
  }

};

class Intersection
{
public:
  // intersection depth
  float t;
  // intersection normal
  Vector normal;
  Point point;

  Intersection(){};
  Intersection(const float & _t):t(_t){}
  Intersection(const Intersection &p) { 
    this->t       = p.t; 
    this->normal  = p.normal; 
    this->point   = p.point;
  }
  ~Intersection(){};

  friend std::ostream & operator << (std::ostream &os, const Intersection &v)
  {
    os << "Intersected t: " << v.t <<", Intersected normal:" << v.normal << ", Intersected point:" << v.point;
    return os;
  }
};
