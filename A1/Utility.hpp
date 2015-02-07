#pragma once
#include "FruitTetris.hpp"

// =============================================================================================
// Utility functions

//-------------------------------------------------------------------------------------------------------------------
// check whether a tile is outside the bound
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

//-------------------------------------------------------------------------------------------------------------------
// Check if the a tile has collision with board objects
bool checkTileGridCollision( int x, int y)
{
	return ( (_IN_BOUND(x,y) && board[x][y] ) || (_IN_BOUND(x, y) && y < 0) );
}

//-------------------------------------------------------------------------------------------------------------------
// check whether the tile has collision with the adjacent grid object
bool checkTilesGridsCollision(vec2 newPos)
{
	bool flag = false;

	for (int i = 0; i < 4; ++i)
	{
		int x = int(newPos.x + tile[i].x);
		int y = int(newPos.y + tile[i].y);

		// if the tile is not black and it have collision to some other tiles
		if ( (flag = ( checkTileGridCollision(x, y) && !_color4_equal(tileColors[i], black)) ) )
			break;
	}
	return flag;
}

//-------------------------------------------------------------------------------------------------------------------
// Test if the rotation condition is met
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

//-------------------------------------------------------------------------------------------------------------------
// Generate color vertexs of 4 tile structure from global variable tileColors
void genColorVertexFromTileColor(color4 * pPointColor, color4 _color)
{
	for (int j = 0; j < 6; j++){
		*(pPointColor + j) = _color;
	}
}


void genColorVertexsFromTileColors(color4 * pPointsColors)
{
	for (int i = 0; i < 4; i++){
		genColorVertexFromTileColor( &pPointsColors[i*6], tileColors[i]);
	}
}

//-------------------------------------------------------------------------------------------------------------------
void genBoardVertexColorFromBoardColor(int x ,int y, color4 _color)
{
	for (int i = 0; i < 6; ++i)
	{
		boardVertexColors[(x + y*BOARD_WIDTH)*6 + i] = _color;
	}
}

//-------------------------------------------------------------------------------------------------------------------
void genBoardVertexColorsFromBoardColors()
{
	for (int i = 0; i < BOARD_WIDTH; i++){
		for (int j = 0; j < BOARD_HEIGHT; ++j)
		{
			genBoardVertexColorFromBoardColor(i, j, boardColors[i][j]);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
// The the tile boundary 
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

//-------------------------------------------------------------------------------------------------------------------
// Fill Tile with random colors
void fillTileWithRandomColor()
{
	// Fill up the four tiles with different colors
	for (int i = 0; i < 4; i++){
		color4 newTileColor = tileColorsSet[rand() % TILE_COLOR_NUM];
		// Set up the current tile with new color
		tileColors[i] = newTileColor;
	}
}
