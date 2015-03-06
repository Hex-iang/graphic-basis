// Includes all source codes about Tile operation 
#pragma once
#include "FruitTetris.hpp"

//-------------------------------------------------------------------------------------------------------------------
// Called at the start of play and every time a tile is placed
void newTile()
{
    tiles.clear();
    // First generate a random tile
    if( tryStopGame() ) return ;

    tileType = rand() % TILE_TYPE_NUM;
    vec2 (* pAllRotationShape)[4] = (tileType == TILE_TYPE_L) ?  allRotationsLshape :
            ( (tileType == TILE_TYPE_S)? allRotationsSshape:allRotationsIshape );

    rotateType = rand() % ( allRotationShapeSize[tileType] );

    // Update the geometry VBO of current tile

    TileBound tileBound = getTileBound(pAllRotationShape[rotateType]);
    int coverage = ( RIGHT_BOUND - int(tileBound.right) + int(tileBound.left) + 1);

    int hpos = rand() % coverage - int(tileBound.left);
    int vpos = UP_BOUND - tileBound.up;
    tilePos = vec2(hpos , vpos);
    // Put the tile at the top of the board
    
    for ( int i = 0; i < 4; i++)
    {
        Tile tmpTile( pAllRotationShape[rotateType][i] + tilePos, genRandomColor());
        tiles.push_back(tmpTile);
    }

    // Call update function to draw the tile with its position as well as its color
    updateTiles(); 

    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------
// Rotate the current tile 
bool rotateTile()
{
    if(!dropTiles.empty() || ifPause || ifGameStop || tiles.empty()) return false;

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
            updateTiles();
            return false;
        }
    }

    // Testing succeed, do actual rotation

    rotateType = (rotateType + 1) % allRotationShapeSize[tileType];
    // Update the geometry VBO of current tile
    for ( int i = 0; i < 4; i++){
// #ifdef DEBUG
//         cout << "rotateTile() [Rotating tiles] position before: " << tiles[i].Position.x << ", "<< tiles[i].Position.y << endl;
// #endif
        tiles[i].Position = ( tilePos + pAllRotationShape[rotateType][i]);

// #ifdef DEBUG
//         cout << "rotateTile() [Rotating tiles] position after: " << tiles[i].Position.x << ", "<< tiles[i].Position.y << endl;
// #endif

    }

    updateTiles();
    return true;
}

//-------------------------------------------------------------------------------------------------------------------
// shift Teris color 
void shiftTileColor()
{
    if (!dropTiles.empty() && !ifGameStop && !ifPause && tiles.empty() ) return;

    color4 oldColor = tiles[3].Color;

    tiles[3].Color = tiles[2].Color;
    tiles[2].Color = tiles[1].Color;
    tiles[1].Color = tiles[0].Color;
    tiles[0].Color = oldColor;
    updateTiles();
}


//-------------------------------------------------------------------------------------------------------------------
// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool moveTile(vec2 direction)
{
    bool flag = true;
    bool inFlag = true;

    for ( vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end() ; ++iter)
    {
        int x = int(iter->Position.x + direction.x);
        int y = int(iter->Position.y + direction.y);

// #ifdef DEBUG
//         cout << "moveTile() - Position: " << x << ", " << y << endl;
// #endif

        if( true == checkTileGridCollision( x, y ) )
        {
// #ifdef DEBUG
//         cout << "moveTile() - [Collision detected]" << endl;
// #endif
            flag = false;
        }

        if( !_IN_BOUND(x, y))
        {
            // If current tile is not in the boundary, not update its
            inFlag = false;
        }
    }

    if ( flag && inFlag)
    {
        // Add velocity to all iteratorable item in the out vector
        tilePos += direction;
        for ( vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end() ; ++iter){
            iter->Position += direction;
        }
        updateTiles();
    }

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
    addUnsupportedTilesToDropTiles();
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
        
            if( _COLOR4_EQUAL(boardColors[i + 0][y], boardColors[i + 1][y]) && 
                _COLOR4_EQUAL(boardColors[i + 1][y], boardColors[i + 2][y]) &&
                !_COLOR4_EQUAL(boardColors[i][y], black)                    )
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
            if( _COLOR4_EQUAL(boardColors[x][i + 0], boardColors[x][i + 1]) && 
                _COLOR4_EQUAL(boardColors[x][i + 1], boardColors[x][i + 2]) &&
                !_COLOR4_EQUAL(boardColors[x][i], black)                )
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

void printTiles()
{
    cout << "printTiles() - [Print Current Tile]" << endl;
    for ( vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end() ; ++iter)
    {
        cout << "Tile#"<< distance(tiles.begin(), iter) << "Position[" << iter->Position.x << "][" << iter->Position.y << "] ";
        cout << "Color = " << _MATCH_COLOR(iter->Color) << endl;
    }    
}

//-------------------------------------------------------------------------------------------------------------------
// Move donw a column of tiles that have fruit matching tiles beneath
bool searchConnectedToEliminatedTiles()
{

}

//-------------------------------------------------------------------------------------------------------------------
// Move donw a column of tiles that have fruit matching tiles beneath
// void addUnsupportedTilesToDropList(bool eliminatedFruitTiles[][BOARD_HEIGHT])
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
            else if ( flag == true && board[col][row] )
            {
                // if eliminate fruit match make the tile not connected to bottom
                Tile item(vec2(col, row), boardColors[col][row]);

                // if the item not exists in the dropTiles
                bool NotExisted = true;
                for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
                {
                    for (vector<Tile>::iterator iter = it->begin(); iter != it->end(); ++iter)
                    {
                        if ( item == (*iter) )
                        {
                            cout << "moveDownFruitTilesCols() - DropTile matching Got" << endl;
                            NotExisted = false;
                        }                    
                    }
                }
                if (NotExisted){
#ifdef DEBUG
                    cout << "moveDownFruitTilesCols() -[Add tile to dropTiles] tile[" << col << "][" << row << "], Board-Color:"
                     << _MATCH_COLOR(boardColors[col][row]) << endl;
#endif 
                    addTileToDropTiles(item);
                }
            }
            else if ( flag == true )
            {
                // if the current row is not directly above eliminated row
                flag = false;
            }
        }
    }

//     updateDropTiles();

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
    bool discovered[BOARD_WIDTH][BOARD_HEIGHT];
    for (int i = 0; i < BOARD_WIDTH; ++i)
        for (int j = 0; j < BOARD_HEIGHT; ++j)
            discovered[i][j] = false;


    stack.push_back(vertex);

    while( !stack.empty())
    {
        vec2 current = stack.back();
        stack.pop_back();
        int current_x = int(current.x);
        int current_y = int(current.y);
        if( !discovered[current_x][current_y] )
        {
// #ifdef DEBUG
//             cout << "searchConnectToBottom() - exploring node[" << current_x << "][" << current_y << "]" << endl;
// #endif
            discovered[current_x][current_y] = true;
            if( board[current_x][current_y]){
                if( current_y == 0)
                {
// #ifdef DEBUG
//                     cout << "searchConnectToBottom() - Current component connected to bottom"<<endl;
// #endif
                    // if we searched to the bottom
                    return true;
                }

                if( _ON_BOARD(current_x + 1, current_y) ) stack.push_back(vec2(current_x + 1, current_y    ));
                if( _ON_BOARD(current_x - 1, current_y) ) stack.push_back(vec2(current_x - 1, current_y    ));
                if( _ON_BOARD(current_x, current_y - 1) ) stack.push_back(vec2(current_x    , current_y - 1));
                if( _ON_BOARD(current_x, current_y + 1) ) stack.push_back(vec2(current_x    , current_y + 1));

            }
        }
    }

    return false;
}

// Search the connected component in the graph and drop down it if not connected
vector<Tile> searchConnectedComponent(bool (&graph)[BOARD_WIDTH][BOARD_HEIGHT], color4 (&cgraph)[BOARD_WIDTH][BOARD_HEIGHT], vec2 vertex )
{
    vector<Tile> connectedTile;
    vector<vec2> stack;
    bool discovered[BOARD_WIDTH][BOARD_HEIGHT];
    for (int i = 0; i < BOARD_WIDTH; ++i)
        for (int j = 0; j < BOARD_HEIGHT; ++j)
            discovered[i][j] = false;

    stack.push_back(vertex);

    while( !stack.empty())
    {
        vec2 current = stack.back();
        stack.pop_back();
        int current_x = int(current.x);
        int current_y = int(current.y);
        if( !discovered[current_x][current_y] )
        {
            discovered[current_x][current_y] = true;
            if( graph[current_x][current_y] )
            {
                graph[current_x][current_y] = false;
                connectedTile.push_back(Tile(current, cgraph[current_x][current_y]));

                if( _ON_BOARD(current_x + 1, current_y)) stack.push_back(vec2(current_x + 1, current_y    ));
                if( _ON_BOARD(current_x - 1, current_y)) stack.push_back(vec2(current_x - 1, current_y    ));
                if( _ON_BOARD(current_x, current_y - 1)) stack.push_back(vec2(current_x    , current_y - 1));
                if( _ON_BOARD(current_x, current_y + 1)) stack.push_back(vec2(current_x    , current_y + 1));
            }
        }
    }
    return connectedTile;
}

// Add one tile to the drop tiles and update the dropping tile set
void addTileToDropTiles(Tile _newDropTile)
{
    bool graph[BOARD_WIDTH][BOARD_HEIGHT] = {false};
    color4 cgraph[BOARD_WIDTH][BOARD_HEIGHT];
    for (int i = 0; i < BOARD_WIDTH; ++i)
        for (int j = 0; j < BOARD_HEIGHT; ++j)
            cgraph[i][j] = black;

    // Setup the graph with current dropping tiles
    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
    {
        for (vector<Tile>::iterator iter = it->begin(); iter < it->end(); ++iter)
        {
            int x = int(iter->Position.x);
            int y = int(iter->Position.y);

            graph[x][y] = true;
            cgraph[x][y] = iter->Color;
        }
    }
    
    dropTiles.clear();
    int new_x = int(_newDropTile.Position.x);
    int new_y = int(_newDropTile.Position.y);
    graph[new_x][new_y] = true;
    cgraph[new_x][new_y] = _newDropTile.Color;

    for (int row = 0; row < BOARD_HEIGHT; ++row)
    {
        for (int col = 0; col < BOARD_WIDTH; ++col)
        {
            if( graph[col][row]){
                vector<Tile> tileSet = searchConnectedComponent(graph, cgraph, vec2(col, row) );
                dropTiles.push_back(tileSet);
            }
        }
    }
}

void addTilesToDropTiles(vector<Tile> _newDropTiles)
{
    bool graph[BOARD_WIDTH][BOARD_HEIGHT] = {false};
    color4 cgraph[BOARD_WIDTH][BOARD_HEIGHT];
    for (int i = 0; i < BOARD_WIDTH; ++i)
        for (int j = 0; j < BOARD_HEIGHT; ++j)
            cgraph[i][j] = black;

    // Setup the graph with current dropping tiles
    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
    {
        for (vector<Tile>::iterator iter = it->begin(); iter < it->end(); ++iter)
        {
            int x = int(iter->Position.x);
            int y = int(iter->Position.y);

            graph[x][y] = true;
            cgraph[x][y] = iter->Color;
            cout << "addTileToDropTiles() - Color: " << _MATCH_COLOR(cgraph[x][y]) << endl;
        }
    }
    
    dropTiles.clear();
    // Add a list of tiles to Drop Tiles
    for (vector<Tile>::iterator iter = _newDropTiles.begin(); iter != _newDropTiles.end(); ++iter)
    {
        int new_x = int(iter->Position.x);
        int new_y = int(iter->Position.y);
        graph[new_x][new_y] = true;
        cgraph[new_x][new_y] = iter->Color;
    }

    for (int row = 0; row < BOARD_HEIGHT; ++row)
    {
        for (int col = 0; col < BOARD_WIDTH; ++col)
        {
            if( graph[col][row]){
                vector<Tile> tileSet = searchConnectedComponent(graph, cgraph, vec2(col, row) );
                dropTiles.push_back(tileSet);
            }
        }
    }
}

// Make all non-supportted tiles falling
void addUnsupportedTilesToDropTiles()
{
    vector<Tile> unSupportedTiles;

    for (int row = 0; row < BOARD_HEIGHT; ++row)
    {
        for (int col = 0; col < BOARD_WIDTH; ++col)
        {
            if( !_COLOR4_EQUAL(boardColors[col][row], black) && !searchConnectToBottom(vec2(col, row)) )
            {
                Tile unSupportedTile(vec2(col, row), boardColors[col][row]);
#ifdef DEBUG
                cout << "addUnsupportedTilesToDropTiles() - Add UnsupportedTiles Conponent:" << 
                "[" << unSupportedTile.Position.x << "][" << unSupportedTile.Position.y<< "]" << endl;
#endif
                unSupportedTiles.push_back(unSupportedTile);
            }
        }
    }

    for (vector<Tile>::iterator iter = unSupportedTiles.begin(); iter != unSupportedTiles.end(); ++iter) 
    {
        int y = int(iter->Position.y);
        int x = int(iter->Position.x);

        Tile item(vec2(x, y), boardColors[x][y]);
        bool NotExisted = true;

        for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
        {
            for (vector<Tile>::iterator iter = it->begin(); iter != it->end(); ++iter)
            {
                if ( item == (*iter) )
                {
#ifdef DEBUG
                    cout << "addUnsupportedTilesToDropTiles() - DropTile matching Got" << endl;
#endif
                    NotExisted = false;
                }    
            }
        }

        if (NotExisted){
#ifdef DEBUG
            cout << "addUnsupportedTilesToDropTiles() -[Add tile to dropTiles] tile[" << x << "][" << y << "], Board-Color:"
             << _MATCH_COLOR(boardColors[x][y]) << ", Tile-Color: " << _MATCH_COLOR(iter->Color) << endl;
#endif       
            addTilesToDropTiles(unSupportedTiles);
        }

    }
#ifdef DEBUG
    printDropTiles();
#endif
    updateDropTiles();

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

        addUnsupportedTilesToDropTiles();

        genBoardVertexColorsFromBoardColors();
        updateBoard();

        // Update the tiles to status after elimination
        cleanUpEliminationTiles(eliminatedFruitTiles);
    }

    return flag;
}

bool fallTiles(vec2 direction)
{
    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end();)
    {
        bool inFlag = true;

        for ( vector<Tile>::iterator iter = it->begin(); iter != it->end() ; ++iter)
        {
            int x = int(iter->Position.x + direction.x);
            int y = int(iter->Position.y + direction.y);

            if( checkTileGridCollision( x, y ) )
                inFlag = false;
        }

        if ( !inFlag )
        {
            setDropTiles(*it);
            it = dropTiles.erase(it);

            genBoardVertexColorsFromBoardColors();
            updateBoard();
        }
        else
        {
            // Add velocity to all iteratorable item in the out vector
            for ( vector<Tile>::iterator iter = it->begin(); iter != it->end() ; ++iter)
                iter->Position += direction;
            ++it;
            updateDropTiles();

        }
    }

    // updateDropTiles();

    return dropTiles.empty();
}

//-------------------------------------------------------------------------------------------------------------------
// Places the current tile - update the board vertex colour VBO and the array maintainiong occupied cells
void setDropTiles(vector<Tile> &_tile)
{
    // First check if the collision between tile and grid is detected
    // If a collision is detected
    for ( vector<Tile>::iterator iter = _tile.begin(); iter != _tile.end() ; ++iter)
    {
        int x = int(iter->Position.x);
        int y = int(iter->Position.y);

        if ( !_COLOR4_EQUAL(iter->Color, black) ){

    #ifdef DEBUG
        cout << "setDropTiles() - [Setting Tile] x = " << x << ", y = " << y << ", Color:"<<  _MATCH_COLOR(iter->Color) << endl;
    #endif
            board[x][y] = true;
            boardColors[x][y] = iter->Color;
        }
    }

}


void setTiles()
{
    // First check if the collision between tile and grid is detected
    // If a collision is detected
    for (vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end(); ++iter) 
    {
        int x = int(iter->Position.x);
        int y = int(iter->Position.y);

        if ( !_COLOR4_EQUAL(iter->Color, black) ){

#ifdef DEBUG
        cout << "setTiles() - [Setting Tile] x = " << x << ", y = " << y 
        << ", Color: " << _MATCH_COLOR(iter->Color)<< endl;
#endif
            board[x][y] = true;
            boardColors[x][y] = iter->Color;
            // genBoardVertexColorFromBoardColor(x, y, boardColors[x][y]);
        }
    }
    // After the tile is setted, clear it up
    tiles.clear();

#ifdef DEBUG
    printBoolBoardSizeArray(board);
#endif
    genBoardVertexColorsFromBoardColors();
    updateBoard();
}

void unsetTiles()
{
    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); ++it)
    {
        for (vector<Tile>::iterator iter = it->begin(); iter != it->end(); ++iter)
        {
            int x = int(iter->Position.x);
            int y = int(iter->Position.y);

#ifdef DEBUG
            cout << "unsetDropTiless() - [Unsetting Tile] x = " << x << ", y = " << y 
            << ", Color: " << _MATCH_COLOR(iter->Color)<< endl;
#endif
            board[x][y] = false;
            boardColors[x][y] = black;
        }
    }

#ifdef DEBUG
    printBoolBoardSizeArray(board);
#endif

    genBoardVertexColorsFromBoardColors();
    updateBoard();
}

void updateTiles()
{

    int idx = 0;
    for (vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end(); ++iter) 
    {
        // Calculate the grid coordinates of the cell
        GLfloat x = iter->Position.x; 
        GLfloat y = iter->Position.y;
        point4 newPoints[TILE_VERTEX_NUM];


        // Contraints that make the tile outside the UP_BOUND of board invisible
        // ==============================================================================

        point4 p1 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);
        point4 p2 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);
        point4 p3 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);
        point4 p4 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);

        point4 p5 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);
        point4 p6 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);
        point4 p7 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);
        point4 p8 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);

        quad( &newPoints[ 0*QUAD_VERTEX_NUM], p1, p2, p3, p4);
        quad( &newPoints[ 1*QUAD_VERTEX_NUM], p1, p5, p6, p2);
        quad( &newPoints[ 2*QUAD_VERTEX_NUM], p1, p5, p8, p4);
        quad( &newPoints[ 3*QUAD_VERTEX_NUM], p2, p6, p7, p3);
        quad( &newPoints[ 4*QUAD_VERTEX_NUM], p3, p4, p8, p7);
        quad( &newPoints[ 5*QUAD_VERTEX_NUM], p5, p6, p7, p8);

        // Put new data in the VBO
        color4 pointsColors[TILE_VERTEX_NUM];

        genColorVertexFromTileColor(pointsColors, iter->Color);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
        glBufferSubData(GL_ARRAY_BUFFER, idx*TILE_VERTEX_NUM*sizeof(color4), TILE_VERTEX_NUM*sizeof(color4), pointsColors);


        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
        glBufferSubData(GL_ARRAY_BUFFER, idx*TILE_VERTEX_NUM*sizeof(point4), TILE_VERTEX_NUM*sizeof(point4), newPoints);
        idx++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateDropTiles()
{
    // Clear the buffer object before update it 
    // cleanVBOTileBuffer();
    int idx = 0;

    for (vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); it++) 
    {
        for (vector<Tile>::iterator iter = it->begin(); iter != it->end(); ++iter)
        {
            GLfloat x = iter->Position.x; 
            GLfloat y = iter->Position.y;
            point4 newPoints[TILE_VERTEX_NUM];
            // Contraints that make the tile outside the UP_BOUND of board invisible
            // ==============================================================================
            point4 p1 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);
            point4 p2 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);
            point4 p3 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);
            point4 p4 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), EDGE_LEN / 2 + DEPTH_1, 1);

            point4 p5 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);
            point4 p6 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);
            point4 p7 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);
            point4 p8 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_1, 1);

            // Two points are used by two triangles each

            quad( &newPoints[ 0*QUAD_VERTEX_NUM], p1, p2, p3, p4);
            quad( &newPoints[ 1*QUAD_VERTEX_NUM], p1, p5, p6, p2);
            quad( &newPoints[ 2*QUAD_VERTEX_NUM], p1, p5, p8, p4);
            quad( &newPoints[ 3*QUAD_VERTEX_NUM], p2, p6, p7, p3);
            quad( &newPoints[ 4*QUAD_VERTEX_NUM], p3, p4, p8, p7);
            quad( &newPoints[ 5*QUAD_VERTEX_NUM], p5, p6, p7, p8);

            // Put new data in the VBO
            color4 pointsColors[TILE_VERTEX_NUM];

            genColorVertexFromTileColor(pointsColors, iter->Color);

            glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
            glBufferSubData(GL_ARRAY_BUFFER, idx*TILE_VERTEX_NUM*sizeof(color4), TILE_VERTEX_NUM*sizeof(color4), pointsColors);

            glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]); 
            glBufferSubData(GL_ARRAY_BUFFER, idx*TILE_VERTEX_NUM*sizeof(point4), TILE_VERTEX_NUM*sizeof(point4), newPoints);

            idx++;

        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
}

