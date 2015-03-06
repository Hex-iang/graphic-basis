#include "FruitTetris.hpp"
#include "Utility.hpp"
#include "Tile.hpp"
// =================================================================================================================
// Tile Controller Function  

//-------------------------------------------------------------------------------------------------------------------
// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updateBoard()
{
    int idx = 0;
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if(board[x][y] == true)
            {
                point4 newPoints[TILE_VERTEX_NUM];
                // If the current board have tiles, then draw it
                point4 p1 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), EDGE_LEN / 2, 1);
                point4 p2 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), EDGE_LEN / 2, 1);
                point4 p3 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), EDGE_LEN / 2, 1);
                point4 p4 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), EDGE_LEN / 2, 1);

                point4 p5 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), - EDGE_LEN / 2, 1);
                point4 p6 = point4(EDGE_LEN     + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), - EDGE_LEN / 2, 1);
                point4 p7 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN * 2  + (y * EDGE_LEN), - EDGE_LEN / 2, 1);
                point4 p8 = point4(EDGE_LEN * 2 + (x * EDGE_LEN), EDGE_LEN      + (y * EDGE_LEN), - EDGE_LEN / 2, 1);

                quad( &newPoints[ 0*QUAD_VERTEX_NUM ], p1, p2, p3, p4);
                quad( &newPoints[ 1*QUAD_VERTEX_NUM ], p1, p5, p6, p2);
                quad( &newPoints[ 2*QUAD_VERTEX_NUM ], p1, p5, p8, p4);
                quad( &newPoints[ 3*QUAD_VERTEX_NUM ], p2, p6, p7, p3);
                quad( &newPoints[ 4*QUAD_VERTEX_NUM ], p3, p4, p8, p7);
                quad( &newPoints[ 5*QUAD_VERTEX_NUM ], p5, p6, p7, p8);         
                   
                color4 pointsColors[TILE_VERTEX_NUM];

                genColorVertexFromTileColor(pointsColors, boardColors[x][y]);

                glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
                glBufferSubData(GL_ARRAY_BUFFER, idx*TILE_VERTEX_NUM*sizeof(color4), TILE_VERTEX_NUM*sizeof(color4), pointsColors);


                glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_POSITION]); 
                glBufferSubData(GL_ARRAY_BUFFER, idx*TILE_VERTEX_NUM*sizeof(point4), TILE_VERTEX_NUM*sizeof(point4), newPoints);
                idx++ ;
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);       
}

//-------------------------------------------------------------------------------------------------------------------
// 
bool checkEndOfGame()
{
    bool flag = false;
    for (int i = 0; i < BOARD_WIDTH; ++i)
    {

#ifdef DEBUG
        if (board[i][UP_BOUND])
        {
            cout << "chcekEndOfGame() - board[" << i << "][" << UP_BOUND << "]"<< endl;
        }
#endif
        flag = ( flag || board[i][UP_BOUND]);
    }

    return flag;
}


// ================================================================================================================= 
// Initialization controller

//-------------------------------------------------------------------------------------------------------------------
// Init robot arm
void initRobotArm()
{
    point4 robotArmPoints[ARMVERTICES];
    color4 robotArmColors[ARMVERTICES];

    point4 vertices[8] = {
        point4( -EDGE_LEN / 2, -EDGE_LEN / 2,  EDGE_LEN / 2, 1.0 ),
        point4( -EDGE_LEN / 2,  EDGE_LEN / 2,  EDGE_LEN / 2, 1.0 ),
        point4(  EDGE_LEN / 2,  EDGE_LEN / 2,  EDGE_LEN / 2, 1.0 ),
        point4(  EDGE_LEN / 2, -EDGE_LEN / 2,  EDGE_LEN / 2, 1.0 ),
        point4( -EDGE_LEN / 2, -EDGE_LEN / 2, -EDGE_LEN / 2, 1.0 ),
        point4( -EDGE_LEN / 2,  EDGE_LEN / 2, -EDGE_LEN / 2, 1.0 ),
        point4(  EDGE_LEN / 2,  EDGE_LEN / 2, -EDGE_LEN / 2, 1.0 ),
        point4(  EDGE_LEN / 2, -EDGE_LEN / 2, -EDGE_LEN / 2, 1.0 )
    };

    quad( &robotArmPoints[ 0*QUAD_VERTEX_NUM ], vertices[0], vertices[1], vertices[2], vertices[3]);
    quad( &robotArmPoints[ 1*QUAD_VERTEX_NUM ], vertices[0], vertices[4], vertices[5], vertices[1]);
    quad( &robotArmPoints[ 2*QUAD_VERTEX_NUM ], vertices[0], vertices[4], vertices[7], vertices[3]);
    quad( &robotArmPoints[ 3*QUAD_VERTEX_NUM ], vertices[1], vertices[5], vertices[6], vertices[2]);
    quad( &robotArmPoints[ 4*QUAD_VERTEX_NUM ], vertices[2], vertices[3], vertices[7], vertices[6]);
    quad( &robotArmPoints[ 5*QUAD_VERTEX_NUM ], vertices[4], vertices[5], vertices[6], vertices[7]);


    colorquad( &robotArmColors[ 0*QUAD_VERTEX_NUM ], cyan,      cyan,       cyan,       cyan);
    colorquad( &robotArmColors[ 1*QUAD_VERTEX_NUM ], purple,    purple,     purple,     purple);
    colorquad( &robotArmColors[ 2*QUAD_VERTEX_NUM ], orange,    orange,     orange,     orange);
    colorquad( &robotArmColors[ 3*QUAD_VERTEX_NUM ], yellow,    yellow,     yellow,     yellow);
    colorquad( &robotArmColors[ 4*QUAD_VERTEX_NUM ], purple,    purple,     purple,     purple);
    colorquad( &robotArmColors[ 5*QUAD_VERTEX_NUM ], cyan,      cyan,      cyan,     cyan);

    // Bind the Current vao to Tils's array
    glBindVertexArray(vaoIDs[VAO_ROBOTARM]);
    glGenBuffers(2, &vboIDs[6]);

    vColor    = glGetAttribLocation(RobotArmShader, "vColor");
    vPosition = glGetAttribLocation(RobotArmShader, "vPosition");
    // vColor    = glGetAttribLocation(UniversalShader, "vColor");
    // vPosition = glGetAttribLocation(UniversalShader, "vPosition");

    // Current tile vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_ROBOTARM_POSITION]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(robotArmPoints), robotArmPoints, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    // Current tile vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_ROBOTARM_COLOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(robotArmColors), robotArmColors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);    
}


//-------------------------------------------------------------------------------------------------------------------
// Init back grid of the board

void initGrid()
{
    // ***Generate geometry data
    point4 gridpoints[GRID_LINE_VERTEX_NUM]; 
    // Array containing the 64 points of the 32 total lines ( 21 horizontal line + 11 vertical line ) to be later put in the VBO
    point4 gridcolours[GRID_LINE_VERTEX_NUM]; 
    // One colour per vertex
  
    // Vertical lines 
    for (int i = 0; i < BOARD_HEIGHT; i++){
        for (int j = 0; j < BOARD_WIDTH; j++)
        {       
            point4 p1 = point4(EDGE_LEN     + (j * EDGE_LEN), EDGE_LEN      + (i * EDGE_LEN),   EDGE_LEN / 2 + DEPTH_3, 1);
            point4 p2 = point4(EDGE_LEN     + (j * EDGE_LEN), EDGE_LEN * 2  + (i * EDGE_LEN),   EDGE_LEN / 2 + DEPTH_3, 1);
            point4 p3 = point4(EDGE_LEN * 2 + (j * EDGE_LEN), EDGE_LEN * 2  + (i * EDGE_LEN),   EDGE_LEN / 2 + DEPTH_3, 1);
            point4 p4 = point4(EDGE_LEN * 2 + (j * EDGE_LEN), EDGE_LEN      + (i * EDGE_LEN),   EDGE_LEN / 2 + DEPTH_3, 1);
            point4 p5 = point4(EDGE_LEN     + (j * EDGE_LEN), EDGE_LEN      + (i * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_3, 1);
            point4 p6 = point4(EDGE_LEN     + (j * EDGE_LEN), EDGE_LEN * 2  + (i * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_3, 1);
            point4 p7 = point4(EDGE_LEN * 2 + (j * EDGE_LEN), EDGE_LEN * 2  + (i * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_3, 1);
            point4 p8 = point4(EDGE_LEN * 2 + (j * EDGE_LEN), EDGE_LEN      + (i * EDGE_LEN), - EDGE_LEN / 2 - DEPTH_3, 1);

            // Two points are used by two triangles each
            quadLine( &gridpoints[ TILE_LINE_VERTEX_NUM*(BOARD_WIDTH*i + j) + 0*QUAD_LINE_VERTEX_NUM], p1, p2, p3, p4);
            quadLine( &gridpoints[ TILE_LINE_VERTEX_NUM*(BOARD_WIDTH*i + j) + 1*QUAD_LINE_VERTEX_NUM], p1, p5, p6, p2);
            quadLine( &gridpoints[ TILE_LINE_VERTEX_NUM*(BOARD_WIDTH*i + j) + 2*QUAD_LINE_VERTEX_NUM], p1, p5, p8, p4);
            quadLine( &gridpoints[ TILE_LINE_VERTEX_NUM*(BOARD_WIDTH*i + j) + 3*QUAD_LINE_VERTEX_NUM], p2, p6, p7, p3);
            quadLine( &gridpoints[ TILE_LINE_VERTEX_NUM*(BOARD_WIDTH*i + j) + 4*QUAD_LINE_VERTEX_NUM], p3, p4, p8, p7);
            quadLine( &gridpoints[ TILE_LINE_VERTEX_NUM*(BOARD_WIDTH*i + j) + 5*QUAD_LINE_VERTEX_NUM], p5, p6, p7, p8);

        }
    }

    // Make all grid lines white
    for (int i = 0; i < GRID_LINE_VERTEX_NUM; i++)
        gridcolours[i] = white;

    // *** set up buffer objects
    // Set up first VAO (representing grid lines)
    glBindVertexArray(vaoIDs[VAO_GRID]);
    // Bind the first VAO
    glGenBuffers(2, vboIDs); 
    // Create two Vertex Buffer Objects for this VAO (positions, colours)

    vColor    = glGetAttribLocation(UniversalShader, "vColor");
    vPosition = glGetAttribLocation(UniversalShader, "vPosition");

    // Grid vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_GRID_POSITION]); 
    // Bind the first grid VBO (vertex positions)
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridpoints), gridpoints, GL_STATIC_DRAW); 
    // Put the grid points in the VBO
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
    glEnableVertexAttribArray(vPosition); 
    // Enable the attribute
    
    // Grid vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_GRID_COLOR]); 
    // Bind the second grid VBO (vertex colours)
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridcolours), gridcolours, GL_STATIC_DRAW); 
    // Put the grid colours in the VBO
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor); 
    // Enable the attribute
}

//-------------------------------------------------------------------------------------------------------------------
// Init playing board
void initBoard()
{
    // *** Generate the geometric data
    point4 boardpoints[BOARD_WIDTH*BOARD_HEIGHT*TILE_VERTEX_NUM];

    // Initially no cell is occupied
    for (int i = 0; i < BOARD_WIDTH; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
        {
            board[i][j]         = false;
            boardColors[i][j]   = black; 
        }

    // Generate vertex color structure from board
    genBoardVertexColorsFromBoardColors();

    // *** set up buffer objects
    glBindVertexArray(vaoIDs[VAO_BOARD]);
    glGenBuffers(2, &vboIDs[2]);

    vColor    = glGetAttribLocation(UniversalShader, "vColor");
    vPosition = glGetAttribLocation(UniversalShader, "vPosition");

    // Grid cell vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_POSITION]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardpoints, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    // Grid cell vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardVertexColors, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);
}

//-------------------------------------------------------------------------------------------------------------------
// No geometry for current tile initially
void initCurrentTile()
{
    // Bind the Current vao to Tils's array
    glBindVertexArray(vaoIDs[VAO_TILE]);
    glGenBuffers(2, &vboIDs[4]);

    vColor    = glGetAttribLocation(UniversalShader, "vColor");
    vPosition = glGetAttribLocation(UniversalShader, "vPosition");

    // Current tile vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_POSITION]);
    glBufferData(GL_ARRAY_BUFFER, BOARD_WIDTH*BOARD_HEIGHT*TILE_VERTEX_NUM*sizeof(point4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    // Current tile vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
    glBufferData(GL_ARRAY_BUFFER, BOARD_WIDTH*BOARD_HEIGHT*TILE_VERTEX_NUM*sizeof(color4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);
}

//-------------------------------------------------------------------------------------------------------------------
// init openGL configure
void init()
{
    // initialize random generator with seed 
    srand(time(NULL));

    // Load shaders and use the shader program
    RobotArmShader = InitShader("shaders/robotvshader.glsl", "shaders/robotfshader.glsl");

    UniversalShader = InitShader("shaders/vshader.glsl", "shaders/fshader.glsl");

    // Get the location of the attributes (for glVertexAttribPointer() calls)

    // Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
    glGenVertexArrays(4, &vaoIDs[0]);

    // Game initialization
    newGame();
    // create new next tile

    // set to default
    glBindVertexArray(0);

    glEnable( GL_DEPTH_TEST );
    glClearColor(0, 0, 0, 0);
}

void newGame()
{
    cout << "New Game" << endl;
    cout << "============================================================" << endl;
    
    // Global variable for controlling game logic
    ifGameStop = false;
    ifPause = false;
    
    dropTiles.clear();

    initGrid();
    initBoard();
    initCurrentTile();
    initRobotArm();
    newTile();  
}

void pauseResumeGame()
{
    ifPause = ! ifPause;

    if( true == ifPause )
    {
        cout << "pauseGame() - [Press again to resume]" << endl;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Starts the game over - empties the board, creates new tiles, resets line counters
void restartGame()
{
    newGame();
}


// ================================================================================================================= 
// CallBack Functions

// Draws the game
void processDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat currentFrame = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    deltaTime += (currentFrame - lastFrame);
    lastFrame = currentFrame;

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // Control game frame length

    if (deltaTime > 0.4 && !ifGameStop && dropTiles.empty())
    {
        deltaTime = 0;

        if( !ifPause && !ifGameStop)
        {
            if( tilesReleased == true ){
                if ( false == moveTile(vec2(0.0, velocity )) )
                {

                    setTiles();
                    checkFullRowsAndEliminate();
                    checkFruitMatchAndEliminate();

                    if( dropTiles.empty()){
                        newTile();
                        updateTiles();
                    }
                    else{
                        unsetTiles();
                        updateDropTiles();
                    }

                }
            }
            else
            {
                // Start counting down
            }
        }
    }
    else if(deltaTime > 0.1 && !ifGameStop && !dropTiles.empty())
    {
        deltaTime = 0;
        if( !ifPause && !ifGameStop)
        {
            if( fallTiles( vec2(0.0f, velocity) ))
            {

                checkFullRowsAndEliminate();
                checkFruitMatchAndEliminate(); 

                if( dropTiles.empty()){
                    newTile();
                    updateTiles();
                }
                else{
                    unsetTiles();
                    updateDropTiles();
                }
            }
            
        }
    }


    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // According to the current setting, generate the basic view matrix and projection matrix

    glm::mat4 projection = glm::perspective(glm::radians(myCamera.Fov)                      // convert the Field of View to radians
                                            , float(xsize) / float(ysize), 0.1f, 100.0f);

    glm::mat4 view = myCamera.GetViewMatrix();

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // Display basic FruitTetris

    loc_model       = glGetUniformLocation( UniversalShader, "model" );
    loc_view        = glGetUniformLocation( UniversalShader, "view" ); 
    loc_projection  = glGetUniformLocation( UniversalShader, "projection" );
    

    glUseProgram(UniversalShader);

    // *Important* - Use the universal shader program

    // Place holder
    glUniformMatrix4fv( loc_view,       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv( loc_projection, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 model = glm::mat4();
    glUniformMatrix4fv( loc_model, 1, GL_FALSE, glm::value_ptr(model));

    int boardSize = 0;
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            boardSize += int(board[x][y]);

    glBindVertexArray(vaoIDs[VAO_BOARD]); // Bind the VAO representing the grid cells (to be drawn first)
    glDrawArrays(GL_TRIANGLES, 0, boardSize*TILE_VERTEX_NUM); // Draw the board (10*20*2 = 400 triangles)

    // Calculating the tiles/dropTiles number 
    int tileSize = tiles.size();
    for( vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); it++ ){
        tileSize += it->size();
    }

    glBindVertexArray(vaoIDs[VAO_TILE]); 
    // Bind the VAO representing the current tile (to be drawn on top of the board)
    glDrawArrays(GL_TRIANGLES, 0, tileSize*TILE_VERTEX_NUM); 
    // Draw the current tile (8 triangles)

    glBindVertexArray(vaoIDs[VAO_GRID]); 
    // Bind the VAO representing the grid lines (to be drawn on top of everything else)
    glDrawArrays(GL_LINES, 0, GRID_LINE_VERTEX_NUM); 
    // Draw the grid lines (21+11 = 32 lines)


    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // Display Robot Arm

    // Switch to another shader 
    glUseProgram(RobotArmShader);

    loc_model       = glGetUniformLocation( RobotArmShader, "model" );
    loc_view        = glGetUniformLocation( RobotArmShader, "view" ); 
    loc_projection  = glGetUniformLocation( RobotArmShader, "projection" );


    glBindVertexArray(vaoIDs[VAO_ROBOTARM]);

    glUniformMatrix4fv( loc_view,       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv( loc_projection, 1, GL_FALSE, glm::value_ptr(projection));

    model = glm::mat4();

    myRobotArm.drawBase(model, loc_model);
    
    myRobotArm.drawUpperArm(model, loc_model);

    myRobotArm.drawLowerArm(model, loc_model);
    // draw


    glutPostRedisplay();
    glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void processReshape(GLsizei width, GLsizei height)
{
    xsize = width;
    ysize = height;
    glViewport(0, 0, width, height);

}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void processSpecialKey(int key, int x, int y)
{
    int mod;
    mod = glutGetModifiers();
    // Initial attempt for special key event
    vec2 displacement = vec2(0,0);

    switch(key) {
    case GLUT_KEY_UP :
        rotateTile();
        break;
    case GLUT_KEY_DOWN :
        displacement -= vec2(0, step);
        break;
    case GLUT_KEY_LEFT :
        if ( mod == GLUT_ACTIVE_CTRL )
        {
            myCamera.RotateCamera(1.0f);
        }
        else
            displacement -= vec2(step, 0);

        break;
    case GLUT_KEY_RIGHT:
        if ( mod == GLUT_ACTIVE_CTRL )
        {
            myCamera.RotateCamera(-1.0f);
        }
        else
            displacement += vec2(step, 0);
        
        break;
    }

    if( dropTiles.empty() && !ifPause && !ifGameStop && ( tilesReleased == true ) ){
        if( displacement != vec2(0, 0) && false == moveTile(displacement) )
        {
            if (displacement.y < 0)
            {
                setTiles();
                checkFullRowsAndEliminate();
                checkFruitMatchAndEliminate();

                if( dropTiles.empty())
                    newTile();
                else
                    unsetTiles();

                updateTiles();

            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void processKeyboard(unsigned char key, int x, int y)
{
    int mod;
    mod = glutGetModifiers();
    switch(key) 
    {
        case 033: 
        // Both escape key and 'q' cause the game to exit
            exit(EXIT_SUCCESS);
            break;
        case 'q':
            exit (EXIT_SUCCESS);
            break;
        case 'p':
            pauseResumeGame();
            break;
        case 'r': 
            // 'r' key restarts the game
            restartGame();
            break;

        // Key for controlling Robot Arm
        case 'a':
            myRobotArm.Rotate(INC_THETA, 1.0f);
            break;
        case 'd':
            myRobotArm.Rotate(DEC_THETA, 1.0f);
            break;
        case 'w':
            myRobotArm.Rotate(INC_PHI, 1.0f);
            break;
        case 's':
            myRobotArm.Rotate(DEC_PHI, 1.0f);
            break;

        // Key for change camera's field of view
        case 'z':
            myCamera.ChangeFOV(1.0f);
            break;
        case 'x':
            myCamera.ChangeFOV(-1.0f);
            break;

        // Key for change camera's position
        case 'i':
            myCamera.MoveCamera(FORWARD, deltaTime);
            break;
        case 'k':
            myCamera.MoveCamera(BACKWARD, deltaTime);
            break;
        case 'j':
            myCamera.MoveCamera(LEFT, deltaTime);
            break;
        case 'l':
            myCamera.MoveCamera(RIGHT, deltaTime);
            break;
        case 13:
            releaseTiles();
            break;
        // Shift tile color
        case ' ':
            shiftTileColor();
            break;

        // Test case
        case 'u':
            if( mod == GLUT_ACTIVE_CTRL ) cout << "CTRL + u" << endl;
    }
    glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

bool tryStopGame()
{
    if(checkEndOfGame())
    {
        ifGameStop = true;
        cout << "Game Over..." << endl;
        cout << "Please Press 'r' To Restart..." << endl;
        return true;
    }

    return false;
}

void processIdle()
{
    glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    // initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
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
    // Callback function for timer 

    glutMainLoop(); // Start main loop
    return 0;
}
