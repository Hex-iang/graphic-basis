/**********************************************************************
 * Triangle class and Simple Triangle Mesh
 **********************************************************************/
#pragma once
#include "object.h"
#include "global.h"
#include <vector>

class Triangle: public Object
{
public:
	Point  p1, p2, p3;
	Vector mat_normal;
	Triangle(const Point & _p1, const Point & _p2, const Point & _p3): p1(_p1), p2(_p2), p3(_p3)
	{
		mat_normal = ((p3 - p2).cross(p2 - p1) ).normalize();
	}

	~Triangle(){}

  Point get_min_bound(){
    return Point( std::min(p1.x, std::min(p2.x , p3.x)) ,
                  std::min(p1.y, std::min(p2.y , p3.y)) ,
                  std::min(p1.z, std::min(p2.z , p3.z)) );
  }
  Point get_max_bound(){
    return Point( std::max(p1.x, std::max(p2.x , p3.x)) ,
                  std::max(p1.y, std::max(p2.y , p3.y)) ,
                  std::max(p1.z, std::max(p2.z , p3.z)) );
  }

 // traditional triangle intersection

#define MOLLER_TRUMBORE
#ifdef MOLLER_TRUMBORE
  bool intersect(const Ray &ray, Intersection & insect)
  {
    // increase the intersection test counter
    Statistic::intersect_test_cnt ++;

  	// a fast MOLLER_TRUMBORE method for finding triangle intersection point
		Vector edge1 = p2 - p1;
		Vector edge2 = p3 - p1;

		Vector pvec = (ray.direction).cross(edge2);

		float det = edge1.dot(pvec);


		if (det > -EPSILON && det < EPSILON) return false;

		float invDet = 1 / det;

		Vector tvec = ray.origin - p1;
		float u = tvec.dot(pvec) * invDet;

		if (u < 0 || u > 1) return false;

		Vector qvec = tvec.cross(edge1);
		float v = ray.direction.dot(qvec) * invDet;

		if (v < 0 || u + v > 1) return false;

		insect.t      = edge2.dot(qvec) * invDet;
		insect.point  = ray.intersectPoint(insect.t);
		insect.normal = this->normal(insect.point);

    // increase the intersection counter
    Statistic::intersect_cnt++;

		return true;
  }
#else
  bool intersect(const Ray &ray, Intersection & insect)
  {

   // first, test if the ray direction is parallel to the surface
   float divider = ray.direction.dot(mat_normal);
   if( divider == 0) return false;

   // calculate intersection point t to the plane
   float t = ( p1.dot(mat_normal) - ray.origin.dot(mat_normal) ) / divider;

   // test if the intersection point is outside the range
   if ( t > ray.tmax)  { return false; }
   else if( t < ray.tmin) { return false; }
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
#endif

  Vector normal(const Point &q)      const { return mat_normal;      }
  RGB 	ambient(const Point &q)      const { return RGB();           }
  RGB 	diffuse(const Point &q)      const { return RGB();           }
  RGB 	specular(const Point &q)     const { return RGB();           }
  float shineness(const Point &q)    const { return float(0.0);      }
  float reflection(const Point &q)   const { return float(0.0);      }
  float transparency(const Point &q) const { return float(0.0);      }
  float transmission(const Point &q) const { return float(0.0);      }
};

// triangle mesh object
class Mesh: public Object
{
public:
  RGB   mat_ambient;
  RGB   mat_diffuse;
  RGB   mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency;
  float mat_transmission;
	float mat_diffuse_reflection;

  std::vector<Triangle> primitives;

	Mesh(const RGB &amb, const RGB &dif, const RGB &spe,
    const float &shine, const float &refl, const float &transp,
    const float &transm , const float &dif_refl):
	  mat_ambient(amb), mat_diffuse(dif), mat_specular(spe),
    mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
    mat_transmission(transm), mat_diffuse_reflection(dif_refl) {}

   ~Mesh(){}

  bool intersect(const Ray &ray, Intersection & insect)
  {
    Intersection tHit(INFINITY);

    for (unsigned int i = 0; i < primitives.size(); ++i)
    {
      Intersection tmpHit(INFINITY);
      if( primitives[i].intersect(ray, tmpHit) )
      {
        if( tmpHit.t < tHit.t )
        {
          tHit = tmpHit;
        }
      }
    }

    if( tHit.t > ray.tmax || tHit.t < ray.tmin)
      return false;
    else
    {
      insect = tHit;
      return true;
    }
  }

  void addTriangle(const Point &p1, const Point &p2, const Point &p3)
  {
  	primitives.push_back(Triangle(p1, p2, p3));
  }

  Vector normal(const Point &q) const
  {
    // std::cout << "there should be something wrong" << std::endl;
  	return Vector(0.0, 0.0, -2.0);
  }

  RGB 	ambient(const Point &q)      const { return mat_ambient;     }
  RGB 	diffuse(const Point &q)      const { return mat_diffuse;     }
  RGB 	specular(const Point &q)     const { return mat_specular;    }
  float shineness(const Point &q)    const { return mat_shineness;   }
  float reflection(const Point &q)   const { return mat_reflection;  }
  float transparency(const Point &q) const { return mat_transparency;}
  float transmission(const Point &q) const { return mat_transmission;}

};
