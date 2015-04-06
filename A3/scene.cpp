//
// this provide functions to set up the scene
//
#include "sphere.hpp"
#include "light.hpp"
#include "vector.hpp"
#include "chessboard.hpp"
#include "chess.h"

#include <stdio.h>
#include <string>

extern Light light;

extern RGB global_ambient;
extern float global_transm;
extern std::vector<Object *> scene;
extern ChessBoard * chess_board;

extern RGB background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern bool chessboard_on;
extern int  bonus_scene_mode;
extern int  accerlerate_mode;
extern bool mirror_on;
//////////////////////////////////////////////////////////////////////////

// weight for diffuse reflection
float diffuse_reflection  = 0.1;

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr = RGB(0.5, 0.05, 0.8);

  // setup global ambient term
  global_ambient = RGB(0.2, 0.2, 0.2);

  // setup air tranmission rate
  global_transm = 1.0;

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
  float sphere1_transm      = 1.0;
  scene.push_back(new Sphere(sphere1_ambient,sphere1_diffuse,
                  sphere1_specular, sphere1_shineness,sphere1_reflectance,
                  sphere1_transp, sphere1_transm, diffuse_reflection, sphere1_ctr, sphere1_rad));

  // sphere 2
  Point sphere2_ctr         = Point(-1.5, 0.0, -3.5);
  float sphere2_rad         = 1.5;
  RGB sphere2_ambient       = RGB(0.6, 0.6, 0.6);
  RGB sphere2_diffuse       = RGB(1.0, 0.0, 0.25);
  RGB sphere2_specular      = RGB(1.0, 1.0, 1.0);
  float sphere2_shineness   = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transp      = 0.0;
  float sphere2_transm      = 1.0;
  scene.push_back(new Sphere(sphere2_ambient,sphere2_diffuse,
                  sphere2_specular, sphere2_shineness,sphere2_reflectance,
                  sphere2_transp, sphere2_transm, diffuse_reflection, sphere2_ctr, sphere2_rad));

  // sphere 3
  Point sphere3_ctr         = Point(-0.35, 1.75, -2.25);
  float sphere3_rad         = 0.5;
  RGB sphere3_ambient       = RGB(0.2, 0.2, 0.2);
  RGB sphere3_diffuse       = RGB(0.0, 1.0, 0.25);
  RGB sphere3_specular      = RGB(0.0, 1.0, 0.0);
  float sphere3_shineness   = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transp      = 0.0;
  float sphere3_transm      = 1.0;
  scene.push_back(new Sphere(sphere3_ambient,sphere3_diffuse,
                  sphere3_specular, sphere3_shineness,sphere3_reflectance,
                  sphere3_transp, sphere3_transm, diffuse_reflection, sphere3_ctr, sphere3_rad));

  if( chessboard_on ){
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 0.5;
    float board_transp       = 0.0;
    float board_transm       = 1.0;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse,
                    board_dark_diffuse, board_specular,
                    board_shineness, board_reflectance,
                    board_transp, board_transm, diffuse_reflection, -2.0, 1.5));
  }

}

/***************************************
 * Customized scene with checkboard
 ***************************************/
void set_up_user_scene() {
  // set background color
  background_clr = RGB(0.5, 0.05, 0.8);
  // background_clr = RGB(0.8, 0.8, 0.8);

  // setup global ambient term
  global_ambient = RGB(0.2, 0.2, 0.2);

  // setup air tranmission rate
  global_transm = 1.0;

  // setup light 1
  light = Light( Point(-2.0, 5.0, 1.0), RGB(0.1, 0.1, 0.1), RGB(1.0, 1.0, 1.0),
    RGB(1.0, 1.0, 1.0), 0.5, 0.3, 0.0);

  // sphere 1
  Point sphere1_ctr         = Point(1.5, -0.2, -3.2);
  float sphere1_rad         = 1.23;
  RGB sphere1_ambient       = RGB(0.7, 0.7, 0.7);
  RGB sphere1_diffuse       = RGB(0.1, 0.5, 0.8);
  RGB sphere1_specular      = RGB(1.0, 1.0, 1.0);
  float sphere1_shineness   = 10;
  float sphere1_reflectance = 0.5;
  float sphere1_transp      = 0.5;
  float sphere1_transm      = float(1.52);
  scene.push_back(new Sphere(sphere1_ambient,sphere1_diffuse,
                  sphere1_specular, sphere1_shineness,sphere1_reflectance,
                  sphere1_transp, sphere1_transm, diffuse_reflection, sphere1_ctr, sphere1_rad));

  // sphere 2
  Point sphere2_ctr         = Point(-1.5, 0.0, -3.5);
  float sphere2_rad         = 1.5;
  RGB sphere2_ambient       = RGB(0.6, 0.6, 0.6);
  RGB sphere2_diffuse       = RGB(1.0, 0.0, 0.25);
  RGB sphere2_specular      = RGB(1.0, 1.0, 1.0);
  float sphere2_shineness   = 6;
  float sphere2_reflectance = 0.5;
  float sphere2_transp      = 0.5;
  float sphere2_transm      = float(1.52);
  scene.push_back(new Sphere(sphere2_ambient,sphere2_diffuse,
                  sphere2_specular, sphere2_shineness,sphere2_reflectance,
                  sphere2_transp, sphere2_transm, diffuse_reflection, sphere2_ctr, sphere2_rad));

  // sphere 3
  Point sphere3_ctr         = Point(-0.35, 1.75, -2.25);
  float sphere3_rad         = 0.5;
  RGB sphere3_ambient       = RGB(0.2, 0.2, 0.2);
  RGB sphere3_diffuse       = RGB(0.0, 1.0, 0.25);
  RGB sphere3_specular      = RGB(0.0, 1.0, 0.0);
  float sphere3_shineness   = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transp      = 0.3;
  float sphere3_transm      = float(1.52);
  scene.push_back(new Sphere(sphere3_ambient,sphere3_diffuse,
                  sphere3_specular, sphere3_shineness,sphere3_reflectance,
                  sphere3_transp, sphere3_transm, diffuse_reflection, sphere3_ctr, sphere3_rad));

  if( chessboard_on )
  {
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 1.0;
    float board_transp       = 0.0;
    float board_transm       = 2.4;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse,
                    board_dark_diffuse, board_specular,
                    board_shineness, board_reflectance,
                    board_transp, board_transm, diffuse_reflection, -2.0, 1.5));
  }

  if( mirror_on )
  {
    // Mirror material property
    RGB mirror_ambient       = RGB(0.3, 0.3, 0.3);
    RGB mirror_diffuse       = RGB(0.6, 0.6, 0.6);
    RGB mirror_specular      = RGB(0.9, 0.9, 0.9);
    float mirror_shineness   = 40;
    float mirror_reflectance = 1.0;
    float mirror_transp      = 0.0;
    float mirror_transm      = float(1.0);

    // place a mirror to the back
    Mesh *pMesh = new Mesh(mirror_ambient, mirror_diffuse, mirror_specular, mirror_shineness, mirror_reflectance, mirror_transp, mirror_transm, diffuse_reflection);

    pMesh->addTriangle(Point(-3.0, -2.0, -4.0), Point(3.0, -2.0, -4.0), Point(-3.0, 2.0, -4.0));

    pMesh->addTriangle(Point(-3.0, 2.0, -4.0), Point(3.0, 2.0, -4.0), Point(3.0, -2.0, -4.0));

    scene.push_back(pMesh);
  }
}

/***************************************
 * bonus scene with chess
 ***************************************/

void set_up_bonus_scene(){
  // set background color
  background_clr = RGB(0.2, 0.5, 0.8);
  // background_clr = RGB(0.8, 0.8, 0.8);

  // setup global ambient term
  global_ambient = RGB(0.2, 0.2, 0.2);

  // setup air tranmission rate
  global_transm = 1.0;

  // setup light 1
  light = Light( Point(-2.0, 5.0, 1.0), RGB(0.1, 0.1, 0.1), RGB(1.0, 1.0, 1.0), RGB(1.0, 1.0, 1.0), 0.5, 0.3, 0.0);

  // Chess material property
  RGB chess_ambient       = RGB(0.02, 0.12, 0.2);
  RGB chess_diffuse       = RGB(0.02, 0.22, 0.42);
  RGB chess_specular      = RGB(0.02, 0.22, 0.42);
  float chess_shineness   = 30;
  float chess_reflectance = 0.3;

  float chess_transp      = 0.9;
  // float chess_transm      = float(1 / 1.52);
  float chess_transm      = float(1.52);
  // Mirror material property
  RGB mirror_ambient       = RGB(0.3, 0.3, 0.3);
  RGB mirror_diffuse       = RGB(0.6, 0.6, 0.6);
  RGB mirror_specular      = RGB(0.9, 0.9, 0.9);
  float mirror_shineness   = 40;
  float mirror_reflectance = 1.0;
  float mirror_transp      = 0.3;
  float mirror_transm      = float(1.52);

  if( mirror_on )
  {
    // place a mirror to the back
    Mesh *pMesh = new Mesh(mirror_ambient, mirror_diffuse, mirror_specular, mirror_shineness, mirror_reflectance, mirror_transp, mirror_transm, diffuse_reflection);

    pMesh->addTriangle(Point(-3.0, -2.0, -3.0), Point(3.0, -2.0, -3.0), Point(-3.0, 2.0, -3.0));

    pMesh->addTriangle(Point(-3.0, 2.0, -3.0), Point(3.0, 2.0, -3.0), Point(3.0, -2.0, -3.0));

    scene.push_back(pMesh);
  }

  // Bonus scene mode :
  //  0 - just load chess_piece.smf
  //  1 - load chess_piece.smf and bishop.smf
  //  2 - load chess_hire.smf and bishop_hire.smf
  //  3 - load all chess files

  if( bonus_scene_mode == 0 )
  {
    // Single Object for testing
    //  - just load chess_piece.smf
    scene.push_back( new Chess(chess_ambient,  chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/chess_piece.smf"), Vector(1.0, -2.0, -2.0), 1.0, accerlerate_mode) );
  }
  else if ( bonus_scene_mode == 1 )
  {
    // Low resolution objects
    //  - load chess_piece.smf and bishop.smf
    scene.push_back( new Chess(chess_ambient, chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/bishop.smf"), Vector(0.5, -2.0, -2.0), 15.0, accerlerate_mode) );
    scene.push_back( new Chess(chess_ambient,  chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/chess_piece.smf"), Vector(1.0, -2.0, -2.0), 1.0, accerlerate_mode) );
  }
  else if( bonus_scene_mode == 2 )
  {
    // High resolution objects
    //  - load chess_hire.smf and bishop_hire.smf
    scene.push_back( new Chess(chess_ambient, chess_diffuse, chess_specular,chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/chess_hires.smf"), Vector(-0.5, -2.0, -2.0), 1.0, accerlerate_mode) );
    scene.push_back( new Chess(chess_ambient, chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/bishop_hires.smf"), Vector(-1.0, -2.0, -2.0), 15.0, accerlerate_mode) );
  }
  else if( bonus_scene_mode == 3 )
  {
    // All chess objects
    //  - load all chess files

    scene.push_back( new Chess(chess_ambient, chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/chess_hires.smf"), Vector(-0.5, -2.0, -2.0), 1.0, accerlerate_mode) );

    scene.push_back( new Chess(chess_ambient, chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/bishop_hires.smf"), Vector(-1.0, -2.0, -2.0), 15.0, accerlerate_mode) );

    scene.push_back( new Chess(chess_ambient, chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/bishop.smf"), Vector(0.5, -2.0, -2.0), 15.0, accerlerate_mode) );

    scene.push_back( new Chess(chess_ambient,  chess_diffuse, chess_specular, chess_shineness, chess_reflectance, chess_transp, chess_transm, diffuse_reflection, std::string("chess_pieces/chess_piece.smf"), Vector(1.0, -2.0, -2.0), 1.0, accerlerate_mode) );

  }



  if( chessboard_on ){
    // add chess board to the ground of the chess
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 0.5;
    float board_transp       = 0.2;
    float board_transm       = 2.4;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse,
                    board_dark_diffuse, board_specular,
                    board_shineness, board_reflectance,
                    board_transp, board_transm, diffuse_reflection, -2.0, 1.0, true));
  }
}


void release_scene()
{
  // release scene objects
  while (!scene.empty()) {
    Object * pObject = scene.back();
    scene.pop_back();
    delete pObject;
    pObject = NULL;
  }
}
