// Includes all source codes about Tile operation 
#pragma once
#include "FruitTetris.hpp"

//-------------------------------------------------------------------------------------------------------------------
// 
void updateTile()
{
    // For each of the 4 'cells' of the tile,
    for (int i = 0; i < 4; i++) 
    {
        // Calculate the grid coordinates of the cell
        GLfloat x = tilePos.x + tile[i].x; 
        GLfloat y = tilePos.y + tile[i].y;


        // Contraints that make the tile outside the UP_BOUND of board invisible
        // ==============================================================================
        point4 p1 = point4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
        point4 p2 = point4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
        point4 p3 = point4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
        point4 p4 = point4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

        // if ( y > UP_BOUND ) continue;

        // Two points are used by two triangles each
        point4 newPoints[6] = {p1, p2, p3, p2, p3, p4}; 

        // Put new data in the VBO
        point4 pointsColors[6];

        genColorVertexFromTileColor(pointsColors, tileColors[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(color4), 6*sizeof(color4), pointsColors);


        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(point4), 6*sizeof(point4), newPoints);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


//-------------------------------------------------------------------------------------------------------------------
// Called at the start of play and every time a tile is placed
void newTile()
{
    // First generate a random tile

    tileType = rand() % TILE_TYPE_NUM;
    vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
            ( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

    rotateType = rand() % ( allRotationShapeSize[tileType] );


#ifdef DEBUG
    cout << "newTile() - [Generate Randomness] TileType: " << tileType << ", RotateType: " << rotateType 
        << ", RotationShape: " << allRotationShapeSize[tileType] << endl;
#endif

    // Update the geometry VBO of current tile
    for (int i = 0; i < 4; i++){
        tile[i] = pAllRotationShape[rotateType][i];
    }

    TileBound tileBound = getTileBound(tile);
    int coverage = ( RIGHT_BOUND - int(tileBound.right) + int(tileBound.left) + 1);

    int hpos = rand() % coverage - int(tileBound.left);
    int vpos = UP_BOUND - tileBound.up;
    tilePos = vec2(hpos , vpos);
    // Put the tile at the top of the board
    
#ifdef DEBUG
    cout << "newTile() - [Get Tile Boundary] Bound(" << tileBound.left << ", " << tileBound.right
        <<", " << tileBound.down << ", " << tileBound.up << ")" << endl;
    cout << "newTile() - [Boundaries] Right: " << (RIGHT_BOUND - int(tileBound.right)) 
        << ", Left: " << (- int(tileBound.left)) << ", Coverage: " << coverage << endl;
    cout << "newTile() - [New Tile Position] x = " << hpos << ", y = " << vpos << endl;
#endif

    // You should randomlize the color in structure tileColors
    fillTileWithRandomColor();

    // Call update function to draw the tile with its position as well as its color
    updateTile(); 

    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Rotate the current tile 
bool rotateTile()
{
    vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
            ( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

    // First test if the rotated version are in the boundary
    if (!testRotation(tilePos))
    {
        // Add logic to rescue a tile if it could be rotate with a unit movement.
        if ( testRotation(tilePos + vec2(1, 0)) )
        {
            cout << "rotateTile() - [Re-Rotate in bound, case 1]"<< endl;
            tilePos += vec2(1, 0);
        }
        else if ( testRotation(tilePos + vec2(-1, 0)) )
        {
            cout << "rotateTile() - [Re-Rotate in bound, case 2]"<< endl;
            tilePos += vec2(-1, 0);
        }
        else if ( testRotation(tilePos + vec2( 0, 1)) )
        {
            cout << "rotateTile() - [Re-Rotate in bound, case 3]"<< endl;
            tilePos += vec2(0, 1);
        }
        else if ( testRotation(tilePos + vec2( 2, 0)) )
        {
            cout << "rotateTile() - [Re-Rotate in bound, case 4]"<< endl;
            tilePos += vec2(2, 0);          
        }
        else if ( testRotation(tilePos + vec2(-2, 0)))
        {
            cout << "rotateTile() - [Re-Rotate in bound, case 5]"<< endl;
            tilePos += vec2(-2, 0);
        }
        else if ( testRotation(tilePos + vec2( 0, 2)) )
        {
            cout << "rotateTile() - [Re-Rotate in bound, case 6]"<< endl;
            tilePos += vec2(0, 2);
        }
        else{

#ifdef DEBUG
            cout << "rotateTile() [Failure Rotation]"<< endl;
#endif
            updateTile();
            return false;
        }
    }

    // Testing succeed, do actual rotation

    rotateType = (rotateType + 1) % allRotationShapeSize[tileType];
    // Update the geometry VBO of current tile
    for (int i = 0; i < 4; i++){
        tile[i] = pAllRotationShape[rotateType][i];

#ifdef DEBUG
        cout << "rotateTile() - [Change Rotation] Tile#" << i << " - " << tilePos.x + tile[i].x << "," << tilePos.y + tile[i].y << endl;
#endif
    }


#ifdef DEBUG
    cout << "rotateTile() - [Change Rotation] TileType: " << tileType << ", RotateType: " << rotateType 
        << ", RotationShape: " << allRotationShapeSize[tileType] << endl;
#endif
    updateTile();
    return true;
}

//-------------------------------------------------------------------------------------------------------------------
// shift Teris color 
void shiftTileColor()
{
    color4 oldColor = tileColors[3];

    tileColors[3] = tileColors[2];
    tileColors[2] = tileColors[1];
    tileColors[1] = tileColors[0];
    tileColors[0] = oldColor;
    updateTile();
}


//-------------------------------------------------------------------------------------------------------------------
// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool moveTile(vec2 direction)
{
    if(ifGameStop) return false;

    bool flag = true;
    // Bound tileBound = getTileBound(tile);
    // vec2 futureTilePos = tilePos + direction;
    if( checkInBound(tilePos + direction) )
    {
        if ( checkTilesGridsCollision(tilePos + direction) )
        {
#ifdef DEBUG
    cout << "moveTile() - [Collision detected] " << endl;
#endif
            flag = false;
        }
        else
        {
            // If there is no collision in the future position
            tilePos += direction;
        }
    }
    else{

#ifdef DEBUG
    cout << "moveTile() - [Out of Bound] " << endl;
#endif      
        flag = false;
    }

#ifdef DEBUG
    cout << "moveTile() - Horizontal Movement: " << direction.x << ", Vertical Movement: " << direction.y << endl;
    cout << "moveTile() - Curret position - " << tilePos.x << ", " << tilePos.y << endl;
#endif

    if( flag ) updateTile();

    return flag;

}

//-------------------------------------------------------------------------------------------------------------------
// Moving the current tile directly to the current bottom 
void moveDownTileToEnd()
{
    if(ifGameStop) return;

#ifdef DEBUG
    cout << "moveDownTileToEnd() - [Straight down.] from ["<< tilePos.x <<"][" << tilePos.y << "]"<< endl;
#endif
    bool flag = false; 
    for (int i = 0; i <= BOARD_HEIGHT; ++i)
    {
        if( !moveTile(vec2(0.0, -1.0)) ){
            flag = true;
            break;
        }
    }

    if (flag == true)
    {
        
#ifdef DEBUG
    cout << "moveDownTileToEnd() - [Straight down.] to ["<< tilePos.x <<"][" << tilePos.y << "]"<< endl;
#endif
        setTile();
        checkFullRowsAndEliminate();
        checkFruitMatchAndEliminate();
        newTile();
        updateTile();

    }
#ifdef DEBUG
    else
    {   
        cout << "moveDownTileToEnd() - [Error] " << endl;
        return;
    }
#endif

}


//-------------------------------------------------------------------------------------------------------------------
// check if  a row is full
void moveDownRow(int startRow)
{
    for (int row = startRow; row < BOARD_HEIGHT; ++row)
    {
        bool isBlackRow = false;

        // If the row to move down is the top row, add a black row
        if (row == UP_BOUND)
            isBlackRow = true;

        if (isBlackRow)
        {
            for (int col = 0; col < BOARD_WIDTH; ++col)
            {
                board[col][row] = false;
                boardColors[col][row] = black;
            }               
        }
        else
        {
            for (int col = 0; col < BOARD_WIDTH; ++col)
            {
                board[col][row] = board[col][row + 1];
                boardColors[col][row] = boardColors[col][row + 1];
            }           
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// check if couple of rows are full
void moveDownRows(bool eliminatedRows[])
{
    for (int i = UP_BOUND; i >= 0; --i)
    {
        if (eliminatedRows[i])
        {
            moveDownRow(i);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Eliminate a full row by setting the board to be false and its color to be black
void eliminateFullRow(int row)
{
    for (int i = 0; i < BOARD_WIDTH; ++i)
    {
        board[i][row] = false;
        boardColors[i][row] = black;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Check if one row is full
bool checkFullRow(int row)
{
    bool isRowFull = true;
    for (int i = 0; i < BOARD_WIDTH; ++i){
        isRowFull = isRowFull && board[i][row];
    }

#ifdef DEBUG
    cout << "checkFullRow() - [Row Status] : " << endl;
    for (int i = 0; i < BOARD_WIDTH; ++i)
    {
        cout << "[" << (board[i][row]? 'T' : 'F') << "],"; 
    }
    cout << endl;
    if ( isRowFull )
    {
        cout << "checkFullRow() - [A full row detected] : Row[" << row << "]" << endl;
    }
#endif


    return isRowFull;
}

//-------------------------------------------------------------------------------------------------------------------
// Wrapper for doing Row checking and eliminaton
void checkFullRowsAndEliminate()
{
    // Perform row checking..
    TileBound tileBound = getTileBound(tile);
    int start = tileBound.down + tilePos.y;
    int end = tileBound.up + tilePos.y;
    bool eliminatedRows[BOARD_HEIGHT] = { false };

    for (int i = start; i < end + 1; ++i)
    {       
        if( checkFullRow(i) ){
            eliminatedRows[i] = true;
            eliminateFullRow(i);
        }
    }

    moveDownRows(eliminatedRows);
    genBoardVertexColorsFromBoardColors();
    updateBoard();
    usleep(500);
}

//-------------------------------------------------------------------------------------------------------------------
// match fruits when a tile is set and see if there is a elimination 
bool matchFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
    // Basically a board-global matching
    bool flag = false;

    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int i = LEFT_BOUND; i < RIGHT_BOUND - 1; ++i)
        {
        
            if( _color4_equal(boardColors[i + 0][y], boardColors[i + 1][y]) && 
                _color4_equal(boardColors[i + 1][y], boardColors[i + 2][y]) &&
                !_color4_equal(boardColors[i][y], black)                    )
            {

#ifdef DEBUG 
                cout << "matchFruitTiles() - [row fruit tiles detected]" << endl;
                cout << "matchFruitTiles() - tile["<< i + 0 << "][" << y <<"] = " << _MATCH_COLOR(boardColors[i + 0][y]) << endl;
                cout << "matchFruitTiles() - tile["<< i + 1 << "][" << y <<"] = " << _MATCH_COLOR(boardColors[i + 1][y]) << endl;
                cout << "matchFruitTiles() - tile["<< i + 2 << "][" << y <<"] = " << _MATCH_COLOR(boardColors[i + 2][y]) << endl;
#endif
                eliminatedFruitTiles[(i + 0)][y] = true;
                eliminatedFruitTiles[(i + 1)][y] = true;
                eliminatedFruitTiles[(i + 2)][y] = true;

                flag = true;
            }
        }
    }

    for (int x = 0; x < BOARD_WIDTH; ++x)
    {   
        for (int i = DOWN_BOUND; i < UP_BOUND - 1; ++i)
        {       
            if( _color4_equal(boardColors[x][i + 0], boardColors[x][i + 1]) && 
                _color4_equal(boardColors[x][i + 1], boardColors[x][i + 2]) &&
                !_color4_equal(boardColors[x][i], black)                )
            {

#ifdef DEBUG 
                cout << "matchFruitTiles() - [column fruit tiles detected]" << endl;
                cout << "matchFruitTiles() - tile["<< x << "][" << i + 0 <<"] = " << _MATCH_COLOR(boardColors[x][i + 0]) << endl;
                cout << "matchFruitTiles() - tile["<< x << "][" << i + 1 <<"] = " << _MATCH_COLOR(boardColors[x][i + 1]) << endl;
                cout << "matchFruitTiles() - tile["<< x << "][" << i + 2 <<"] = " << _MATCH_COLOR(boardColors[x][i + 2]) << endl;
#endif
                eliminatedFruitTiles[x][i + 0] = true;
                eliminatedFruitTiles[x][i + 1] = true;
                eliminatedFruitTiles[x][i + 2] = true;

                flag = true;
            }
        }

    }
    
    // Change the tile color back to black
    for (int i = 0; i < 4; ++i)
    {
        int x = int(tilePos.x + tile[i].x);
        int y = int(tilePos.y + tile[i].y);
        if (eliminatedFruitTiles[x][y] == true)
            tileColors[i]  = black;
    }

    printEliminationTiles(eliminatedFruitTiles);
    return flag;
}

//-------------------------------------------------------------------------------------------------------------------
// Eliminate Tiles that match the fruit pattern
bool eliminateFruitTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
    bool flag = false;
    for (int col = 0; col < BOARD_WIDTH; ++col)
    {
        for (int row = 0; row < BOARD_HEIGHT; ++row)
        {
            if (eliminatedFruitTiles[col][row])
            {
                flag = true;
                board[col][row] = false;
                boardColors[col][row] = black;
            }
        }
    }

    return flag;
}

//-------------------------------------------------------------------------------------------------------------------
// Move donw a column of tile that have fruit matching tiles beneath
void moveDownFruitTileCol(int startRow, int col)
{
    for (int row = startRow; row < BOARD_HEIGHT; ++row)
    {
        bool isBlackRow = false;

        // If the row to move down is the top row, add a black row
        if (row == UP_BOUND)
            isBlackRow = true;

        if (isBlackRow)
        {
            board[col][row] = false;
            boardColors[col][row] = black;              
        }
        else
        {
            board[col][row] = board[col][row + 1];
            boardColors[col][row] = boardColors[col][row + 1];
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Move donw a column of tiles that have fruit matching tiles beneath
void moveDownFruitTilesCols(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
    for (int row = UP_BOUND; row >= 0; --row)
    {
        for (int col = 0; col < BOARD_WIDTH; ++col)
        {
            if (eliminatedFruitTiles[col][row]){
                moveDownFruitTileCol(row, col);

            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Clean up a the elimination array
void cleanUpEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
    for (int i = 0; i < BOARD_WIDTH; ++i)
    {   
        for (int j = 0; j < BOARD_HEIGHT; ++j)
        {
            eliminatedFruitTiles[i][j] = false;
        }
    }
}

void printEliminationTiles(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
    cout << "printEliminationTiles() - [Fruit Elimination Matrix]" << endl;
    for (int j = UP_BOUND; j >= 0; --j)
    {   
        for (int i = 0; i < BOARD_WIDTH; ++i)
        {
            cout << "[" << (eliminatedFruitTiles[i][j] ? "@" : "_") << "]";
        }
        cout << endl;
    }
    cout << endl;
}

bool checkTileSupport()
{
    bool flag[4] = {false};

    for (int i = 0; i < 4; ++i)
    {
        if ( _color4_equal(tileColors[i], black) )
        {
            int y = int(tilePos.y + tile[i].y);
            int x = int(tilePos.x + tile[i].x);

            if(board[x][y - 1])
                flag[i] = true; 

        }
    }

    return flag[0] || flag[1] || flag[2] || flag[3];
}

//-------------------------------------------------------------------------------------------------------------------
// Wrapper for Fruit Matching and elimination
bool checkFruitMatchAndEliminate()
{
    bool flag = true;
    // The second priority is to eliminate fruit tiles
    bool eliminatedFruitTiles[BOARD_WIDTH][BOARD_HEIGHT] = {false};
    // After reset the board, update the buffer object

    cleanUpEliminationTiles(eliminatedFruitTiles);

    while( matchFruitTiles(eliminatedFruitTiles))
    {
        // set the matching tile color to be black, and make the tile falling again since it is 

#ifdef DEBUG
        cout << "checkFruitMatchAndEliminate() -[eliminate fruit tiles]" << endl;
#endif
        // if fruit tiles could continue matching, do it iteratively
        eliminateFruitTiles(eliminatedFruitTiles);
        moveDownFruitTilesCols(eliminatedFruitTiles);

        flag = checkTileSupport();

        genBoardVertexColorsFromBoardColors();
        updateBoard();
        usleep(500);

        // Update the tiles to status after elimination
        cleanUpEliminationTiles(eliminatedFruitTiles);
    }

    return flag;
}