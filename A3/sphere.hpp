/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include <vector>
using namespace std;

class Sphere{
public: 
  Point center;
  float radius, radius2;

  float mat_ambient[3];    // material property used in Phong model
  float mat_diffuse[3];
  float mat_specular[3];
  float mat_shineness;

  float reflectance;       // Quantity of reflected light constribution to pixel
  float transparency;      // Quantity of transparency level of sphere


  // *********************************************************************
  // Sphere constructor
  // *********************************************************************
  Sphere(const Point &ctr, const float &rad, const float amb[], const float dif[], 
    const float spe[], const float &shine, const float &refl, const float &transp) : 
    center(ctr), radius(rad), radius2(rad * rad),mat_shineness(shine),
    reflectance(refl), transparency(transp)
  {
    (this->mat_ambient)[0]  = amb[0];
    (this->mat_ambient)[1]  = amb[1];
    (this->mat_ambient)[2]  = amb[2];
    (this->mat_diffuse)[0]  = dif[0];
    (this->mat_diffuse)[1]  = dif[1];
    (this->mat_diffuse)[2]  = dif[2];
    (this->mat_specular)[0] = spe[0];
    (this->mat_specular)[1] = spe[1];
    (this->mat_specular)[2] = spe[2];
  }

  // *********************************************************************
  // Function for calculate sphere normal
  // *********************************************************************
  Vector normal(const Point &q) {
    return (this->center - q).normalize();
  }

  // *********************************************************************
  // Function for finding ray-sphere intersection point 
  // *********************************************************************
  bool intersect(const Point &rayorig, const Vector &raydir, float *hit0 = NULL, float *hit1 = NULL) const
  {
    Vector l = this->center - rayorig;
    float tca = l.dot(raydir);
    
    // if the angle between sp
    if (tca < 0) return false;
    float d2 = l.dot(l) - tca * tca;
    
    if (d2 > this->radius2) return false;
    float thc = sqrt(radius2 - d2);
    
    // find near hit and far hit
    if (hit0 != NULL && hit1 != NULL) {
      *hit0 = tca - thc;
      *hit1 = tca + thc;
    }

    return true;
  }

};