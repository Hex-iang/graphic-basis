/**********************************************************************
 * Ray class
 **********************************************************************/
 #pragma once
 #include "vector.hpp"

class Ray
{
public:
  Point origin;
  Vector direction;
  float tmax;
  unsigned triangleId;  

  Vector invdir;        /// precomputed for ray-box intersection
  int sign[3];          /// precomputed for ray-box intersection

  Ray(const Vector &orig, const Vector &dir, const float far = float(10000.0) ):
  origin(orig), direction(dir), tmax(far)
  {
    invdir = float(1) / dir;
    sign[0] = (invdir.x < 0);
    sign[1] = (invdir.y < 0);
    sign[2] = (invdir.z < 0);
  }

  Point intersecPoint(const float &t) const
  { 
    return origin + direction * t; 
  }
};

