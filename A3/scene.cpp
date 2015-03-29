//
// this provide functions to set up the scene
//
#include "sphere.hpp"
#include "light.hpp"
#include "vector.hpp"
#include "chessboard.hpp"
#include <stdio.h>

extern Light light;

extern RGB global_ambient;
extern std::vector<Object *> scene;
extern ChessBoard * chess_board;

extern RGB background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern bool chessboard_on;

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr = RGB(0.5, 0.05, 0.8);

  // setup global ambient term
  global_ambient = RGB(0.2, 0.2, 0.2);

  // setup light 1
  light = Light( Point(-2.0, 5.0, 1.0), RGB(0.1, 0.1, 0.1), RGB(1.0, 1.0, 1.0), RGB(1.0, 1.0, 1.0), 0.5, 0.3, 0.0);

  // sphere 1
  Point sphere1_ctr         = Point(1.5, -0.2, -3.2);
  float sphere1_rad         = 1.23;
  RGB sphere1_ambient       = RGB(0.7, 0.7, 0.7);
  RGB sphere1_diffuse       = RGB(0.1, 0.5, 0.8);
  RGB sphere1_specular      = RGB(1.0, 1.0, 1.0);
  float sphere1_shineness   = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_transp      = 0.0;
  scene.push_back(new Sphere(sphere1_ctr, sphere1_rad, sphere1_ambient,
                  sphere1_diffuse, sphere1_specular, sphere1_shineness,
                  sphere1_reflectance, sphere1_transp));

  // sphere 2
  Point sphere2_ctr         = Point(-1.5, 0.0, -3.5);
  float sphere2_rad         = 1.5;
  RGB sphere2_ambient       = RGB(0.6, 0.6, 0.6);
  RGB sphere2_diffuse       = RGB(1.0, 0.0, 0.25);
  RGB sphere2_specular      = RGB(1.0, 1.0, 1.0);
  float sphere2_shineness   = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transp      = 0.0;
  scene.push_back(new Sphere(sphere2_ctr, sphere2_rad, sphere2_ambient,
                  sphere2_diffuse, sphere2_specular, sphere2_shineness,
                  sphere2_reflectance, sphere2_transp));

  // sphere 3
  Point sphere3_ctr         = Point(-0.35, 1.75, -2.25);
  float sphere3_rad         = 0.5;
  RGB sphere3_ambient       = RGB(0.2, 0.2, 0.2);
  RGB sphere3_diffuse       = RGB(0.0, 1.0, 0.25);
  RGB sphere3_specular      = RGB(0.0, 1.0, 0.0);
  float sphere3_shineness   = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transp      = 0.0;

  scene.push_back(new Sphere(sphere3_ctr, sphere3_rad, sphere3_ambient,
                  sphere3_diffuse, sphere3_specular, sphere3_shineness,
                  sphere3_reflectance, sphere3_transp));
  

  if( chessboard_on ){
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 0.5;
    float board_transp       = 0.0;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse, 
                    board_dark_diffuse, board_specular, 
                    board_shineness, board_reflectance, 
                    board_transp, -4.0, 1.0));
  }

}


void release_default_scene() {

  // release scene objects
  while (!scene.empty()) {
    Object * pObject = scene.back();
    scene.pop_back();
    delete pObject;
    pObject = NULL;
  }

}

/***************************************
 * Customized scene with checkboard
 ***************************************/
void set_up_user_scene() {

}

void release_user_scene()
{

}

/***************************************
 * bonus scene with chess
 ***************************************/

void set_up_bonus_scene(){

}

