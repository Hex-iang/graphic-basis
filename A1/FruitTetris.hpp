#include "include/Angel.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <random>
using namespace std;

// ============================================================================================
// macros for tile types

#define TILE_TYPE_NUM 		3
#define TILE_COLOR_NUM		5
#define DEBUG 				1


#define VBO_GRID_POSITION	0
#define VBO_GRID_COLOR		1
#define VBO_BOARD_POSITION	2
#define VBO_BOARD_COLOR		3
#define VBO_TILE_POSITION	4
#define VBO_TILE_COLOR 		5

#define VAO_GRID			0
#define VAO_BOARD			1
#define VAO_TILE			2

// #define _IN_BOUND(x, y)	 (y <= UP_BOUND && y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)
#define _IN_BOUND(x, y)	 (y >= DOWN_BOUND && x >= LEFT_BOUND && x <= RIGHT_BOUND)

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
const vec4 white  		= vec4(1.0, 1.0, 1.0, 1.0);
const vec4 black  		= vec4(0.0, 0.0, 0.0, 1.0);

// extra colors for fruits 
const vec4 orange 		= vec4(1.0, 0.5, 0.0, 1.0); 
const vec4 red 			= vec4(1.0, 0.0, 0.0, 1.0);
const vec4 green 		= vec4(0.0, 1.0, 0.0, 1.0);
const vec4 purple		= vec4(1.0, 0.0, 1.0, 1.0);
const vec4 yellow 		= vec4(1.0, 1.0, 0.0, 1.0);

const vec4 tileColorsSet[5] = {
	orange, red, green, purple, yellow
};

// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;

// current tile
vec2 tile[4]; 	
vec4 tileColors[4];			
// An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilePos = vec2(5, 19);
// The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
int tileType 	= TILE_TYPE_L;
int rotateType 		= 0;

//board[x][y] represents whether the cell (x,y) is occupied
bool board[BOARD_WIDTH][BOARD_HEIGHT];
vec4 boardColors[BOARD_WIDTH][BOARD_HEIGHT];
// An array containing the colour of each of the 10*20*2*3 vertices that make up the board
// Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
// will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardVertexColors[BOARD_WIDTH * BOARD_HEIGHT * 3 * 2];

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
// Velocity for each timer movement and step for keyboard movement

// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
int allRotationShapeSize[TILE_TYPE_NUM] = {4, 2, 2};

vec2 allRotationsLshape[4][4] = 
	{
		{vec2(-1,-1), vec2(-1, 0), vec2( 0, 0), vec2( 1,  0)},
		{vec2( 1,-1), vec2( 0,-1), vec2( 0, 0), vec2( 0,  1)},
		{vec2( 1, 1), vec2( 1, 0), vec2( 0, 0), vec2(-1,  0)},
		{vec2(-1, 1), vec2( 0, 1), vec2( 0, 0), vec2( 0, -1)}
	};

// All rotations for S and L shapes
// ============================================================================================
vec2 allRotationsSshape[2][4] = 
	{
		{vec2(-1, -1), vec2( 0,-1), vec2(0, 0), vec2(1, 0)},
		{vec2( 1, -1), vec2( 1, 0), vec2(0, 0), vec2(0, 1)}
	};

vec2 allRotationsIshape[2][4] = 
	{
		{vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2(1, 0)},
		{vec2( 0,-2), vec2( 0,-1), vec2(0, 0), vec2(0, 1)}
	};


// =============================================================================================
// Utility functions

//-------------------------------------------------------------------------------------------------------------------
// check whether the tile is outside the bound
bool checkInBound(vec2 newPos)
{
	bool flag = true;
	for (int i = 0; i < 4; ++i)
	{
		GLfloat x = newPos.x + tile[i].x;
		GLfloat y = newPos.y + tile[i].y;

		if (!_IN_BOUND(x, y))
			flag = false;
	}

	return flag;
}

bool checkTileGridCollision( int x, int y)
{
	return ( (_IN_BOUND(x,y) && board[x][y] ) || (_IN_BOUND(x, y) && y < 0) );
}

// check whether the tile has collision with the adjacent grid object
bool checkTilesGridsCollision(vec2 newPos)
{
	bool flag = false;

	for (int i = 0; i < 4; ++i)
	{
		int x = int(newPos.x + tile[i].x);
		int y = int(newPos.y + tile[i].y);

		if ( (flag = checkTileGridCollision(x, y)) )
			break;
	}
	return flag;
}

// Try rotates the current tile, return true if the tile is successfully rotated, 
// And return false if there are some issues related to rotation 
bool testRotation(vec2 currentTilePos)
{
	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
			( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

	// First test if the rotated version are in the boundary
	for (int i = 0; i < 4; ++i)
	{	
		int nextShape = (rotateType + 1) % allRotationShapeSize[tileType];
		int x = int(currentTilePos.x + pAllRotationShape[nextShape][i].x);
		int y = int(currentTilePos.y + pAllRotationShape[nextShape][i].y);

#ifdef DEBUG
		cout << "testRotation() - [Ratated tile#"<< i <<" x:" << x << ", y:" << y << "]" << endl;
#endif

		if ( !_IN_BOUND(x,y) || checkTileGridCollision(x, y) )
			return false;
	}

	return true;
}


// Get Tile Bound 
//-------------------------------------------------------------------------------------------------------------------
struct TileBound{
	GLfloat left, right, up, down;
	TileBound(int _left, int _right, int _up, int _down):left(_left), right(_right), up(_up), down(_down){}
};

const TileBound getTileBound( vec2 * pTile)
{
	TileBound tileBound(0, 0, 0, 0);
	for (int i = 0; i < 4; i++){
		tileBound.left 	= min(pTile[i].x, tileBound.left);
		tileBound.right = max(pTile[i].x, tileBound.right);
		tileBound.down 	= min(pTile[i].y, tileBound.down);
		tileBound.up    = max(pTile[i].y, tileBound.up);
	}

	return tileBound;
}

// Fill Tile with random colors
void fillTileWithRandomColor()
{
	// Fill up the four tiles with different colors
	for (int i = 0; i < 4; i++){
		vec4 newTileColor = tileColorsSet[rand() % TILE_COLOR_NUM];
		// Set up the current tile with new color
		tileColors[i] = newTileColor;
	}
}


void genColorVertexFromTileColor(vec4 * pPointColor, vec4 _color)
{
	for (int j = 0; j < 6; j++){
		*(pPointColor + j) = _color;
	}
}

// Generate color vertexs of 4 tile structure from global variable tileColors
void genColorVertexsFromTileColors(vec4 * pPointsColors)
{
	for (int i = 0; i < 4; i++){
		genColorVertexFromTileColor( &pPointsColors[i*6], tileColors[i]);
	}
}

void genBoardVertexColorFromBoardColor(int x ,int y, vec4 _color)
{
	for (int i = 0; i < 6; ++i)
	{
		boardVertexColors[(x + y*BOARD_WIDTH)*6 + i] = _color;
	}
}

void genBoardVertexColorsFromBoardColors()
{
	for (int i = 0; i < BOARD_WIDTH; i++){
		for (int j = 0; j < BOARD_HEIGHT; ++j)
		{
			genBoardVertexColorFromBoardColor(i, j, boardColors[i][j]);
		}
	}
}

// void genColorVertexsFromTileColors(vec4 * pPointsColors)
// {
// 	for (int i = 0; i < 4; i++){
// 		for (int j = 0; j < 6; j++){
// 			*(pPointsColors + i*6 + j) = tileColors[i];
// 		}	
// 	}
// }