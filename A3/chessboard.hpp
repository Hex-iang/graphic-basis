/**********************************************************************
 * Chess Board class - always lay on the ground
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include "object.h"
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

extern RGB null_clr;

class ChessBoard : public Object
{
  // chess board material property  
  RGB mat_ambient;
  RGB light_diffuse;
  RGB dark_diffuse;
  RGB mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency; 
  float mat_transmission;
public:

  // Since chess board is infinite and lays on the x-z plane, 
  // we only need its y value to determinate its location
  float plane_y;

  // Grid width variable for determinating grid size
  float grid_wid;

  ChessBoard(const RGB &ambient, const RGB &light_dif, const Vector &dark_dif, 
    const RGB &spe, const float &shine, const float &refl, 
    const float &transp, const float &transm, const float y = - 2.0, 
    const float wid = 1.0) : 
    mat_ambient(ambient), light_diffuse(light_dif), dark_diffuse(dark_dif),
    mat_specular(spe), mat_shineness(shine), mat_reflection(refl), 
    mat_transparency(transp), mat_transmission(transm), 
    plane_y(y), grid_wid(wid) {}

  ~ChessBoard(){}

  Vector normal(const Point & q) { return Vector(0.0, 1.0, 0.0); }
  
  bool intersect(const Ray &ray, float *hit = NULL)
  {
    // if there is no y component, then there is no intersection
    if (ray.direction.y == 0) return false;

    float t = (ray.origin.y + plane_y) / ray.direction.y;

    // if the ray is out of the maximum range, return false
    if ( t > ray.tmax) return false;
    // else if ray is negative, return false
    else if ( t < 0 ) return false;
    // else the ray hits the plane, return true
    else *hit = t;

    return true;
  }

  // *********************************************************************
  // Function for returning material property
  // *********************************************************************
  RGB ambient(const Point &q)         const { return mat_ambient; }
  RGB specular(const Point &q)        const { return mat_specular; }
  float shineness(const Point &q)     const { return mat_shineness; }
  float reflection(const Point &q)    const { return mat_reflection; }
  float transparency(const Point &q)  const { return mat_transparency; }
  float transmission(const Point &q)  const { return mat_transmission; }

  RGB diffuse(const Point &q) const 
  {
    // calculate point coordinates on chess board 
    int x = floor(q.x / grid_wid + 0.5);
    int y = floor(q.z / grid_wid + 0.5);

    // rescale chessboard coordinates and calculate its coresponding color
    if( y >= 0)
    {
      if( ( (int(abs(x)) % 2 ) == 0 && (int(abs(y)) % 2 ) == 0) || 
          ( (int(abs(x)) % 2 ) == 1 && (int(abs(y)) % 2 ) == 1) )
        return light_diffuse;
      else
        return dark_diffuse;
    }
    else
    {
      if( ( (int(abs(x)) % 2 ) == 0 && (int(abs(y)) % 2 ) == 0) || 
          ( (int(abs(x)) % 2 ) == 1 && (int(abs(y)) % 2 ) == 1) )
        return dark_diffuse;
      else
        return light_diffuse;
    }
  }

};
