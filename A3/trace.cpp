#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "global.h"
#include "sphere.hpp"

//==================================================
// Global variables
//==================================================
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB background_clr;
extern RGB null_clr;

extern std::vector<Sphere *> scene;

// light 1 position and color
extern Point light_source;
extern Vector light_ambient;
extern Vector light_diffuse;
extern Vector light_specular;

// global ambient term
extern Vector global_ambient;

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern bool shadow_on;
extern bool reflection_on;
extern bool refraction_on;
extern bool checkboard_on;
extern bool diffuse_reflection_on;
extern bool antialiasing_on;

extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Shadow test for phong model
 *********************************************************************/

bool shadow_test(const Point &rayOrigin, const Vector &rayDirect, const Sphere * psh)
{
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    if( psh == scene[i] ) continue;

    float hit0 = INFINITY,  hit1 = INFINITY;
    // maximum ray range
    float tmax = 1000.0;
    if (scene[i]->intersect(rayOrigin, rayDirect, tmax, &hit0, &hit1)) {
      if(hit1 < 0 && hit0 < 0) continue;
      
      return false;
    }
  }
  return true;
}

/*********************************************************************
 * Phong local illumination + shadow ray generation
 *********************************************************************/
RGB phong(const Point &hitPoint, const Vector &view, const Vector &hitNormal, const Sphere * sphere) 
{
  // first calculate globla/local ambient term for the sphere 
	RGB color = global_ambient * sphere->mat_ambient + light_ambient * sphere->mat_ambient;

  // assume that we have a point light source and shot the shadow ray
  Vector shadow_ray = light_source - hitPoint; 
  
  // Normalize it shadow ray to get its direction L
  Vector L = (light_source - hitPoint).normalize();

  if( (!shadow_on) || shadow_test(hitPoint, shadow_ray, sphere) ){

    float d_L2 = shadow_ray.dot(shadow_ray);
    float d_L = std::sqrt(d_L2);
    float light_att = 1 / (decay_a + decay_b * d_L + decay_c * d_L2);

    // second, plus diffuse reflectance for the object
    color += light_diffuse * sphere->mat_diffuse * std::max(hitNormal.dot(L), (float)0.0) * light_att;

    // compute vector R from the L and normal
    Vector R = hitNormal * 2 * (hitNormal.dot(L)) - L;
    
    // third, plus specular reflectance for the object
    RGB specular = light_specular * sphere->mat_specular * std::max( (float)std::pow(R.dot(view), sphere->mat_shineness), (float)0.0) * light_att;
    
    color += specular;

  }

  return color;
}

/*********************************************************************
 * Scene intersection
 *********************************************************************/
float intersect_scene(const Point &rayOrigin, const Vector &rayDirect, Sphere * &pSphere)
{
  float tHit = INFINITY;
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    float hit0 = INFINITY,  hit1 = INFINITY;
    // maximum ray range
    float tmax = 1000.0;
    if (scene[i]->intersect(rayOrigin, rayDirect, tmax, &hit0, &hit1)) {
      // if hit0 < 0, means that rayOrigin inside sphere, should use hit1
      // cout << "Intersected point: " << hit0 << ", " << hit1 << endl;
      if (hit0 < 0 && hit1 > 0) {
        hit0 = hit1;
      }
      else if( hit0 < 0 && hit1 < 0) { 
        // if both negative roots, means the intersection is not valid
        continue; 
      }

      if (hit0 < tHit) {
        // if current sphere have a more near hit, use it 
        tHit = hit0;
        pSphere = scene[i];
      }
    }
  }

  return tHit;
}

/************************************************************************
 * This is the recursive ray tracer ************************************************************************/
RGB recursive_ray_trace(const Point &rayOrigin, const Vector &rayDirect, const int &depth) {
  
  Sphere * pSphere = NULL;

  // find the nearest ray-sphere intersection point
  float tHit = intersect_scene(rayOrigin, rayDirect, pSphere);

  // if there is no intersection found, return background color
  if (!pSphere){
    return background_clr;
  }

  bool inSphere = false;
  // find intersection point and its corresponding surface normal 
  RGB color;
  Point     hitPoint  = rayOrigin + rayDirect * tHit;
  Vector    hitNormal = pSphere->normal(hitPoint);

  // if normal and ray rayDirect on the same side, means rayOrigin inside sphere
  // so we should reverse the normal rayDirect
  if (hitNormal.dot(rayDirect) > 0)
  {
    hitNormal = - hitNormal;
    inSphere = true;
  }

  // Recursive ray tracing for reflection
  if ( reflection_on && ( depth < step_max ) )
  {

  }
  else {
    // when reflection is turn off, just use phong model to calculate
    // the local reflectance at current point

    // view = - rayDirect
    Vector view = - rayDirect;
    color = phong(hitPoint, view, hitNormal, pSphere);



  }
	
	return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/

void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start     = -0.5 * image_width;
  float y_start     = -0.5 * image_height;
  RGB   ret_color;
  Point       cur_pixel_pos;
  Vector      ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i < win_height; i++) {
    for (j=0; j < win_width; j++) {
      ray = (cur_pixel_pos - eye_pos).normalize();

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray, 0);

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

      // Checkboard for testing
      // RGB clr = RGB(float(i/32), 0, float(j/32));
      // ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y   += y_grid_size;
    cur_pixel_pos.x   = x_start;
  }
}
