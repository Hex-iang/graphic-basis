/**********************************************************************
 * Chess class
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include "object.h"
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Triangle: public Object
{
  RGB mat_ambient;
  RGB mat_diffuse;
  RGB mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency; 
  float mat_transmission;
public:
	Point p1, p2, p3;
	Vector mat_normal;
	Triangle(const Point & _p1, const Point & _p2, const Point & _p3): p1(_p1), p2(_p2), p3(_p3) 
	{ 
		mat_normal = ((p3 - p2).cross(p2 - p1) ).normalize(); 
	}

	Triangle(const RGB &amb, const RGB &dif, const RGB &spe, 
    const float &shine, const float &refl, const float &transp, 
    const float &transm, const Point & _p1, const Point & _p2, const Point & _p3 ): 
	  mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
    mat_transmission(transm), p1(_p1), p2(_p2), p3(_p3) 
  {
		mat_normal = ((p3 - p2).cross(p2 - p1) ).normalize();
	}

	~Triangle(){}

  bool intersect(const Point &origin, const Vector &direction, const float tmax, float *hit = NULL, Object * pObject = NULL, bool * flag = NULL)
  {
  	// first, test if the ray direction is parallel to the surface
  	float divider = direction.dot(mat_normal);
  	if( divider == 0) return false;
  	
  	// calculate intersection point t to the plane
  	float t = ( p1.dot(mat_normal) - origin.dot(mat_normal) ) / divider;
  
  	// test if the intersection point is outside the range 
  	if ( t > tmax) 	{ return false; }
  	else if( t < 0) { return false; }
  	// test if interestion point is inside triangle
  	Point p = origin + direction * t;

  	bool inside = ( (p - p1).dot( (p2 - p1).cross(mat_normal) ) >= 0 ) && 
  					 			( (p - p2).dot( (p3 - p2).cross(mat_normal) ) >= 0 ) &&
  					 			( (p - p3).dot( (p1 - p3).cross(mat_normal) ) >= 0 );

  	if ( inside ) *hit = t;


  	return inside;

  }
  
  Vector normal(const Point &q) { return mat_normal; }
  RGB 	ambient(const Point &q)      const { return mat_ambient;     }
  RGB 	diffuse(const Point &q)      const { return mat_diffuse;     }
  RGB 	specular(const Point &q)     const { return mat_specular;    }
  float shineness(const Point &q)    const { return mat_shineness;   }
  float reflection(const Point &q)   const { return mat_reflection;  }
  float transparency(const Point &q) const { return mat_transparency;}
  float transmission(const Point &q) const { return mat_transmission;}
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
public:
	vector<Object *> primitives;
	vector<Object *> boundingboxes;

  Chess(const RGB &amb, const RGB &dif, const RGB &spe, 
    const float &shine, const float &refl, const float &transp, 
    const float &transm, const string &str) : 
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
    mat_transmission(transm)
  {
  	vector<Point > vertices;
  	float xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;
  	float xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;

  	std::ifstream infile(str.c_str());
  	std::cout << "reading file:" << str << "..." << std::endl;
  	std::string line;
  	while (std::getline(infile, line)){
  		std::istringstream buffer(line);
  		char type;
  		// read the first character and see which type of line it is
  		buffer >> type;
  		
  		if(type == '#') continue;

  		if(type == 'v')
  		{
  			float x, y, z;
  			buffer >> x >> y >> z;
  			z-= 3.0;
  			if( x > xmax) xmax = x;
  			if( x < xmin) xmin = x;
  			if( y > ymax) ymax = y;
  			if( y < ymin) ymin = y;
  			if( z > zmax) zmax = z;
  			if( z < zmin) zmin = z;

  			vertices.push_back(Point(x, y, z));
  		}
  		else if( type == 'f')
  		{
  			int i, j, k;
  			buffer >> i >> j >> k;
  			primitives.push_back( new Triangle(amb, dif, spe, shine, refl, transp, transm, vertices[i], vertices[j], vertices[k]));
  		}
  		
  	}

  	Point p[8] = {Point(xmin, ymin, zmin), Point(xmin, ymin, zmax), Point(xmin, ymax, zmin), Point(xmin, ymax, zmax),
  								Point(xmax, ymin, zmin), Point(xmax, ymin, zmax), Point(xmax, ymax, zmin), Point(xmax, ymax, zmax)};
  	// build up bounding box through maximum primitives
  	boundingboxes.push_back( new Triangle(p[0], p[1], p[2]));
  	boundingboxes.push_back( new Triangle(p[1], p[2], p[3]));
  	boundingboxes.push_back( new Triangle(p[0], p[2], p[4]));
  	boundingboxes.push_back( new Triangle(p[2], p[6], p[4]));
  	boundingboxes.push_back( new Triangle(p[0], p[1], p[5]));
  	boundingboxes.push_back( new Triangle(p[0], p[5], p[4]));
  	boundingboxes.push_back( new Triangle(p[4], p[7], p[6]));
  	boundingboxes.push_back( new Triangle(p[4], p[5], p[7]));
  	boundingboxes.push_back( new Triangle(p[2], p[3], p[7]));
  	boundingboxes.push_back( new Triangle(p[2], p[7], p[6]));
  	boundingboxes.push_back( new Triangle(p[1], p[3], p[7]));
  	boundingboxes.push_back( new Triangle(p[1], p[7], p[5]));

  	std::cout << "triangle number: " << primitives.size() << std::endl;

  }

  ~Chess() {
  	for (unsigned int i = 0; i < primitives.size(); ++i)
  	{
  		Object * pTriangle = primitives.back();
  		primitives.pop_back();
  		delete pTriangle;
  		pTriangle = NULL;
  	}

  	for (unsigned int i = 0; i < boundingboxes.size(); ++i)
  	{
  		Object * pTriangle = boundingboxes.back();
  		boundingboxes.pop_back();
  		delete pTriangle;
  		pTriangle = NULL;
  	}
  }

  bool intersect(const Point &origin, const Vector &direction, const float tmax, float *hit = NULL, Object * pObject = NULL, bool * flag = NULL)
  {
  	bool inside = false;
  	for (unsigned int i = 0; i < boundingboxes.size(); ++i)
  	{
  		if(boundingboxes[i]->intersect(origin, direction, tmax, hit, NULL, NULL)){
  			inside = true;
  			break;
  		}
  	}

  	if( inside )
  	{
  		for (unsigned int i = 0; i < primitives.size(); ++i)
  		{
  			if(primitives[i]->intersect(origin, direction, tmax, hit, NULL, NULL))
  			{
          if( flag == NULL){
            pObject = primitives[i];
            *flag = true; 
          }

  				return true;
  			}
  		}
  	}

  	return false;
  }
  
  Vector normal(const Point &q) 
  {
  	return Vector();
  }

  // getter function for derived class variable 
  RGB 	ambient(const Point &q)      const { return mat_ambient;     }
  RGB 	diffuse(const Point &q)      const { return mat_diffuse;     }
  RGB 	specular(const Point &q)     const { return mat_specular;    }
  float shineness(const Point &q)    const { return mat_shineness;   }
  float reflection(const Point &q)   const { return mat_reflection;  }
  float transparency(const Point &q) const { return mat_transparency;}
  float transmission(const Point &q) const { return mat_transmission;}
};