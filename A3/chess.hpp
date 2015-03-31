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
  RGB   mat_ambient;
  RGB   mat_diffuse;
  RGB   mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency; 
  float mat_transmission;
public:
	Point  p1, p2, p3;
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

  bool intersect(const Ray &ray, Intersection & insect)
  {

  	// first, test if the ray direction is parallel to the surface
  	float divider = ray.direction.dot(mat_normal);
  	if( divider == 0) return false;
  	
  	// calculate intersection point t to the plane
  	float t = ( p1.dot(mat_normal) - ray.origin.dot(mat_normal) ) / divider;
  
  	// test if the intersection point is outside the range 
  	if ( t > ray.tmax) 	{ return false; }
  	else if( t < 0) { return false; }
  	// test if interestion point is inside triangle
  	Point p = ray.intersectPoint(t);

  	bool inside = ( (p - p1).dot( (p2 - p1).cross(mat_normal) ) >= 0 ) && 
  					 			( (p - p2).dot( (p3 - p2).cross(mat_normal) ) >= 0 ) &&
  					 			( (p - p3).dot( (p1 - p3).cross(mat_normal) ) >= 0 );

  	if ( inside ) insect.t = t;

    insect.point  = p;
    insect.normal = this->normal(p);

  	return inside;

  }
  
  Vector normal(const Point &q)            { return mat_normal;      }
  RGB 	ambient(const Point &q)      const { return mat_ambient;     }
  RGB 	diffuse(const Point &q)      const { return mat_diffuse;     }
  RGB 	specular(const Point &q)     const { return mat_specular;    }
  float shineness(const Point &q)    const { return mat_shineness;   }
  float reflection(const Point &q)   const { return mat_reflection;  }
  float transparency(const Point &q) const { return mat_transparency;}
  float transmission(const Point &q) const { return mat_transmission;}
};

#define DEBUG
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

  Chess(const RGB &amb, const RGB &dif, const RGB &spe, 
    const float &shine, const float &refl, const float &transp, 
    const float &transm, const string &str) : 
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
    mat_transmission(transm)
  {
  	vector<Point > vertices;
    int vertices_cnt = 0, triangles_cnt = 0, line_cnt =0;

  	std::ifstream infile(str.c_str());
  	std::cout << "reading file:" << str << "..." << std::endl;
  	std::string line;
  	while (std::getline(infile, line)){
      line_cnt++;

  		std::istringstream buffer(line);
  		char type;
  		// read the first character and see which type of line it is
  		buffer >> type;
  		
  		if(type == '#') continue;

  		if(type == 'v')
  		{
        vertices_cnt ++;
  			float x, y, z;
  			buffer >> x >> y >> z;
        Point p(x, y, z);

  			vertices.push_back(p);
  		}
  		else if( type == 'f')
  		{
        triangles_cnt++;
  			int i, j, k;
  			buffer >> i >> j >> k;
  			primitives.push_back( new Triangle(amb, dif, spe, shine, refl, transp, transm, vertices[i], vertices[j], vertices[k]));
  		}
  		
  	}

#ifdef DEBUG
      std::cout << "[naive_load] Read file complete: " << line_cnt << " lines in total" << std::endl;

      std::cout << "total points number: "    << vertices_cnt << endl;
      std::cout << "total triangles number: " << triangles_cnt << endl;
#endif
  }

  ~Chess() {
  	for (unsigned int i = 0; i < primitives.size(); ++i)
  	{
  		Object * pTriangle = primitives.back();
  		primitives.pop_back();
  		delete pTriangle;
  		pTriangle = NULL;
  	}
  }

  bool intersect(const Ray &ray, Intersection & insect)
  {
    Intersection tHit(INFINITY);

		for (unsigned int i = 0; i < primitives.size(); ++i)
		{
      Intersection tmpHit(INFINITY);
			if( primitives[i]->intersect(ray, tmpHit) )
			{
        if( tmpHit.t < tHit.t )
        {
          tHit = tmpHit;
        }
			}
		}
    
    if( tHit.t == INFINITY)
  	  return false;
    else
    {
      insect = tHit;
      return true;
    }
  }
  
  Vector normal(const Point &q) 
  {
    std::cout << "there should be something wrong" << std::endl;
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
