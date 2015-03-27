/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include <vector>
#include <iostream>
using namespace std;

class Sphere{
public: 
  Point center;
  float radius, radius2;

  Vector mat_ambient;    // material property used in Phong model
  Vector mat_diffuse;
  Vector mat_specular;
  float mat_shineness;

  float reflection;       // Quantity of reflected light constribution to pixel
  float transparency;      // Quantity of transparency level of sphere


  // *********************************************************************
  // Sphere constructor
  // *********************************************************************
  Sphere(const Point &ctr, const float &rad, const Vector &amb, 
    const Vector &dif, const Vector &spe, const float &shine, 
    const float &refl, const float &transp) : 
    center(ctr), radius(rad), radius2(rad * rad),
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), reflection(refl), transparency(transp)
  {}

  // *********************************************************************
  // Function for calculate sphere normal
  // *********************************************************************
  Vector normal(const Point &q) {
    return (this->center - q).normalize();
  }

  // *********************************************************************
  // Quadratic function solver 
  // *********************************************************************
  static bool solveQuadraticEquation(const float &a, const float &b, const float &c, float *x0, float *x1)
  {
    // discriminant for the equation
    float discr = b * b - 4 * a * c;
    if (discr < 0){
      // no solution to this case
      return false;
    }
    else if (discr == 0) {
      // single solution
      *x0 = *x1 = - 0.5 * b / a;
    }
    else {
      // dual solution
      // notice that the original root equation is replaced for more stable
      // floating point computation
      float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
      *x0 = q / a;
      *x1 = c / q;
    }
    if (*x0 > *x1) std::swap(*x0, *x1);
    return true;
  }


  // *********************************************************************
  // Function for finding ray-sphere intersection point 
  // *********************************************************************
  bool intersect(const Point &origin, const Vector &direction, const float tmax, float *hit0 = NULL, float *hit1 = NULL) const
  {

    Vector L = origin - center;
    float a = direction.dot(direction);
    float b = 2 * direction.dot(L);
    float c = L.dot(L) - radius2;
    
    // if there is no solution to the qudratic solver, return false
    if (!solveQuadraticEquation(a, b, c, hit0, hit1)) 
      return false;

    // if the nearest hit point is also outside the ray range, return false
    if (*hit0 > tmax)
      return false;

    return true;
  }

};
