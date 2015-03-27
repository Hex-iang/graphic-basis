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
#include "light.hpp"
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

extern std::vector<Object *> scene;

// light 1
extern Light light;

// global ambient term
extern Vector global_ambient;

// light decay parameters

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

bool shadow_test(const Point &ray_origin, const Vector &ray_direct, const Object * pObject_ignore)
{
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    if( pObject_ignore == scene[i] ) continue;

    float hit = INFINITY;
    // maximum ray range
    float tmax = 1000.0;
    if (scene[i]->intersect(ray_origin, ray_direct, tmax, &hit)) {
      
      return false;
    }
  }
  return true;
}

/*********************************************************************
 * Scene intersection
 *********************************************************************/
float intersect_scene(const Point &ray_origin, const Vector &ray_direct, Object * &pObject, Object * pObject_ignore = NULL)
{
  float tHit = INFINITY;
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    if(pObject_ignore == scene[i]) continue;
    float hit = INFINITY;
    // maximum ray range
    float tmax = 1000.0;
    if (scene[i]->intersect(ray_origin, ray_direct, tmax, &hit)) {
      // skip the sphere we want to ignore
      if (hit < tHit) {
        // if current sphere have a more near hit, use it 
        tHit = hit;
        pObject = scene[i];
      }
    }
  }

  return tHit;
}


/*********************************************************************
 * Phong local illumination + shadow ray generation
 *********************************************************************/
RGB phong(const Point &hit_point, const Vector &view, const Vector &hit_normal, const Object * pObject) 
{
  // first calculate globla/local ambient term for the sphere 
	RGB color = global_ambient * pObject->mat_ambient + light.ambient * pObject->mat_ambient;

  // assume that we have a point light source and shot the shadow ray
  Vector shadow_ray = light.source - hit_point; 
  
  // Normalize it shadow ray to get its direction L
  Vector L = (light.source - hit_point).normalize();

  if( (!shadow_on) || shadow_test(hit_point, shadow_ray, pObject) ){

    float d_L2 = shadow_ray.dot(shadow_ray);
    float d_L = std::sqrt(d_L2);
    float light_att = light.attenuation(d_L, d_L2);

    // second, plus diffuse reflectance for the object
    color += light.diffuse * pObject->mat_diffuse * std::max(hit_normal.dot(L), (float)0.0) * light_att;

    // compute vector R from the L and normal
    Vector R = (hit_normal * 2 * (hit_normal.dot(L)) - L).normalize();
    
    // third, plus specular reflectance for the object
    RGB specular = light.specular * pObject->mat_specular * std::max( (float)std::pow(R.dot(view), pObject->mat_shineness), (float)0.0) * light_att;
    
    color += specular;

  }

  return color;
}

/************************************************************************
 * This is the recursive ray tracer ************************************************************************/
RGB recursive_ray_trace(const Point &ray_origin, const Vector &ray_direct, const int &depth, Object * pObject_ignore = NULL) {
  
  Object * pObject = NULL;

  // find the nearest ray-sphere intersection point
  float tHit = intersect_scene(ray_origin, ray_direct, pObject, pObject_ignore);

  // if there is no intersection found
  if (!pObject){
    if(depth > 0)
      return null_clr;
    else
      return background_clr;
  }

  bool inSphere = false;
  // find intersection point and its corresponding surface normal 
  RGB     color(0.0, 0.0, 0.0);
  Point   hit_point  = ray_origin + ray_direct * tHit;
  Vector  hit_normal = pObject->normal(hit_point);

  // if normal and ray ray_direct on the same side, means ray_origin inside sphere
  // so we should reverse the normal ray_direct
  if (hit_normal.dot(ray_direct) > 0)
  {
    hit_normal = - hit_normal;
    inSphere = true;
  }

  // Calculate local reflectance at current point

  // view = - ray_direct
  Vector view = - ray_direct;
  color = phong(hit_point, view, hit_normal, pObject);

  // Recursive ray tracing for reflection
  if ( reflection_on && ( pObject->reflection > 0 ) && ( depth < step_max ) )
  {
    // for object with reflection property
    Vector refl_direct = ( ray_direct - hit_normal * 2 * ray_direct.dot(hit_normal) ).normalize();

    RGB reflection = recursive_ray_trace(hit_point, refl_direct, depth + 1, pObject);

    color += pObject->reflection * reflection;
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

      ret_color = recursive_ray_trace(eye_pos, ray, 0);

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y   += y_grid_size;
    cur_pixel_pos.x   = x_start;
  }
}
