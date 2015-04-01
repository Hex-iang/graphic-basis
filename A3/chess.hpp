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

  // chess piece position property 
public:
	vector<Object *> primitives;
  vector<Triangle> box;

  Chess(const RGB &amb, const RGB &dif, const RGB &spe, 
    const float &shine, const float &refl, const float &transp, 
    const float &transm, const string &str, 
    const Vector displacement = Vector(0.0, 0.0, 0.0), const float scale = 1.0) : 
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
    mat_transmission(transm)
  {
  	vector<Point > vertices;
    int vertices_cnt = 0, triangles_cnt = 0, line_cnt =0;

    float xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
    float xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

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
        Point p = scale * Point(x, y, z) + displacement;

        // std::cout << p << std::endl;

        if(xmin > p.x) xmin = p.x - 0.1;
        if(ymin > p.y) ymin = p.y - 0.1;
        if(zmin > p.z) zmin = p.z - 0.1;
        if(xmax < p.x) xmax = p.x + 0.1;
        if(ymax < p.y) ymax = p.y + 0.1;
        if(zmax < p.z) zmax = p.z + 0.1;

        // std::cout << p << std::endl;
  			vertices.push_back(p);
  		}
  		else if( type == 'f')
  		{
        triangles_cnt++;
  			int i, j, k;
  			buffer >> i >> j >> k;

        // std::cout << i  << "," << j << "," << k << std::endl; 
  			
        primitives.push_back( new Triangle(amb, dif, spe, shine, refl, transp, transm, vertices[i], vertices[j], vertices[k]));
  		}
  	}

    // // points for the 
    Point p1(xmin, ymin, zmin); Point p2(xmin, ymin, zmax);
    Point p3(xmin, ymax, zmin); Point p4(xmin, ymax, zmax);
    Point p5(xmax, ymin, zmin); Point p6(xmax, ymin, zmax);
    Point p7(xmax, ymax, zmin); Point p8(xmax, ymax, zmax);

    std::cout << "diagnoal points:" << p1 << p8 << std::endl;

    // optimization 1
    box.push_back(Triangle(p1, p2, p4));
    box.push_back(Triangle(p1, p3, p4));
    box.push_back(Triangle(p1, p5, p7));
    box.push_back(Triangle(p1, p7, p3));
    box.push_back(Triangle(p1, p2, p6));
    box.push_back(Triangle(p1, p5, p6));
    box.push_back(Triangle(p2, p6, p8));
    box.push_back(Triangle(p2, p4, p8));
    box.push_back(Triangle(p3, p4, p8));
    box.push_back(Triangle(p3, p7, p8));
    box.push_back(Triangle(p5, p6, p7));
    box.push_back(Triangle(p6, p7, p8));


      std::cout << "[naive_load] Read file complete: " << line_cnt << " lines in total" << std::endl;
      std::cout << "total points number: "    << vertices_cnt << endl;
      std::cout << "total triangles number: " << triangles_cnt << endl;
      std::cout << "vertices number: " << vertices.size() << endl;
      std::cout << "primitives number: " << primitives.size() << endl;

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
    // bool inside = true;
    // Intersection tHit(INFINITY);

    bool inside = false;
    Intersection tHit(INFINITY);
    // first optimization: a bounding box of the object
    for( unsigned int i = 0; i < box.size(); ++i)
    {
      Intersection tmpHit(INFINITY);
      if( box[i].intersect(ray, tmpHit) )
      {
        inside = true;
      }
    }

    if( inside ){
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
      
      if( tHit.t > ray.tmax)
        return false;
      else
      {
        insect = tHit;
        return true;
      }
    }
    else
    {
      return false;
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
