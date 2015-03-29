/**********************************************************************
 * Chess Board class - always lay on the ground
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include "object.h"
#include <vector>
#include <iostream>
using namespace std;

class ChessBoard : public Object
{
  RGB light_ambient;
  RGB dark_ambient;
  RGB mat_diffuse;
  RGB mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency; 

  // Since chess board is infinite and lays on the x-z plane, 
  // we only need its y value to determinate its location
  float plane_y;

  // Grid width variable for determinating grid size
  float grid_wid;

public:
  ChessBoard(const RGB &light, const RGB &dark, const float y = 0.0, const float wid = 1.0): 
  light_ambient(light), dark_ambient(dark), plane_y(y), grid_wid(wid)
  {
  }
  ~ChessBoard(){}

  Vector normal(const Point & q) { return Vector(0.0, 1.0, 0.0); }
  
  bool intersect(const Point &origin, const Vector &direction, const float tmax, float *hit = NULL)
  {
    // if there is no y component, then there is no intersection
    if (direction.y == 0) return false;

    float t = - (origin.y + plane_y) /  direction.y;

    // if the ray is out of the maximum range, return false
    if ( t > tmax) return false;
    // else if ray is negative, return false
    else if ( t < 0 ) return false;
    // else the ray hits the plane, return true
    else *hit = t;

    return true;
  }

  // *********************************************************************
  // Function for returning material property
  // *********************************************************************
  RGB ambient(const Point &q) const { return dark_ambient; }
  RGB diffuse(const Point &q) const { return mat_diffuse; }
  RGB specular(const Point &q) const { return mat_specular; }
  float shineness(const Point &q) const { return mat_shineness; }
  float reflection(const Point &q) const { return mat_reflection; }
  float transparency(const Point &q) const { return mat_transparency; }

};
