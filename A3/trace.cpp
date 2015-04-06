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
#include "chessboard.hpp"
#include "rand.hpp"
#include "ray.hpp"

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
extern ChessBoard * chess_board;
// light 1
extern Light light;

// global ambient term
extern Vector global_ambient;
// global transmission rate
extern float global_transm;

// light decay parameters

extern bool shadow_on;
extern bool reflection_on;
extern bool refraction_on;
extern bool diffuse_reflection_on;
extern bool antialiasing_on;
extern bool poisson_on;

extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Shadow test for phong model
 *********************************************************************/

bool shadow_test(const Ray &shadow_ray, const Object * pObject_current)
{
  // find the nearest ray-sphere intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    if( pObject_current == scene[i] ) continue;

    Intersection hit(INFINITY);

    if (scene[i]->intersect(shadow_ray, hit)) {
      return false;
    }
  }
  return true;
}

/*********************************************************************
 * Scene intersection
 *********************************************************************/
Intersection intersect_scene(const Ray & ray, Object * &pObject, Object * pObject_current)
{
  Intersection tHit(INFINITY);
  // find the nearest ray-object intersection point
  for (unsigned i = 0; i < scene.size(); ++i)
  {
    // skip the object we want to ignore
    if(pObject_current == scene[i]) continue;
    Intersection hit(INFINITY);
    // maximum ray range
    if (scene[i]->intersect(ray, hit)) {
      if (hit.t < tHit.t) {
        // if current object have a more near hit, use it
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
RGB phong(const Intersection &hit, const Vector &view, const Object * pObject)
{
  // first calculate globla/local ambient term for the sphere
	// RGB color;
  RGB color = (global_ambient + light.ambient ) * pObject->ambient(hit.point);
  // assume that we have a point light source and shot the shadow ray
  // Vector shadow_ray = hit.point - light.source;
  Vector shadow_ray = (light.source - hit.point).normalize();

  // Normalize it shadow ray to get its direction L
  Vector L = light.source - hit.point;

  // if( (!shadow_on) || shadow_test( Ray(light.source, shadow_ray), pObject) ){
  if( (!shadow_on) || shadow_test( Ray(hit.point, shadow_ray), pObject) ){
    float d_L2 = L.dot(L);
    float d_L = std::sqrt(d_L2);
    float light_att = light.attenuation(d_L, d_L2);

    L = L.normalize();
    // second, plus diffuse reflectance for the object
    color += light.diffuse * pObject->diffuse(hit.point) * std::max((hit.normal).dot(L), (float)0.0) * light_att;

    // compute vector R from the L and normal
    Vector R = ( hit.normal * 2 * ( (hit.normal).dot(L)) - L ).normalize();

    // third, plus specular reflectance for the object
    color += light.specular * pObject->specular(hit.point) * light_att *
        (float)std::pow( std::max(R.dot(view),(float)0.0),
                          pObject->shineness(hit.point) );
  }

  return color;
}

/************************************************************************
 * This is the recursive ray tracer ************************************************************************/
RGB recursive_ray_trace(const Ray &ray, const int &depth, Object * pObject_current = NULL) {

  Object * pObject = NULL;

  // find the nearest ray-sphere intersection point
  Intersection hit = intersect_scene(ray, pObject, pObject_current);

  // if there is no intersection found
  if (!pObject){

    if( depth > 0 )
    {
      // if no intersection, then return background color when depth > 0
      return null_clr;
    }
    // or return null color otherwise
    else
      return background_clr;
  }

  // find intersection point and its corresponding surface normal
  RGB     color(0.0, 0.0, 0.0);
  RGB     reflection(0.0, 0.0, 0.0);
  RGB     refraction(0.0, 0.0, 0.0);
  RGB     diffuse(0.0, 0.0, 0.0);

  // bool value for checking if the ray is inside a object
  bool inside = false;

  // if normal and ray ray_direct on the same side, means ray_origin inside sphere
  // so we should reverse the normal ray_direct
  if (hit.normal.dot(ray.direction) > 0)
  {
    hit.normal = - hit.normal;
    inside = true;
  }

  // Calculate local reflectance at current point

  // view = - ray_direct
  Vector view = - ray.direction;

  // recursive ray tracing for diffuse inter-reflection
  if( diffuse_reflection_on && ( pObject->diffuse_reflection(hit.point) > 0) && (depth < step_max) )
  {
    // if diffuse inter-reflection is on, there is no ambient light any more
    Rand random( (float) -1.0, (float ) 1.0);
    for(int i = 0; i < DIFFUSE_RAY_NUM; i++)
    {
      // randomly generate rays for diffused reflection orientations
      bool valid;
      Vector diffuse_direct;
      do{
        valid = true;
        diffuse_direct = random.generate();
        // the random diffuse ray must on the same side of hit point normal
        if( diffuse_direct.dot(hit.normal) < 0)
          valid = false;
      }while(!valid);

      diffuse += recursive_ray_trace(Ray(hit.point, diffuse_direct), depth + 1, pObject);
    }
    diffuse = (diffuse * (float) (1.0 / DIFFUSE_RAY_NUM )).rerange();
    // std::cout << "diffuse color: " << diffuse << std::endl;
  }

  // Recursive ray tracing for reflection
  if ( reflection_on && ( pObject->reflection(hit.point) > 0 )
  && ( depth < step_max ) )
  {
    // for object with reflection property
    Vector refl_direct = ( ray.direction - hit.normal * 2 * ray.direction.dot(hit.normal) ).normalize();

    reflection = recursive_ray_trace(Ray(hit.point, refl_direct), depth + 1, pObject);

  }

  // recursive ray tracing for refraction
  if ( refraction_on && ( pObject->transparency(hit.point) > 0 )
  && ( depth < step_max) )
  {
    // In our scene, we are always outside of the sphere

    float eta = 0;

    if (inside){
      if(pObject_current == NULL)
        eta = pObject->transmission(hit.point) / global_transm;
      else
        eta = pObject->transmission(hit.point) / pObject_current->transmission(hit.point);
    }
    else{
      if(pObject_current == NULL)
        eta = global_transm / pObject->transmission(hit.point);
      else
        eta = pObject_current->transmission(hit.point) / pObject->transmission(hit.point);
    }

    // Sin(refr) = Sin(in) * eta
    float cos_in = hit.normal.dot( view );
    // Cos(refr) = sqrt( 1 - (Sin(in) * eta)^2 )
    float cos_refr_2 = 1 - eta * eta * (1 - cos_in * cos_in) ;

    if( cos_refr_2 >= 0 ){

      float cos_refr = sqrt(cos_refr_2);
      // Vector for refraction direction
      Vector refr_direct = (eta * cos_in - cos_refr) * hit.normal - ( view * eta);
      refr_direct = refr_direct.normalize();
      // avoid total refraction
      refraction = recursive_ray_trace(Ray(hit.point, refr_direct), depth + 1, pObject);
    }
    else
    {
      // total refraction case, no refraction
      refraction = RGB(0.0, 0.0, 0.0);
    }

  }

  // calculate final color effect
  color = pObject->transparency(hit.point) * refraction
        + pObject->reflection(hit.point) * reflection
        + phong(hit, view, pObject)
        + pObject->diffuse_reflection(hit.point) * diffuse;

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
  Vector      direct;
  RGB         ret_color;
  Point       cur_pixel_pos;
  Vector      ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i < win_height; i++) {
    for (j=0; j < win_width; j++) {

      if(antialiasing_on && poisson_on)
      {
        // super sampling the result
        float antialias_cnt = 5.0;
        Poisson pRandom( float( sqrt(x_grid_size * y_grid_size) / ( 2 * sqrt(antialias_cnt) ) ), (float) 0.0, (float) 1.0);
        pRandom.reset();

        for (int k = 0; k < int(antialias_cnt); ++k)
        {
          Vector new_pixel_pos = cur_pixel_pos;
          Point p = pRandom.generate();

          new_pixel_pos.x += (p.x + i) / float(win_height) * x_grid_size;
          new_pixel_pos.y += (p.y + j) / float(win_width ) * y_grid_size;

          Vector direct = (new_pixel_pos - eye_pos).normalize();
          ret_color += recursive_ray_trace( Ray(eye_pos, direct), 0);
        }
        ret_color = ( ret_color * float( 1.0 / antialias_cnt)).rerange();
      }
      else if( antialiasing_on )
      {
        float antialias_cnt = 5.0;
        int sampling[][2] = { {-1, -1}, {0, 0}, {-1, 1}, {1, -1}, {1, 1}};
        for (int k = 0; k < int(antialias_cnt); ++k)
        {
          Vector new_pixel_pos = cur_pixel_pos;

          new_pixel_pos.x += sampling[k][0] * 0.25 * x_grid_size;
          new_pixel_pos.y += sampling[k][1] * 0.25 * y_grid_size;

          direct = (new_pixel_pos - eye_pos).normalize();
          ret_color += recursive_ray_trace( Ray(eye_pos, direct), 0);
        }
        ret_color = ( ret_color * float( 1.0 / antialias_cnt)).rerange();

      }
      else
      {
        // traditional ray tracing
        direct = (cur_pixel_pos - eye_pos).normalize();

        ret_color = recursive_ray_trace( Ray(eye_pos, direct), 0);
      }

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y   += y_grid_size;
    cur_pixel_pos.x   = x_start;
  }
}
