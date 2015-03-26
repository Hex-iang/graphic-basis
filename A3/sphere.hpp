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

  Vector mat_ambient;    // material property used in Phong model
  Vector mat_diffuse;
  Vector mat_specular;
  float mat_shineness;

  float reflectance;       // Quantity of reflected light constribution to pixel
  float transparency;      // Quantity of transparency level of sphere


  // *********************************************************************
  // Sphere constructor
  // *********************************************************************
  Sphere(const Point &ctr, const float &rad, const Vector &amb, 
    const Vector &dif, const Vector &spe, const float &shine, 
    const float &refl, const float &transp) : 
    center(ctr), radius(rad), radius2(rad * rad),
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), reflectance(refl), transparency(transp)
  {}

  // *********************************************************************
  // Function for calculate sphere normal
  // *********************************************************************
  Vector normal(const Point &q) {
    return (this->center - q).normalize();
  }

  // *********************************************************************
  // Function for finding ray-sphere intersection point 
  // *********************************************************************
  bool intersect(const Point &origin, const Vector &direction, float *hit0 = NULL, float *hit1 = NULL) const
  {
    // Geometric formula for the ray sphere interaction 

    // l is the vector from ray center to sphere center
    Vector l = this->center - origin;

    // t_ca is the projection of ray-sphere vector on ray's direction
    float t_ca = l.dot(direction);
    
    // The ray didn't shoot to the sphere direction if projection < 0
    if (t_ca < 0) return false;

    // Pythagorean theorem
    float d2 = l.dot(l) - t_ca * t_ca;
    
    // if the ray-center distance is larger, means there is no intersection
    if (d2 > this->radius2) return false;
    // Pythagorean theorem to 
    float t_hc = sqrt(radius2 - d2);
    
    // find near hit and far hit
    // if near hit < 0, means that eye is inside sphere, then use hit1; 
    // otherwise use hit0
    if (hit0 != NULL && hit1 != NULL) {
      *hit0 = t_ca - t_hc;
      *hit1 = t_ca + t_hc;
    }

    return true;
  }

};
