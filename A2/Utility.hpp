#pragma once
#include "FruitTetris.hpp"

// =============================================================================================
// Utility functions


//-------------------------------------------------------------------------------------------------------------------
// Check if the a tile has collision with board objects
bool checkTileGridCollision( int x, int y)
{
	return ( (_IN_BOUND(x,y) && board[x][y] ) || (y < 0) );
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


		if ( !_IN_BOUND(x,y) || checkTileGridCollision(x, y) )
			return false;
	}

	return true;
}


//-------------------------------------------------------------------------------------------------------------------
// Generate color vertexs of 4 tile structure from global variable tileColors
void genColorVertexFromTileColor(color4 * pPointColor, color4 _color)
{
	for (int j = 0; j < TILE_VERTEX_NUM; j++){
		*(pPointColor + j) = _color;
	}
}


//-------------------------------------------------------------------------------------------------------------------
void genBoardVertexColorFromBoardColor(int x ,int y, color4 _color)
{
	for (int i = 0; i < TILE_VERTEX_NUM; ++i)
	{
		boardVertexColors[(x + y*BOARD_WIDTH)*TILE_VERTEX_NUM + i] = _color;
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
color4 genRandomColor()
{
	return tileColorsSet[rand() % TILE_COLOR_NUM];
}

//-------------------------------------------------------------------------------------------------------------------
// Generate points for a quad
void quad( point4 * pPoints, point4 p1, point4 p2, point4 p3, point4 p4)
{
    pPoints[0] = p1;
    pPoints[1] = p2;
    pPoints[2] = p3;
    pPoints[3] = p1;
    pPoints[4] = p3;
    pPoints[5] = p4;
}
//-------------------------------------------------------------------------------------------------------------------
// Generate line grid for a quad
void quadLine( point4 * pPoints, point4 p1, point4 p2, point4 p3, point4 p4)
{
    pPoints[0] = p1;
    pPoints[1] = p2;

    pPoints[2] = p2;
    pPoints[3] = p3;

    pPoints[4] = p3;
    pPoints[5] = p4;

    pPoints[6] = p4;
    pPoints[7] = p1;
}
