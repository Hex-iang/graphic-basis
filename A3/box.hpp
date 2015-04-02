/**********************************************************************
 * Box class for bounding box volume 
 **********************************************************************/
#pragma once
#include "object.h"

class Box: public Object
{
public: 
	// we need two point to represent an aligned box
	Point bounds[2];
	// so the lower x plane can be represented as y = minBound.x, same that higher 
	// plane can be represented as y = maxBound.x. 
	// we can apply this technique to y and z component
	Box(){}
	Box(const Point& minb, const Point& maxb){
		bounds[0] = minb; bounds[1] = maxb;
	} 
	~Box(){}

	bool intersect(const Ray &ray, Intersection & insect)
	{
		// set min/max value for each axis
		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		// calculate the tmin, tmax value in the x planes
		tmin = (bounds[    ray.sign[0]].x - ray.origin.x) * ray.invdir.x;
		tmax = (bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.invdir.x;

		// calculate tmin, tmax in y planes
		tymin = (bounds[	ray.sign[1]].y - ray.origin.y) * ray.invdir.y;
		tymax = (bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.invdir.y;

		// exclude the case that ray is oudside box
		if ((tmin > tymax) || (tymin > tmax))
			return false;

		// use a larger tmin
		if (tymin > tmin)
			tmin = tymin;

		// use a smaller tmax
		if (tymax < tmax)
			tmax = tymax;
		
		// same thing as in the y direction
		tzmin = (bounds[	  ray.sign[2]].z - ray.origin.z) * ray.invdir.z;
		tzmax = (bounds[1	- ray.sign[2]].z - ray.origin.z) * ray.invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;
		
		// decrease the range that could be covered by the ray
		// because the ray is intersecting the box, so it will only intersect the
		// triangle inside box range
		if (tmin > ray.tnear) ray.tnear = tmin;
		if (tmax < ray.tfar) 	ray.tfar  = tmax;

		return true;
	}
  const Point & operator [](const int idx){ return bounds[idx]; }

};
