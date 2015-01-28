/*
CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code

- This is ONLY a skeleton code showing:
How to use multiple buffers to store different objects
An efficient scheme to represent the grids and blocks

- Compile and Run:
Type make in terminal, then type ./FruitTetris

This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission
by Rui Ma (ruim@sfu.ca) on 2014-03-04. 

Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/

#include "include/Angel.h"
#include <cstdlib>
#include <iostream>

// Personal included library
#include <ctime>
#include <algorithm>
#include <random>
using namespace std;

// Macros defined by Hexiang
// ============================================================================================
// macros for tile types

#define TILE_TYPE_NUM 	3
#define TILE_COLOR_NUM	5
#define DEBUG 			1

//  constant variable
// ============================================================================================


const int TILE_TYPE_L = 0;
const int TILE_TYPE_S = 1;
const int TILE_TYPE_I = 2;

// const for board boundaries 
const int LEFT_BOUND 	= 0;
const int RIGHT_BOUND 	= 9;
const int DOWN_BOUND 	= 0;
const int UP_BOUND  	= 19;


// const for board size
const int BOARD_WIDTH 	= 10;
const int BOARD_HEIGHT	= 20;

// colors
const vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 black  = vec4(0.0, 0.0, 0.0, 1.0);

// extra colors for fruits 
const vec4 orange 	= vec4(1.0, 0.5, 0.0, 1.0); 
const vec4 red 		= vec4(1.0, 0.0, 0.0, 1.0);
const vec4 green 	= vec4(0.0, 1.0, 0.0, 1.0);
const vec4 purple	= vec4(1.0, 0.0, 1.0, 1.0);
const vec4 yellow 	= vec4(1.0, 1.0, 0.0, 1.0);

const vec4 tileColors[5] = {
	orange, red, green, purple, yellow
};

// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;

// current tile
vec2 tile[4]; 				
// An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilePos = vec2(5, 19);
// The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
int tileType 	= TILE_TYPE_L;
int rotateType 		= 0;

// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
int allRotationShapeSize[TILE_TYPE_NUM] = {4, 2, 2};

vec2 allRotationsLshape[4][4] = 
	{
		{vec2( 0, 0), vec2(-1, 0), vec2(1, 0), vec2(-1, -1)},
		{vec2( 0, 1), vec2( 0, 0), vec2(0,-1), vec2( 1, -1)},
		{vec2( 1, 1), vec2(-1, 0), vec2(0, 0), vec2( 1,  0)},
		{vec2(-1, 1), vec2( 0, 1), vec2(0, 0), vec2( 0, -1)}
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

//board[x][y] represents whether the cell (x,y) is occupied
bool board[BOARD_WIDTH][BOARD_HEIGHT];

// An array containing the colour of each of the 10*20*2*3 vertices that make up the board
// Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
// will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[BOARD_WIDTH * BOARD_HEIGHT * 3 * 2];

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

// Global structures needed by Hexiang 
// ============================================================================================
GLfloat step = 1.0;


//-------------------------------------------------------------------------------------------------------------------

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updateTile()
{
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 

	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilePos.x + tile[i].x; 
		GLfloat y = tilePos.y + tile[i].y;


		// Contraints that make the tile outside the UP_BOUND of board invisible
		// ==============================================================================
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		if ( y > UP_BOUND ) continue;

		// Two points are used by two triangles each
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
}

// Get Tile Bound 
//-------------------------------------------------------------------------------------------------------------------
struct Bound{
	GLfloat left, right, up, down;
	Bound(int _left, int _right, int _up, int _down):left(_left), right(_right), up(_up), down(_down){}
};

const Bound getTileBound( vec2 * pTile)
{
	Bound tileBound(0, 0, 0, 0);
	for (int i = 0; i < 4; i++){
		tileBound.left 	= min(pTile[i].x, tileBound.left);
		tileBound.right = max(pTile[i].x, tileBound.right);
		tileBound.down 	= min(pTile[i].y, tileBound.down);
		tileBound.up    = max(pTile[i].y, tileBound.up);
	}

	return tileBound;
}

// Fill Tile with random colors
void fillupTile( vec4 * pTileColor ){
	// Fill up the four tiles with different colors
	for (int i = 0; i < 4; i++){
		vec4 tileColor = tileColors[rand() % TILE_COLOR_NUM];
		int j = 6;
		while (j--){
			*pTileColor = tileColor;
			pTileColor ++;
		}
	}
}

// Called at the start of play and every time a tile is placed
void newTile()
{
	// First generate a random tile

	tileType = rand() % TILE_TYPE_NUM;
	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
				( (tileType == TILE_TYPE_I)? allRotationsIshape : allRotationsSshape );
	rotateType = rand() % ( allRotationShapeSize[tileType] );


#ifdef DEBUG
	cout << "newTile() [Generate Randomness] - TileType: " << tileType << ", RotateType: " << rotateType 
		<< ", RotationShape: " << allRotationShapeSize[tileType] << endl;
#endif

	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++){
		tile[i] = pAllRotationShape[rotateType][i];

#ifdef DEBUG
		cout << "newTile() [Tile init] - Tile#" << i << " - " << tile[i].x << "," << tile[i].y << endl;
#endif

	}

	Bound tileBound = getTileBound(tile);
	int coverage = ( RIGHT_BOUND - int(tileBound.right) + int(tileBound.left) + 1);

	int hpos = rand() % coverage - int(tileBound.left);
	int vpos = 19 - tileBound.up;
	tilePos = vec2(hpos , vpos);
	// Put the tile at the top of the board
	
#ifdef DEBUG
	cout << "newTile() [Get Tile Boundary] - Bound(" << tileBound.left << ", " << tileBound.right
		<<", " << tileBound.down << ", " << tileBound.up << ")" << endl;
	cout << "newTile() [Boundaries] - Right: " << (RIGHT_BOUND - int(tileBound.right)) 
		<< ", Left: " << (- int(tileBound.left)) << ", Coverage: " << coverage << endl;
	cout << "newTile() [New Tile Position] - x = " << hpos << ", y = " << vpos << endl;
#endif

	// Get the 4 pieces of the new tile
	updateTile(); 

	// Update the color VBO of current tile
	vec4 newcolours[24];

	// You should randomlize the color
	fillupTile(newcolours);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); 
	// Bind the VBO containing current tile vertex colours
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); 
	// Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[64]; 
	// Array containing the 64 points of the 32 total lines ( 21 horizontal line + 11 vertical line ) to be later put in the VBO
	vec4 gridcolours[64]; 
	// One colour per vertex
	
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] 	= vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] 	 = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}
	// Make all grid lines white
	for (int i = 0; i < 64; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]);
	// Bind the first VAO
	glGenBuffers(2, vboIDs); 
	// Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); 
	// Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridpoints), gridpoints, GL_STATIC_DRAW); 
	// Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
	glEnableVertexAttribArray(vPosition); 
	// Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); 
	// Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridcolours), gridcolours, GL_STATIC_DRAW); 
	// Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vColor); 
	// Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			
			// Two points are reused
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	// Bind the Current vao to Tils's array
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vColor);
}

void init()
{
	// initialize random generator with seed 
	srand(time(NULL));

	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor 	  = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

	// Game initialization
	newTile(); 	
	// create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------

// Try rotates the current tile, return true if the tile is successfully rotated, 
// And return false if there are some issues related to rotation 
// bool rotateTile()
// {
// 	rotateType = (rotateType + 1) % allRotationShapeSize[tileType];
// 	vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
// 				( (tileType == TILE_TYPE_I)? allRotationsIshape : allRotationsSshape );

// 	// Update the geometry VBO of current tile
// 	for (int i = 0; i < 4; i++){
// 		tile[i] = pAllRotationShape[rotateType][i];

// #ifdef DEBUG
// 	cout << "rotateTile() [Change Rotation] - Tile#" << i << " - " << tile[i].x << "," << tile[i].y << endl;
// #endif

// 	}
// #ifdef DEBUG
// 	cout << "rotateTile() [Change Rotation] - TileType: " << tileType << ", RotateType: " << rotateType 
// 		<< ", RotationShape: " << allRotationShapeSize[tileType] << endl;
// #endif
// 	updateTile();
// }

// Different from the first version, this rotation function is going to do 
// ratation transformation to the tile instead of rendering now position of time
bool rotateTile()
{

}

//-------------------------------------------------------------------------------------------------------------------

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
void checkFullRow(int row)
{

}

//-------------------------------------------------------------------------------------------------------------------

// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void setTile()
{
	checkFullRow(1);	
}

//-------------------------------------------------------------------------------------------------------------------

// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool moveTile(vec2 direction)
{
	Bound tileBound = getTileBound(tile);
	vec2 futureTilePos = tilePos + direction;

	return false;
}
//-------------------------------------------------------------------------------------------------------------------

// Starts the game over - empties the board, creates new tiles, resets line counters
void restartGame()
{
	cout << "Restart Game..." << endl;
	initGrid();
	initBoard();
	initCurrentTile();
	newTile(); 
}

//-------------------------------------------------------------------------------------------------------------------

// Draws the game
void processDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)


	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void processReshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void processSpecialKey(int key, int x, int y)
{
	// Initial attempt for special key event
	vec2 displacement = vec2(0,0);

	switch(key) {
	case GLUT_KEY_UP :
		displacement += vec2(0, step);
		break;
	case GLUT_KEY_DOWN :
		displacement -= vec2(0, step);
		break;
	case GLUT_KEY_LEFT :
		displacement -= vec2(step, 0);
		break;
	case GLUT_KEY_RIGHT:
		displacement += vec2(step, 0);
		break;
	}

	moveTile(displacement);

#ifdef DEBUG
	cout << "Curret position - " << tilePos.x << ", " << tilePos.y << endl;
#endif

	glutPostRedisplay();

}

//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void processKeyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' key restarts the game
			restartGame();
			break;
		case 's':
			rotateTile();
			break;
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void processIdle(void)
{
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	// initizlize grew
	glewInit();

	// init grid, broad, tile
	init();

	// Callback functions
	glutDisplayFunc(processDisplay);
	glutReshapeFunc(processReshape);
	glutSpecialFunc(processSpecialKey);
	glutKeyboardFunc(processKeyboard);
	glutIdleFunc(processIdle);

	glutMainLoop(); // Start main loop
	return 0;
}
