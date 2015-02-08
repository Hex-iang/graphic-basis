// Includes all source codes about Tile operation 
#pragma once
#include "FruitTetris.hpp"

//-------------------------------------------------------------------------------------------------------------------
// Called at the start of play and every time a tile is placed
void newTile()
{
    // First generate a random tile
    if( tryStopGame() ) return ;

    tileType = rand() % TILE_TYPE_NUM;
    vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
            ( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

    rotateType = rand() % ( allRotationShapeSize[tileType] );

// #ifdef DEBUG
//     cout << "newTile() - [Generate Randomness] TileType: " << tileType << ", RotateType: " << rotateType 
//         << ", RotationShape: " << allRotationShapeSize[tileType] << endl;
// #endif

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
    if(!dropTiles.empty() || ifPause || ifGameStop) return false;

    vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
            ( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

    // First test if the rotated version are in the boundary
    if (!testRotation(tilePos))
    {
        // Add logic to rescue a tile if it could be rotate with a unit movement.
        if ( testRotation(tilePos + vec2(1, 0)) )
        {
            tilePos += vec2(1, 0);
        }
        else if ( testRotation(tilePos + vec2(-1, 0)) )
        {
            tilePos += vec2(-1, 0);
        }
        else if ( testRotation(tilePos + vec2( 0, 1)) )
        {
            tilePos += vec2(0, 1);
        }
        else if ( testRotation(tilePos + vec2( 2, 0)) )
        {
            tilePos += vec2(2, 0);          
        }
        else if ( testRotation(tilePos + vec2(-2, 0)))
        {
            tilePos += vec2(-2, 0);
        }
        else if ( testRotation(tilePos + vec2( 0, 2)) )
        {
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
    }


// #ifdef DEBUG
//     cout << "rotateTile() - [Change Rotation] TileType: " << tileType << ", RotateType: " << rotateType 
//         << ", RotationShape: " << allRotationShapeSize[tileType] << endl;
// #endif
    updateTile();
    return true;
}

//-------------------------------------------------------------------------------------------------------------------
// shift Teris color 
void shiftTileColor()
{
    if (!dropTiles.empty() && !ifGameStop && !ifPause) return;

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
    bool flag = true;
    // Bound tileBound = getTileBound(tile);
    // vec2 futureTilePos = tilePos + direction;
    if( checkInBound(tilePos + direction) )
    {
        if ( checkTilesGridsCollision(tilePos + direction) )
        {
            flag = false;
        }
        else
        {
            // If there is no collision in the future position
            tilePos += direction;
        }
    }
    else{
        flag = false;
    }

    if( flag ) updateTile();

    return flag;

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
    // cout << "checkFullRow() - [Row Status] : " << endl;
    // for (int i = 0; i < BOARD_WIDTH; ++i)
    // {
    //     cout << "[" << (board[i][row]? 'T' : 'F') << "],"; 
    // }
    // cout << endl;
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

    bool eliminatedRows[BOARD_HEIGHT] = { false };

    for (int i = 0; i < BOARD_HEIGHT; ++i)
    {       
        if( checkFullRow(i) ){
            eliminatedRows[i] = true;
            eliminateFullRow(i);
        }
    }

    moveDownRows(eliminatedRows);
    genBoardVertexColorsFromBoardColors();
    updateBoard();
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
#ifdef DEBUG
                cout << "eliminateFruitTiles() - tile["<< col <<"][" << row << "]" <<endl;
#endif
                flag = true;
                board[col][row] = false;
                boardColors[col][row] = black;

            }
        }
    }

    return flag;
}

void printDropTiles()
{
    cout << endl;
    cout << "printDropTiles() - [Show DropTile List]" << endl;
    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
    {
        cout << "dropTiles#" <<  distance(dropTiles.begin(), it) << endl;
        for ( vector<Tile>::iterator iter = it->begin(); iter != it->end() ; ++iter)
        {
            cout << "Tile#"<< distance(it->begin(), iter) << "Position[" << iter->Position.x << "][" << iter->Position.y << "] ";
            cout << "Color = " << _MATCH_COLOR(iter->Color) << endl;
        }
    }  
    cout << endl;
}


//-------------------------------------------------------------------------------------------------------------------
// Move donw a column of tiles that have fruit matching tiles beneath
void moveDownFruitTilesCols(bool eliminatedFruitTiles[][BOARD_HEIGHT])
{
    for (int col = 0; col < BOARD_WIDTH; ++col)
    {
        bool flag = false;
        for (int row = 0; row < BOARD_HEIGHT; ++row)
        {
            if (eliminatedFruitTiles[col][row]){
                flag = true;
            }
            else if ( flag == true && board[col][row] ) // && !searchConnectToBottom(vec2(col, row)) )
            {
                // if eliminate fruit match make the tile not connected to bottom
                Tile item(vec2(col, row), boardColors[col][row]);

                // if the item not exists in the dropTiles
                if( find(dropTiles.begin(), dropTiles.end(), item) == dropTiles.end()){       
                    addTileToDropTiles(item);
                }
            }
        }
    }

    updateDropTile();

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

void printBoolBoardSizeArray(bool _array[][BOARD_HEIGHT])
{
    cout << "printBoolBoardSizeArray()" << endl;
    for (int j = UP_BOUND; j >= 0; --j)
    {   
        cout << setw(2) << j << "-";
        for (int i = 0; i < BOARD_WIDTH; ++i)
        {
            cout << "[" << (_array[i][j] ? "@" : "_") << "]";
        }
        cout << endl;
    }

    cout << "   ";
    for (int i = 0; i < BOARD_WIDTH; ++i)
    {
        cout << "[" << i << "]";
    }
    cout << endl;
}

// Check if the current tile is connected to the bottom
bool searchConnectToBottom(vec2 vertex)
{
    vector<vec2> stack;
    bool discovered[BOARD_WIDTH][BOARD_WIDTH] = {false};

    stack.push_back(vertex);

    while( !stack.empty())
    {
        vec2 current = stack.back();
        stack.pop_back();
        int current_x = int(current.x);
        int current_y = int(current.y);
        if( board[current_x][current_y] && !discovered[current_x][current_y] )
        {
            discovered[current_x][current_y] = true;
            if( current_y == 0)
                return true;
            else{
                stack.push_back(vec2(current_x + 1, current_y    ));
                stack.push_back(vec2(current_x - 1, current_y    ));
                stack.push_back(vec2(current_x    , current_y - 1));
                stack.push_back(vec2(current_x    , current_y + 1));
            }
        }
    }
    return false;
}

bool inline checkIfTilesAreNeighbor(Tile _pTile1, Tile _pTile2)
{
    return ( (abs(_pTile1.Position.x - _pTile2.Position.x) == 1 && _pTile1.Position.y == _pTile2.Position.y)||
             (abs(_pTile1.Position.y - _pTile2.Position.y) == 1 && _pTile1.Position.x == _pTile2.Position.x) );
}

void addTileToDropTiles(Tile _newDropTile)
{
    if ( dropTiles.empty() ){
#ifdef DEBUG
        cout << "addTileToDropTiles() - DropTiles empty, directly add new droptile" << endl;
#endif
        vector<Tile> newVec;
        newVec.push_back(_newDropTile);
        dropTiles.push_back(newVec);
    }
    else
    {
        bool nonConnectedTiles = false;
        for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
        {
            bool last_iteration = ( it == (--dropTiles.end()));
            bool inserted = false;

            if (last_iteration && !inserted)
            {
                nonConnectedTiles = true;
            }
        }

        if (nonConnectedTiles){
            vector<Tile> newVec;
            newVec.push_back(_newDropTile);
            dropTiles.push_back(newVec);
        }

    }
}

bool checkTileSupport()
{
    bool flag[4] = {false};

    for (int i = 0; i < 4; ++i)
    {
        int y = int(tilePos.y + tile[i].y);
        int x = int(tilePos.x + tile[i].x);
        if ( !_color4_equal(boardColors[x][y], black) )
        {

            if( searchConnectToBottom(vec2(x, y)) ){
                flag[i] = true; 
            }
            else
            {       
                Tile item(vec2(x, y), boardColors[x][y]);     
                if( find(dropTiles.begin(), dropTiles.end(), item) == dropTiles.end()){
#ifdef DEBUG
                    cout << "checkTileSupport() -[Add tile to dropTiles] tile[" << x << "][" << y << "], Board-Color:"
                     << _MATCH_COLOR(boardColors[x][y]) << ", Tile-Color: " << _MATCH_COLOR(tileColors[i]) << endl;
#endif       
                    addTileToDropTiles(item);

                }
            }

        }
    }
    printDropTiles();
    updateDropTile();
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
        // if fruit tiles could continue matching, do it iteratively

        eliminateFruitTiles(eliminatedFruitTiles);
        moveDownFruitTilesCols(eliminatedFruitTiles);

        checkTileSupport();

        genBoardVertexColorsFromBoardColors();
        updateBoard();

        // Update the tiles to status after elimination
        cleanUpEliminationTiles(eliminatedFruitTiles);
    }

    return flag;
}

bool fallTiles(vec2 direction)
{
#ifdef DEBUG
    printBoolBoardSizeArray(board);
    cout << "#################Before###################" << endl;
    printDropTiles();
#endif

    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end();)
    {
        bool inFlag = true;

        for ( vector<Tile>::iterator iter = it->begin(); iter != it->end() ; ++iter)
        {
            int x = int(iter->Position.x + direction.x);
            int y = int(iter->Position.y + direction.y);

            inFlag = inFlag && checkInBound( vec2(x,y) );
        }

        if ( !inFlag )
        {
            setDropTile(*it);
            it = dropTiles.erase(it);

#ifdef DEBUG

            cout << "#################After Setup###################" << endl;
            printDropTiles();
#endif

            genBoardVertexColorsFromBoardColors();
            updateBoard();
        }
        else
        {
            // Add velocity to all iteratorable item in the out vector
            for ( vector<Tile>::iterator iter = it->begin(); iter != it->end() ; ++iter)
                iter->Position += direction;
            ++it;
            updateDropTile();

#ifdef DEBUG

            cout << "#################After MoveDown###################" << endl;
            printDropTiles();
#endif

        }
    }

    // updateDropTile();

    return dropTiles.empty();
}

//-------------------------------------------------------------------------------------------------------------------
// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void setDropTile(vector<Tile> &_tile)
{
    // First check if the collision between tile and grid is detected
    // If a collision is detected
    for ( vector<Tile>::iterator iter = _tile.begin(); iter != _tile.end() ; ++iter)
    {
        int x = int(iter->Position.x);
        int y = int(iter->Position.y);

        if ( !_color4_equal(iter->Color, black) ){

    #ifdef DEBUG
        cout << "setDropTile() - [Setting Tile] x = " << x << ", y = " << y << endl;
    #endif
            board[x][y] = true;
            boardColors[x][y] = iter->Color;
        }
    }

}


void setTile()
{
    // First check if the collision between tile and grid is detected
    // If a collision is detected
    for (int i = 0; i < 4; ++i)
    {
        int x = int(tilePos.x + tile[i].x);
        int y = int(tilePos.y + tile[i].y);

        if ( !_color4_equal(tileColors[i], black) ){

#ifdef DEBUG
        cout << "setTile() - [Setting Tile] x = " << x << ", y = " << y 
        << ", Color: " << _MATCH_COLOR(tileColors[i])<< endl;
#endif
            board[x][y] = true;
            boardColors[x][y] = tileColors[i];
            // genBoardVertexColorFromBoardColor(x, y, boardColors[x][y]);
        }
    }

    printBoolBoardSizeArray(board);
    genBoardVertexColorsFromBoardColors();
    updateBoard();
}

void unsetDropTiles()
{
    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
    {
        for (vector<Tile>::iterator iter = it->begin(); iter != it->end(); ++iter)
        {
            int x = int(iter->Position.x);
            int y = int(iter->Position.y);

#ifdef DEBUG
            cout << "unsetDropTiles() - [Unsetting Tile] x = " << x << ", y = " << y 
            << ", Color: " << _MATCH_COLOR(iter->Color)<< endl;
#endif
            board[x][y] = false;
            boardColors[x][y] = black;
        }
    }

    printBoolBoardSizeArray(board);
    genBoardVertexColorsFromBoardColors();
    updateBoard();
}

void cleanVBOTileBuffer()
{
    // Clean up Color and Position buffer
    color4 blank[BOARD_WIDTH*BOARD_HEIGHT*3*2] = {black};
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BOARD_WIDTH*BOARD_HEIGHT*3*2*sizeof(color4), blank);
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BOARD_WIDTH*BOARD_HEIGHT*3*2*sizeof(point4), blank);
}

void updateDropTile()
{
    // Clear the buffer object before update it 
    cleanVBOTileBuffer();
    int dtSize = dropTiles.size();

    for (int i = 0; i < dtSize; i++) 
    {
        // Calculate the grid coordinates of the cell
        GLfloat x = dropTiles[i].Position.x; 
        GLfloat y = dropTiles[i].Position.y;


        // Contraints that make the tile outside the UP_BOUND of board invisible
        // ==============================================================================
        point4 p1 = point4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
        point4 p2 = point4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
        point4 p3 = point4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
        point4 p4 = point4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

        // Two points are used by two triangles each
        point4 newPoints[6] = {p1, p2, p3, p2, p3, p4}; 

        // Put new data in the VBO
        color4 pointsColors[6];

        genColorVertexFromTileColor(pointsColors, dropTiles[i].Color);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(color4), 6*sizeof(color4), pointsColors);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(point4), 6*sizeof(point4), newPoints);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
}

void updateTile()
{
    // First Clean up buffer
    cleanVBOTileBuffer();
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
        color4 pointsColors[6];

        genColorVertexFromTileColor(pointsColors, tileColors[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(color4), 6*sizeof(color4), pointsColors);


        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(point4), 6*sizeof(point4), newPoints);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}