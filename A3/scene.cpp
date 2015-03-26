//
// this provide functions to set up the scene
//
#include "sphere.hpp"
#include "vector.hpp"
#include <stdio.h>

extern Point light;
extern Vector light_ambient;
extern Vector light_diffuse;
extern Vector light_specular;

extern Vector global_ambient;
extern vector<Sphere> scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr = RGB_float(0.5, 0.05, 0.8);

  // setup global ambient term
  global_ambient = Vector(0.2, 0.2, 0.2);

  // setup light 1
  light = Point(-2.0, 5.0, 1.0);

  light_ambient  = Vector(0.1, 0.1, 0.1);
  light_diffuse  = Vector(1.0, 1.0, 1.0);
  light_specular = Vector(1.0, 1.0, 1.0);

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // sphere 1
  Point sphere1_ctr         = Point(1.5, -0.2, -3.2);
  float sphere1_rad         = 1.23;
  Vector sphere1_ambient    = Vector(0.7, 0.7, 0.7);
  Vector sphere1_diffuse    = Vector(0.1, 0.5, 0.8);
  Vector sphere1_specular   = Vector(1.0, 1.0, 1.0);
  float sphere1_shineness   = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_transp      = 0.0;
  scene.push_back(Sphere(sphere1_ctr, sphere1_rad, sphere1_ambient,
                  sphere1_diffuse, sphere1_specular, sphere1_shineness,
                  sphere1_reflectance, sphere1_transp));

  // sphere 2
  Point sphere2_ctr         = Point(-1.5, 0.0, -3.5);
  float sphere2_rad         = 1.5;
  Vector sphere2_ambient    = Vector(0.6, 0.6, 0.6);
  Vector sphere2_diffuse    = Vector(1.0, 0.0, 0.25);
  Vector sphere2_specular   = Vector(1.0, 1.0, 1.0);
  float sphere2_shineness   = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transp      = 0.0;
  scene.push_back(Sphere(sphere2_ctr, sphere2_rad, sphere2_ambient,
                  sphere2_diffuse, sphere2_specular, sphere2_shineness,
                  sphere2_reflectance, sphere2_transp));

  // sphere 3
  Point sphere3_ctr         = Point(-0.35, 1.75, -2.25);
  float sphere3_rad         = 0.5;
  Vector sphere3_ambient    = Vector(0.2, 0.2, 0.2);
  Vector sphere3_diffuse    = Vector(0.0, 1.0, 0.25);
  Vector sphere3_specular   = Vector(0.0, 1.0, 0.0);
  float sphere3_shineness   = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transp      = 0.0;

  scene.push_back(Sphere(sphere3_ctr, sphere3_rad, sphere3_ambient,
                  sphere3_diffuse, sphere3_specular, sphere3_shineness,
                  sphere3_reflectance, sphere3_transp));
}

/***************************************
 * Customized scene with checkboard
 ***************************************/
void set_up_user_scene() {

}

/***************************************
 * bonus scene with chess
 ***************************************/

void set_up_bonus_scene(){

}
