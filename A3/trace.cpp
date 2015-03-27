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
extern Point  light_source;
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

bool shadow_test(const Point &ray_origin, const Vector &ray_direct, const Sphere * psh_ignore)
{
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    if( psh_ignore == scene[i] ) continue;

    float hit0 = INFINITY,  hit1 = INFINITY;
    // maximum ray range
    float tmax = 1000.0;
    if (scene[i]->intersect(ray_origin, ray_direct, tmax, &hit0, &hit1)) {
      if(hit1 < 0 && hit0 < 0) continue;
      
      return false;
    }
  }
  return true;
}

/*********************************************************************
 * Scene intersection
 *********************************************************************/
float intersect_scene(const Point &ray_origin, const Vector &ray_direct, Sphere * &pSphere, Sphere * pShere_ignore = NULL)
{
  float tHit = INFINITY;
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    float hit0 = INFINITY,  hit1 = INFINITY;
    // maximum ray range
    float tmax = 1000.0;
    if (scene[i]->intersect(ray_origin, ray_direct, tmax, &hit0, &hit1)) {
      // skip the sphere we want to ignore
      if(pShere_ignore == scene[i]) continue;

      // if hit0 < 0, means that ray_origin inside sphere, should use hit1
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


/*********************************************************************
 * Phong local illumination + shadow ray generation
 *********************************************************************/
RGB phong(const Point &hit_point, const Vector &view, const Vector &hit_normal, const Sphere * sphere) 
{
  // first calculate globla/local ambient term for the sphere 
	RGB color = global_ambient * sphere->mat_ambient + light_ambient * sphere->mat_ambient;

  // assume that we have a point light source and shot the shadow ray
  Vector shadow_ray = light_source - hit_point; 
  
  // Normalize it shadow ray to get its direction L
  Vector L = (light_source - hit_point).normalize();

  if( (!shadow_on) || shadow_test(hit_point, shadow_ray, sphere) ){

    float d_L2 = shadow_ray.dot(shadow_ray);
    float d_L = std::sqrt(d_L2);
    float light_att = 1 / (decay_a + decay_b * d_L + decay_c * d_L2);

    // second, plus diffuse reflectance for the object
    color += light_diffuse * sphere->mat_diffuse * std::max(hit_normal.dot(L), (float)0.0) * light_att;

    // compute vector R from the L and normal
    Vector R = (hit_normal * 2 * (hit_normal.dot(L)) - L).normalize();
    
    // third, plus specular reflectance for the object
    RGB specular = light_specular * sphere->mat_specular * std::max( (float)std::pow(R.dot(view), sphere->mat_shineness), (float)0.0) * light_att;
    
    color += specular;

  }

  return color;
}

/************************************************************************
 * This is the recursive ray tracer ************************************************************************/
RGB recursive_ray_trace(const Point &ray_origin, const Vector &ray_direct, const int &depth, Sphere * pShere_ignore = NULL) {
  
  Sphere * pSphere = NULL;

  // find the nearest ray-sphere intersection point
  float tHit = intersect_scene(ray_origin, ray_direct, pSphere, pShere_ignore);

  // if there is no intersection found, return background color
  if (!pSphere){
    if(depth > 0)
      return null_clr;
    else
      return background_clr;
  }

  bool inSphere = false;
  // find intersection point and its corresponding surface normal 
  RGB     color(0.0, 0.0, 0.0);
  Point   hit_point  = ray_origin + ray_direct * tHit;
  Vector  hit_normal = pSphere->normal(hit_point);

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
  color = phong(hit_point, view, hit_normal, pSphere);

  // Recursive ray tracing for reflection
  if ( reflection_on && ( pSphere->reflection > 0 ) && ( depth < step_max ) )
  {
    // for object with reflection property
    Vector refl_direct = ( ray_direct - hit_normal * 2 * ray_direct.dot(hit_normal) ).normalize();

    RGB reflection = recursive_ray_trace(hit_point, refl_direct, depth + 1, pSphere);

    if(depth == 0)
    {
      std::cout << "reflection color accumulated: " << pSphere->reflection * reflection << std::endl;
    }

    color += pSphere->reflection * reflection;
  }
  // else {
  //   // when reflection is turn off, just use phong model to calculate
  //   // the local reflectance at current point

  //   // view = - ray_direct
  //   Vector view = - ray_direct;
  //   color = phong(hit_point, view, hit_normal, pSphere);

  // }
	
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
