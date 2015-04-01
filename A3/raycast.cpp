/***********************************************************
 * CMPT 361, Summer 2014
 *
 *  raycast.cpp
 *      
 *  Render a simple scene using ray tracing
 * 
 *  NAME:         Hexiang Hu
 *  SFU ID:       301239910
 *
***********************************************************/

#include "include/Angel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <ctime>

#include "chessboard.hpp"
#include "light.hpp"
#include "trace.h"
#include "global.h"
#include "sphere.hpp"
#include "image_util.h"
#include "scene.h"

//
// Global variables
//

int win_width = WIN_WIDTH;
int win_height = WIN_HEIGHT;

GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];   
// array for the final image 
// This gets displayed in glut window via texture mapping, 
// you can also save a copy as bitmap by pressing 's'

float image_width = IMAGE_WIDTH;
float image_height = (float(WIN_HEIGHT) / float(WIN_WIDTH)) * IMAGE_WIDTH;

// some colors
RGB background_clr;         // background color
RGB null_clr = RGB(0.0, 0.0, 0.0);   // NULL color

//
// these view parameters should be fixed
//
// Point eye_pos = Point(0.0, 0.0, 0.0);            // eye position
// float image_plane = -1.5;                        // image plane position
Point eye_pos = Point(0.0, 0.0, 1.5);               // eye position
// float image_plane = -1.5;                           // image plane position
float image_plane = 0.0;
// list of spheres in the scene
vector<Object *> scene;
ChessBoard * chess_board;

// light 1 position and color
Light light;

// global ambient term
Vector global_ambient;
float global_transm;

// light decay parameters
float decay_a;
float decay_b;
float decay_c;

// maximum level of recursions; you can use to control whether reflection
// is implemented and for how many levels
int step_max = 1;

// All global flags for controlling different degree of ray tracing
bool shadow_on                    = false;
bool reflection_on                = false;
bool refraction_on                = false;
bool chessboard_on                = false;
bool diffuse_reflection_on        = false;
bool antialiasing_on              = false;
bool nondisplay_on                = false;
bool poisson_on                   = false;
bool full_detail_on               = false;
// OpenGL
const int NumPoints = 6;

//----------------------------------------------------------------------------

void init()
{
  // Vertices of a square
  double ext = 1.0;
  vec4 points[NumPoints] = {
    vec4( -ext, -ext,  0, 1.0 ), //v1
    vec4(  ext, -ext,  0, 1.0 ), //v2
    vec4( -ext,  ext,  0, 1.0 ), //v3 
    vec4( -ext,  ext,  0, 1.0 ), //v3 
    vec4(  ext, -ext,  0, 1.0 ), //v2
    vec4(  ext,  ext,  0, 1.0 )  //v4
  };

  // Texture coordinates
  vec2 tex_coords[NumPoints] = {
    vec2( 0.0, 0.0 ),
    vec2( 1.0, 0.0 ),
    vec2( 0.0, 1.0 ),
    vec2( 0.0, 1.0 ),
    vec2( 1.0, 0.0 ),
    vec2( 1.0, 1.0 )
  };

  // Initialize texture objects
  GLuint texture;
  glGenTextures( 1, &texture );

  glBindTexture( GL_TEXTURE_2D, texture );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0,
    GL_RGB, GL_FLOAT, frame );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glActiveTexture( GL_TEXTURE0 );

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers( 1, &buffer );
  glBindBuffer( GL_ARRAY_BUFFER, buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(tex_coords), NULL, GL_STATIC_DRAW );
  GLintptr offset = 0;
  glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(points), points );
  offset += sizeof(points);
  glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords );

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
  glUseProgram( program );

  // set up vertex arrays
  offset = 0;
  GLuint vPosition = glGetAttribLocation( program, "vPosition" );
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
    BUFFER_OFFSET(offset) );

  offset += sizeof(points);
  GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" ); 
  glEnableVertexAttribArray( vTexCoord );
  glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
    BUFFER_OFFSET(offset) );

  glUniform1i( glGetUniformLocation(program, "texture"), 0 );

  glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

/*********************************************************
 * This is the OpenGL display function. It is called by
 * the event handler to draw the scene after you have
 * rendered the image using ray tracing. Remember that
 * the pointer to the image memory is stored in 'frame'.
 **********************************************************/
#ifdef __linux__

void display( void )
{
  glClear( GL_COLOR_BUFFER_BIT );
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_CULL_FACE);

  glDrawArrays( GL_TRIANGLES, 0, NumPoints );

  glutPostRedisplay();

  glutSwapBuffers();
}
#endif
/*********************************************************
 * This function handles keypresses
 *
 *   s - save image
 *   q - quit
 *
 *********************************************************/
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':case 'Q':
    exit(0);
    break;
  case 's':case 'S':
    save_image();

#ifdef __linux__
    glutPostRedisplay();
#endif
    
    break;
  default:
    break;
  }
}



//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
  // Parse the arguments
  if (argc < 3) {
    printf("Missing arguments ... use:\n");
    printf("./raycast [-u | -d] step_max <options>\n");
    return -1;
  }
  
  // generate random seed for global usage
  srand(time(NULL));

  int default_scene_on = strcmp(argv[1], "-d");
  int user_scene_on    = strcmp(argv[1], "-u");
  int bonus_scene_on   = strcmp(argv[1], "-b");

  step_max = atoi(argv[2]); // maximum level of recursions

  // Optional arguments
  for(int i = 3; i < argc; i++)
  {
    if      (strcmp(argv[i], "+s") == 0)    shadow_on                  = true;
    else if (strcmp(argv[i], "+l") == 0)    reflection_on              = true;
    else if (strcmp(argv[i], "+r") == 0)    refraction_on              = true;
    else if (strcmp(argv[i], "+f") == 0)    diffuse_reflection_on      = true;
    else if (strcmp(argv[i], "+c") == 0)    chessboard_on              = true;
    else if (strcmp(argv[i], "+p") == 0)    antialiasing_on            = true;
    else if (strcmp(argv[i], "+n") == 0)    nondisplay_on              = true;
    else if (strcmp(argv[i], "+b") == 0)    poisson_on                 = true;
    else if (strcmp(argv[i], "full") == 0)  full_detail_on             = true;
    else {
      std::cout << "Unknown command: " << argv[i] << std::endl;
      return -1;
    }
  }

  if ( !user_scene_on )   set_up_user_scene();
  if ( !bonus_scene_on )   set_up_bonus_scene();
  if ( !default_scene_on )   set_up_default_scene();

  // ray trace the scene now
  printf("Rendering scene using my fantastic ray tracer ...\n");
  ray_trace();

  // we want to make sure that intensity values are normalized
  histogram_normalization();
  

#ifdef __APPLE__
  // if the current platform is a macitosh, output the tracing result immediately and finish
  save_image("ray-tracer-picture/generated.bmp");
  return 0;

#else
  // use a non displayable mode to just generate image and skip OpenGL rendering
  if (nondisplay_on) {
    save_image("ray-tracer-picture/generated.bmp");
    return 0;
  }

  // Show the result in glut via texture mapping
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
  glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );
  glutCreateWindow( "Ray tracing" );
  glewInit();
  init();

  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutMainLoop();

  return 0;
#endif

  // release scene object
  release_scene();
}
