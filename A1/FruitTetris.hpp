# pragma once
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include "unistd.h"
#include "include/Angel.h"
#include <vector>
#include <iomanip>
using namespace std;

// ============================================================================================
// macros for tile types

#define TILE_TYPE_NUM 		3
#define TILE_COLOR_NUM		5
#define DEBUG 				1


#define VBO_GRID_POSITION		0
#define VBO_GRID_COLOR			1
#define VBO_BOARD_POSITION		2
#define VBO_BOARD_COLOR			3
#define VBO_TILE_POSITION		4
#define VBO_TILE_COLOR 			5

// #define VBO_DROPTILE_COLOR 		6
// #define VBO_DROPTILE_POSITION 	7



#define VAO_GRID			0
#define VAO_BOARD			1
#define VAO_TILE			2
// #define VAO_DROPTILE		3

// #define _IN_BOUND(x, y)	 (y <= UP_BOUND && y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)
#define _IN_BOUND(x, y)	 (y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)

#define _color4_equal(a,b) (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w) 

#define _MATCH_COLOR(color) ( _color4_equal(color, black) 	? "black" 	: \
							( _color4_equal(color, orange)	? "orange" 	: \
							( _color4_equal(color, red)		? "red" 	: \
							( _color4_equal(color, green)	? "green" 	: \
							( _color4_equal(color, purple)	? "purple"	: \
							( _color4_equal(color, yellow)	? "yellow"	: \
							( _color4_equal(color, white)	? "white"	: "Unknown color" \
								)))))))


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//  constant variable
// ============================================================================================

const int TILE_TYPE_L 	= 0;
const int TILE_TYPE_S 	= 1;
const int TILE_TYPE_I 	= 2;

// const for board boundaries 
const int LEFT_BOUND 	= 0;
const int RIGHT_BOUND 	= 9;
const int DOWN_BOUND 	= 0;
const int UP_BOUND  	= 19;


// const for board size
const int BOARD_WIDTH 	= 10;
const int BOARD_HEIGHT	= 20;

// colors
const color4 white  		= color4(1.0, 1.0, 1.0, 1.0);
const color4 black  		= color4(0.0, 0.0, 0.0, 1.0);

// extra colors for fruits 
const color4 orange 		= color4(1.0, 0.5, 0.0, 1.0); 
const color4 red 			= color4(1.0, 0.0, 0.0, 1.0);
const color4 green 			= color4(0.0, 1.0, 0.0, 1.0);
const color4 purple			= color4(1.0, 0.0, 1.0, 1.0);
const color4 yellow 		= color4(1.0, 1.0, 0.0, 1.0);

const color4 tileColorsSet[5] = {
	orange, red, green, purple, yellow
};

// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;

class Tile{
public:
	Tile(vec2 _pos, color4 _color) : Position(_pos), Color(_color){ }
	Tile(const Tile & _tile ) { this->Position = _tile.Position; this->Color = _tile.Color; }

	bool operator== (const Tile &other) const {
		return _color4_equal(this->Color, other.Color) && this->Position.x == other.Position.x && this->Position.y == other.Position.y;
  	}

	bool operator!=(const Tile &other) const {
		return !(*this == other);
	}

	vec2 Position;
	color4 Color;
};

vec2 tile[4];
color4 tileColors[4];

vector< vector<Tile> > dropTiles;

// An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilePos = vec2(5, 19);
// The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
int tileType 		= TILE_TYPE_L;
int rotateType 		= 0;

//board[x][y] represents whether the cell (x,y) is occupied
bool board[BOARD_WIDTH][BOARD_HEIGHT];
color4 boardColors[BOARD_WIDTH][BOARD_HEIGHT];
color4 boardVertexColors[BOARD_WIDTH * BOARD_HEIGHT * 3 * 2];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[3]; 
// One VAO for each object: 0. the grid 1. the board 2. the current piece

GLuint vboIDs[6]; 
// Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

GLfloat velocity 	= -1.0;
GLfloat step 		=  1.0;


// Game control signal
bool ifPause 		= false;
bool ifGameStop 	= false;

// Glut time control float
GLfloat deltaTime 	= 0.0f;		// Time between current frame and last frame
GLfloat lastFrame 	= 0.0f;  	// Time of last frame

// Velocity for each timer movement and step for keyboard movement

// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
int allRotationShapeSize[TILE_TYPE_NUM] = {4, 4, 4};

vec2 allRotationsLshape[4][4] = 
	{
		{vec2(-1,-1), vec2(-1, 0), vec2( 0, 0), vec2( 1,  0)},
		{vec2( 1,-1), vec2( 0,-1), vec2( 0, 0), vec2( 0,  1)},
		{vec2( 1, 1), vec2( 1, 0), vec2( 0, 0), vec2(-1,  0)},
		{vec2(-1, 1), vec2( 0, 1), vec2( 0, 0), vec2( 0, -1)}
	};

// All rotations for S and L shapes
// ============================================================================================
vec2 allRotationsSshape[4][4] = 
	{
		{vec2(-1, -1), vec2( 0,-1), vec2(0, 0), vec2( 1, 0)},
		{vec2( 1, -1), vec2( 1, 0), vec2(0, 0), vec2( 0, 1)},
		{vec2( 1,  1), vec2( 0, 1), vec2(0, 0), vec2(-1, 0)},
		{vec2(-1,  1), vec2(-1, 0), vec2(0, 0), vec2( 0,-1)}
	};

vec2 allRotationsIshape[4][4] = 
	{
		{vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2( 1, 0)},
		{vec2( 0,-2), vec2( 0,-1), vec2(0, 0), vec2( 0, 1)},
		{vec2( 2, 0), vec2( 1, 0), vec2(0, 0), vec2(-1, 0)},
		{vec2( 0, 2), vec2( 0, 1), vec2(0, 0), vec2( 0,-1)}

	};


// Get Tile Bound 
//-------------------------------------------------------------------------------------------------------------------
struct TileBound{
	GLfloat left, right, up, down;
	TileBound(int _left, int _right, int _up, int _down):left(_left), right(_right), up(_up), down(_down){}
};

// ===========================================================================================
// Utility function 
void genBoardVertexColorFromBoardColor(int x ,int y, color4 _color);
void genBoardVertexColorsFromBoardColors();

void genColorVertexFromTileColor(color4 * pPointColor, color4 _color);
void genColorVertexsFromTileColors(color4 * pPointsColors);

void fillTileWithRandomColor();
const TileBound getTileBound( vec2 * pTile);

bool checkInBound(vec2 newPos);
bool checkTileGridCollision( int x, int y);
bool checkTilesGridsCollision(vec2 newPos);
bool testRotation(vec2 currentTilePos);

// ===========================================================================================
// Function Declaration
void updateBoard();
void updateTile();
void newTile();
bool rotateTile();
void shiftTileColor();
void unsetTile();
bool checkTileSupport();

void moveDownRow(int startRow);
void moveDownRows(bool eliminatedRows[]);
void eliminateFullRow(int row);
bool checkFullRow(int row);
void setTile();
bool moveTile(vec2 direction);
void moveDownTileToEnd();

void initGrid();
void initBoard();
void initCurrentTile();
void init();

void processDisplay();
void processReshape(GLsizei w, GLsizei h);
void processSpecialKey(int key, int x, int y);
void processKeyboard(unsigned char key, int x, int y);
void processTimer(int val);
void processIdle();

void newGame();
void pauseResumeGame();
bool tryStopGame();
void restartGame();
bool checkEndOfGame();

bool matchFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void printBoolBoardSizeArray(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void cleanUpEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
void moveDownFruitTilesCols(bool eliminatedFruitTiles[][BOARD_HEIGHT]);
bool eliminateFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT]);

bool checkTileSupport();
bool checkFruitMatchAndEliminate();
void checkFullRowsAndEliminate();
bool fallTiles();
void updateDropTile();
void setDropTile(vector<Tile> &_tile);
bool searchConnectToBottom(vec2 vertex);
void addTileToDropTiles(Tile _newDropTile);