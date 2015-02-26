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
                point4 p1 = point4(START_POINT_X + 33.0 + (x * 33.0), START_POINT_Y + 33.0  + (y * 33.0), START_POINT_Z + 33, 1);
                point4 p2 = point4(START_POINT_X + 33.0 + (x * 33.0), START_POINT_Y + 66.0  + (y * 33.0), START_POINT_Z + 33, 1);
                point4 p3 = point4(START_POINT_X + 66.0 + (x * 33.0), START_POINT_Y + 66.0  + (y * 33.0), START_POINT_Z + 33, 1);
                point4 p4 = point4(START_POINT_X + 66.0 + (x * 33.0), START_POINT_Y + 33.0  + (y * 33.0), START_POINT_Z + 33, 1);

#ifdef _3DGAME 
                point4 p5 = point4(START_POINT_X + 33.0 + (x * 33.0), START_POINT_Y + 33.0  + (y * 33.0), START_POINT_Z - .0, 1);
                point4 p6 = point4(START_POINT_X + 33.0 + (x * 33.0), START_POINT_Y + 66.0  + (y * 33.0), START_POINT_Z - .0, 1);
                point4 p7 = point4(START_POINT_X + 66.0 + (x * 33.0), START_POINT_Y + 66.0  + (y * 33.0), START_POINT_Z - .0, 1);
                point4 p8 = point4(START_POINT_X + 66.0 + (x * 33.0), START_POINT_Y + 33.0  + (y * 33.0), START_POINT_Z - .0, 1);

                quad( &newPoints[ 0*QUAD_VERTEX_NUM], p1, p2, p3, p4);
                quad( &newPoints[ 1*QUAD_VERTEX_NUM], p1, p5, p6, p2);
                quad( &newPoints[ 2*QUAD_VERTEX_NUM], p1, p5, p8, p4);
                quad( &newPoints[ 3*QUAD_VERTEX_NUM], p2, p6, p7, p3);
                quad( &newPoints[ 4*QUAD_VERTEX_NUM], p3, p4, p8, p7);
                quad( &newPoints[ 5*QUAD_VERTEX_NUM], p5, p6, p7, p8);         
#else
                // point4 newPoints[TILE_VERTEX_NUM] = {p1, p2, p3, p2, p3, p4};
                quad( newPoints, p1, p2, p3, p4);
#endif                       
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
// Init back grid of the board
#ifdef _3DGAME 

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
            point4 p1 = point4(START_POINT_X + 33.0 + (j * 33.0), START_POINT_Y + 33.0  + (i * 33.0), START_POINT_Z + 33.3, 1);
            point4 p2 = point4(START_POINT_X + 33.0 + (j * 33.0), START_POINT_Y + 66.0  + (i * 33.0), START_POINT_Z + 33.3, 1);
            point4 p3 = point4(START_POINT_X + 66.0 + (j * 33.0), START_POINT_Y + 66.0  + (i * 33.0), START_POINT_Z + 33.3, 1);
            point4 p4 = point4(START_POINT_X + 66.0 + (j * 33.0), START_POINT_Y + 33.0  + (i * 33.0), START_POINT_Z + 33.3, 1);
            point4 p5 = point4(START_POINT_X + 33.0 + (j * 33.0), START_POINT_Y + 33.0  + (i * 33.0), START_POINT_Z - .3, 1);
            point4 p6 = point4(START_POINT_X + 33.0 + (j * 33.0), START_POINT_Y + 66.0  + (i * 33.0), START_POINT_Z - .3, 1);
            point4 p7 = point4(START_POINT_X + 66.0 + (j * 33.0), START_POINT_Y + 66.0  + (i * 33.0), START_POINT_Z - .3, 1);
            point4 p8 = point4(START_POINT_X + 66.0 + (j * 33.0), START_POINT_Y + 33.0  + (i * 33.0), START_POINT_Z - .3, 1);

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
#else
void initGrid()
{
    // ***Generate geometry data
    point4 gridpoints[GRID_LINE_VERTEX_NUM]; 
    // Array containing the 64 points of the 32 total lines ( 21 horizontal line + 11 vertical line ) to be later put in the VBO
    point4 gridcolours[GRID_LINE_VERTEX_NUM]; 
    // One colour per vertex
  
    // Vertical lines 
    for (int i = 0; i < 11; i++){
        gridpoints[2*i]     = point4((START_POINT_X + 33.0 + (33.0 * i)), START_POINT_Y + 33.0,    START_POINT_Z + .4, 1);
        gridpoints[2*i + 1] = point4((START_POINT_X + 33.0 + (33.0 * i)), START_POINT_Y + 693.0,   START_POINT_Z + .4, 1);
    }
    // Horizontal lines
    for (int i = 0; i < 21; i++){
        gridpoints[22 + 2*i]     = point4(START_POINT_X + 33.0,     (START_POINT_Y + 33.0 + (33.0 * i)), START_POINT_Z - .4, 1);
        gridpoints[22 + 2*i + 1] = point4(START_POINT_X + 363.0,    (START_POINT_Y + 33.0 + (33.0 * i)), START_POINT_Z - .4, 1);
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
#endif

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

    // Grid cell vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_POSITION]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardpoints, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    // Grid cell vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardVertexColors, GL_DYNAMIC_DRAW);
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
    // GLuint program = InitShader("vshader3d.glsl", "fshader3d.glsl");
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");

    model_view = glGetUniformLocation( program, "model_view" );

    glUseProgram(program);

    // Get the location of the attributes (for glVertexAttribPointer() calls)
    vPosition = glGetAttribLocation(program, "vPosition");
    vColor    = glGetAttribLocation(program, "vColor");

    // Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
    glGenVertexArrays(3, &vaoIDs[0]);

    // The location of the uniform variables in the shader program
    locxsize = glGetUniformLocation(program, "xsize"); 
    locysize = glGetUniformLocation(program, "ysize");

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
    glUniform1i(locxsize, xsize); 
    // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
    glUniform1i(locysize, ysize);

    GLfloat currentFrame = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0;
    deltaTime += (currentFrame - lastFrame);
    lastFrame = currentFrame;

    if (deltaTime > 0.4 && !ifGameStop && dropTiles.empty())
    {
        deltaTime = 0;

        if( !ifPause && !ifGameStop)
        {
            if( dropTiles.empty()){
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
        }
    }
    else if(deltaTime > 0.1 && !ifGameStop && !dropTiles.empty())
    {
        deltaTime = 0;
        if( !ifPause && !ifGameStop)
        {
            if( fallTiles( vec2(0.0, velocity) ))
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

#ifdef _3DGAME

    point4  eye( eyex, eyey, eyez, 1.0 );
    point4  at( atx, aty, atz, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  mv = LookAt( eye, at, up );
    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );

#else
    mat4  mv = LookAt( point4(1.0, 1.0, eyez, 1.0),
        point4(1.0, 1.0, 0, 1.0), 
        vec4(0.0, 1.0, 0.0, 0.0 ));

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );

#endif

    int boardSize = 0;
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            boardSize += int(board[x][y]);

    glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
    glDrawArrays(GL_TRIANGLES, 0, boardSize*TILE_VERTEX_NUM); // Draw the board (10*20*2 = 400 triangles)

    // Calculating the tiles/dropTiles number 
    int tileSize = tiles.size();
    for( vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); it++ ){
        tileSize += it->size();
    }

    glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
    glDrawArrays(GL_TRIANGLES, 0, tileSize*TILE_VERTEX_NUM); // Draw the current tile (8 triangles)

    glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
    glDrawArrays(GL_LINES, 0, GRID_LINE_VERTEX_NUM); // Draw the grid lines (21+11 = 32 lines)

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
        displacement -= vec2(step, 0);
        break;
    case GLUT_KEY_RIGHT:
        displacement += vec2(step, 0);
        break;
    }

    if( dropTiles.empty() && !ifPause && !ifGameStop ){
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
#ifdef _3DGAME
        case 'w': 
            eyex    += 0.01;
            cout << "================================================" << endl;
            cout << "eyex: " << eyex <<", eyey: " << eyey << ", eyez: " << eyez << endl;
            cout << "Window - sizex: " << xsize <<", sizey: " << ysize << endl;
            break;
        case 's': 
            eyex    -= 0.01;
            cout << "================================================" << endl;
            cout << "eyex: " << eyex <<", eyey: " << eyey << ", eyez: " << eyez << endl;
            cout << "Window - sizex: " << xsize <<", sizey: " << ysize << endl;
            break;
        case 'a': 
            eyey    += 0.01;
            cout << "================================================" << endl;
            cout << "eyex: " << eyex <<", eyey: " << eyey << ", eyez: " << eyez << endl;
            cout << "Window - sizex: " << xsize <<", sizey: " << ysize << endl;
            break;
        case 'd': 
            eyey    -= 0.01;
            cout << "================================================" << endl;
            cout << "eyex: " << eyex <<", eyey: " << eyey << ", eyez: " << eyez << endl;
            cout << "Window - sizex: " << xsize <<", sizey: " << ysize << endl;
            break;
        case 'z': 
            eyez    += 0.01;
            cout << "================================================" << endl;
            cout << "eyex: " << eyex <<", eyey: " << eyey << ", eyez: " << eyez << endl;
            cout << "Window - sizex: " << xsize <<", sizey: " << ysize << endl;
            break;
        case 'x': 
            eyez    -= 0.01;
            cout << "================================================" << endl;
            cout << "eyex: " << eyex <<", eyey: " << eyey << ", eyez: " << eyez << endl;
            cout << "Window - sizex: " << xsize <<", sizey: " << ysize << endl;
            break;
         case '2':
            eyex    = 1.0;
            eyey    = 1.0;
            eyez    = .7;
            break;
        case '3':
            eyex = 0.95;
            eyey   = 0.95;
            eyez = 0.25;
            break;  
#endif
        case ' ':
            shiftTileColor();
            break;
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
