#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.hpp"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern vector<Sphere> scene;

// light 1 position and color
extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern bool shadow_on;
extern bool illuminance_on;
extern bool refraction_on;
extern bool checkboard_on;
extern bool diffuse_interreflection_on;
extern bool antialiasing_on;

extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(const Point &q, const Vector &v, 
  const Vector &surf_norm, const vector<Sphere> &scene) {

//
// do your thing here
//

	RGB_float color;
	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(const Point &eye, const Vector &direction, 
  const vector<Sphere> &local_scene, const int &depth) {
  
  const Sphere *pSphere = NULL;

  float tnear = INFINITY;
  // find ray-sphere intersection
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    float hit0 = INFINITY,  hit1 = INFINITY;
    if (scene[i].intersect(eye, direction, &hit0, &hit1)) {
      // if the intersection is in the opposite side of sphere,
      if (hit0 < 0) hit0 = hit1;
      if (hit0 < tnear) {
        tnear = hit0;
        pSphere = &scene[i];
      }
    }
  }

  // if there is no intersection found, return background color
  if (!pSphere) return background_clr;
  RGB_float color;
  Point     pHit = eye + direction * tnear;

  // calculate shading
  if ( depth < step_max )
  {

  }
  else {

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
  RGB_float   ret_color;
  Point       cur_pixel_pos;
  Vector      ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i < win_height; i++) {
    for (j=0; j < win_width; j++) {
      ray = eye_pos - cur_pixel_pos;

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray, scene, 0);

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

      // Checkboard for testing
      RGB_float clr = RGB_float(float(i/32), 0, float(j/32));
      ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y   += y_grid_size;
    cur_pixel_pos.x   = x_start;
  }
}
