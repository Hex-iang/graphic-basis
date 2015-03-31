//
// this provide functions to set up the scene
//
#include "sphere.hpp"
#include "light.hpp"
#include "vector.hpp"
#include "chessboard.hpp"
#include "chess.hpp"

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

//////////////////////////////////////////////////////////////////////////

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
  float sphere1_transm      = 1.1;
  scene.push_back(new Sphere(sphere1_ambient,sphere1_diffuse, 
                  sphere1_specular, sphere1_shineness,sphere1_reflectance, 
                  sphere1_transp, sphere1_transm, sphere1_ctr, sphere1_rad));

  // sphere 2
  Point sphere2_ctr         = Point(-1.5, 0.0, -3.5);
  float sphere2_rad         = 1.5;
  RGB sphere2_ambient       = RGB(0.6, 0.6, 0.6);
  RGB sphere2_diffuse       = RGB(1.0, 0.0, 0.25);
  RGB sphere2_specular      = RGB(1.0, 1.0, 1.0);
  float sphere2_shineness   = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transp      = 0.0;
  float sphere2_transm      = 1.1;
  scene.push_back(new Sphere(sphere2_ambient,sphere2_diffuse, 
                  sphere2_specular, sphere2_shineness,sphere2_reflectance, 
                  sphere2_transp, sphere2_transm, sphere2_ctr, sphere2_rad));

  // sphere 3
  Point sphere3_ctr         = Point(-0.35, 1.75, -2.25);
  float sphere3_rad         = 0.5;
  RGB sphere3_ambient       = RGB(0.2, 0.2, 0.2);
  RGB sphere3_diffuse       = RGB(0.0, 1.0, 0.25);
  RGB sphere3_specular      = RGB(0.0, 1.0, 0.0);
  float sphere3_shineness   = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transp      = 0.0;
  float sphere3_transm      = 1.1;  
  scene.push_back(new Sphere(sphere3_ambient,sphere3_diffuse, 
                  sphere3_specular, sphere3_shineness,sphere3_reflectance, 
                  sphere3_transp, sphere3_transm, sphere3_ctr, sphere3_rad));

  if( chessboard_on ){
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 0.5;
    float board_transp       = 0.0;
    float board_transm     = 1.0;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse, 
                    board_dark_diffuse, board_specular, 
                    board_shineness, board_reflectance, 
                    board_transp, board_transm, -4.0, 1.0));
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
  light = Light( Point(-2.0, 5.0, 1.0), RGB(0.1, 0.1, 0.1), RGB(1.0, 1.0, 1.0), RGB(1.0, 1.0, 1.0), 0.5, 0.3, 0.0);

  // sphere 1
  Point sphere1_ctr         = Point(1.5, -0.2, -3.2);
  float sphere1_rad         = 1.23;
  RGB sphere1_ambient       = RGB(0.7, 0.7, 0.7);
  RGB sphere1_diffuse       = RGB(0.1, 0.5, 0.8);
  RGB sphere1_specular      = RGB(1.0, 1.0, 1.0);
  float sphere1_shineness   = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_transp      = 0.8;
  float sphere1_transm      = 1.2;
  scene.push_back(new Sphere(sphere1_ambient,sphere1_diffuse, 
                  sphere1_specular, sphere1_shineness,sphere1_reflectance, 
                  sphere1_transp, sphere1_transm, sphere1_ctr, sphere1_rad));

  // sphere 2
  Point sphere2_ctr         = Point(-1.5, 0.0, -3.5);
  float sphere2_rad         = 1.5;
  RGB sphere2_ambient       = RGB(0.6, 0.6, 0.6);
  RGB sphere2_diffuse       = RGB(1.0, 0.0, 0.25);
  RGB sphere2_specular      = RGB(1.0, 1.0, 1.0);
  float sphere2_shineness   = 6;
  float sphere2_reflectance = 0.2;
  float sphere2_transp      = 0.8;
  float sphere2_transm      = 1.3;
  scene.push_back(new Sphere(sphere2_ambient,sphere2_diffuse, 
                  sphere2_specular, sphere2_shineness,sphere2_reflectance, 
                  sphere2_transp, sphere2_transm, sphere2_ctr, sphere2_rad));

  // sphere 3
  Point sphere3_ctr         = Point(-0.35, 1.75, -2.25);
  float sphere3_rad         = 0.5;
  RGB sphere3_ambient       = RGB(0.2, 0.2, 0.2);
  RGB sphere3_diffuse       = RGB(0.0, 1.0, 0.25);
  RGB sphere3_specular      = RGB(0.0, 1.0, 0.0);
  float sphere3_shineness   = 30;
  float sphere3_reflectance = 0.2;
  float sphere3_transp      = 0.5;
  float sphere3_transm      = 1.1;  
  scene.push_back(new Sphere(sphere3_ambient,sphere3_diffuse, 
                  sphere3_specular, sphere3_shineness,sphere3_reflectance, 
                  sphere3_transp, sphere3_transm, sphere3_ctr, sphere3_rad));

  if( chessboard_on ){
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 0.0;
    float board_transp       = 0.0;
    float board_transm       = 1.0;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse, 
                    board_dark_diffuse, board_specular, 
                    board_shineness, board_reflectance, 
                    board_transp, board_transm, -4.0, 1.0));
  }
}

#define DEBUG
/***************************************
 * bonus scene with chess
 ***************************************/

void naive_load(const RGB &amb, const RGB &dif, const RGB &spe, 
    const float &shine, const float &refl, const float &transp, 
    const float &transm, const string &str)
{
    int line_cnt      = 0;
    int vertices_cnt  = 0;
    int triangles_cnt = 0;

    vector<Point > vertices;

    std::ifstream infile(str.c_str());
    std::cout << "reading file:" << str << "..." << std::endl;
    std::string line;
    while (std::getline(infile, line)){
      line_cnt ++;
      std::istringstream buffer(line);
      char type;
      // read the first character and see which type of line it is
      buffer >> type;
      
      if(type == '#') continue;

      if(type == 'v')
      {
        vertices_cnt++;

        float x, y, z;
        buffer >> x >> y >> z;
        Point p(x, y, z);

        vertices.push_back(p);
      }
      else if( type == 'f')
      {
        triangles_cnt++;

        int i, j, k;
        buffer >> i >> j >> k;
        scene.push_back( new Triangle(amb, dif, spe, shine, refl, transp, transm, vertices[i], vertices[j], vertices[k]));
      }
      
    }
    
#ifdef DEBUG
      std::cout << "[naive_load] Read file complete: " << line_cnt << " lines in total" << std::endl;

      std::cout << "total points number: "    << vertices_cnt << endl;
      std::cout << "total triangles number: " << triangles_cnt << endl;
#endif

}


void set_up_bonus_scene(){
  // set background color
  background_clr = RGB(0.5, 0.05, 0.8);
  // background_clr = RGB(0.8, 0.8, 0.8);

  // setup global ambient term
  global_ambient = RGB(0.2, 0.2, 0.2);

  // setup air tranmission rate
  global_transm = 1.0;
  
  // setup light 1
  light = Light( Point(-2.0, 5.0, 1.0), RGB(0.1, 0.1, 0.1), RGB(1.0, 1.0, 1.0), RGB(1.0, 1.0, 1.0), 0.5, 0.3, 0.0);

  RGB chess_ambient       = RGB(0.3, 0.3, 0.3);
  RGB chess_diffuse       = RGB(0.5, 0.5, 1.0);
  RGB chess_specular      = RGB(0.8, 0.8, 0.8);
  float chess_shineness   = 30;
  float chess_reflectance = 0.5;
  float chess_transp      = 0.2;
  float chess_transm      = 1.0;  

  scene.push_back( new Chess(chess_ambient, chess_diffuse, 
                             chess_specular,chess_shineness,
                             chess_reflectance, chess_transp, chess_transm, std::string("chess_pieces/chess_piece.smf")));

  // scene.push_back( new Chess(chess_ambient, chess_diffuse, 
  //                            chess_specular,chess_shineness,
  //                            chess_reflectance, chess_transp, chess_transm, std::string("chess_pieces/bishop.smf")));

  // naive_load(chess_ambient, chess_diffuse,
  //            chess_specular,chess_shineness,
  //            chess_reflectance, chess_transp, 
  //            chess_transm, std::string("chess_pieces/bishop.smf"));

  // naive_load(chess_ambient, chess_diffuse,
  //            chess_specular,chess_shineness,
  //            chess_reflectance, chess_transp, 
  //            chess_transm, std::string("chess_pieces/chess_piece.smf"));

  if( chessboard_on ){
    // add chess board to the ground of the chess 
    RGB board_ambient        = RGB(0.20, 0.20, 0.20);
    RGB board_light_diffuse  = RGB(1.00, 1.00, 1.00);
    RGB board_dark_diffuse   = RGB(0.10, 0.10, 0.10);
    RGB board_specular       = RGB(1.00, 1.00, 1.00);
    float board_shineness    = 30;
    float board_reflectance  = 0.0;
    float board_transp       = 0.0;
    float board_transm       = 1.0;

    scene.push_back(new ChessBoard(board_ambient, board_light_diffuse, 
                    board_dark_diffuse, board_specular, 
                    board_shineness, board_reflectance, 
                    board_transp, board_transm, -4.0, 1.0));
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

