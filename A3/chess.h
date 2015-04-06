/**********************************************************************
 * Chess class
 **********************************************************************/
#pragma once
#include "object.h"
#include "triangle.hpp"
#include "matrix.hpp"
#include "box.hpp"
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

class Triangle;
class Box;

// a structure for accerlerate chess object intersection
struct Cell
{
  Cell()  {}
  ~Cell() {}
  std::vector<Triangle> triangles;

  void insert( const Triangle & t)
  {
    triangles.push_back(t);
  }

  bool intersect(const Ray &ray, Intersection & insect);

};

class Chess: public Object
{
  RGB mat_ambient;
  RGB mat_diffuse;
  RGB mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency;
  float mat_transmission;

  float mat_diffuse_reflection;

  // Optimization type
  // 0 - no optimization the trianglated object
  // 1 - optimized with a single bounding volume
  // 2 - optimized with grid algorithm
  int optimize;

  // chess piece position property
public:
	std::vector<Triangle> primitives;
  // a binding volume for first level optimization
  Box box;

  // structures for grid optimization
  std::map<uint32_t, Cell> cells;
  Vector cellDimension;
  uint32_t resolution[3];

  Chess(const RGB &amb, const RGB &dif, const RGB &spe,
    const float &shine, const float &refl, const float &transp,
    const float &transm, const float &dif_refl, const std::string &str,
    const Vector displacement = Vector(0.0, 0.0, 0.0),
    const float s = 1.0, int opt = 1);

  ~Chess() { }

  bool intersect(const Ray &ray, Intersection & insect);


  Vector normal(const Point &q) const      { return Vector(0.0, 1.0, 0.0); }

  // getter function for derived class variable
  RGB 	ambient(const Point &q)      const { return mat_ambient;     }
  RGB 	diffuse(const Point &q)      const { return mat_diffuse;     }
  RGB 	specular(const Point &q)     const { return mat_specular;    }
  float shineness(const Point &q)    const { return mat_shineness;   }
  float reflection(const Point &q)   const { return mat_reflection;  }
  float transparency(const Point &q) const { return mat_transparency;}
  float transmission(const Point &q) const { return mat_transmission;}
  float diffuse_reflection(const Point &q) const { return mat_diffuse_reflection; }
};

template<typename T> inline T clamp(const T &v, const T &lo, const T &hi)
{ return std::max(lo, std::min(v, hi)); }
