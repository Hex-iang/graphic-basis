#include "FruitTetris.hpp"
#include "Utility.hpp"
#include "Tile.hpp"

// =================================================================================================================
// Tile Controller Function  

//-------------------------------------------------------------------------------------------------------------------
// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updateBoard()
{
    // Update board colors
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(boardVertexColors), boardVertexColors);

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
void initGrid()
{
    // ***Generate geometry data
    point4 gridpoints[64]; 
    // Array containing the 64 points of the 32 total lines ( 21 horizontal line + 11 vertical line ) to be later put in the VBO
    point4 gridcolours[64]; 
    // One colour per vertex
    
    // Vertical lines 
    for (int i = 0; i < 11; i++){
        gridpoints[2*i]     = point4((33.0 + (33.0 * i)), 33.0, 0, 1);
        gridpoints[2*i + 1] = point4((33.0 + (33.0 * i)), 693.0, 0, 1);
    }
    // Horizontal lines
    for (int i = 0; i < 21; i++){
        gridpoints[22 + 2*i]     = point4(33.0, (33.0 + (33.0 * i)), 0, 1);
        gridpoints[22 + 2*i + 1] = point4(363.0, (33.0 + (33.0 * i)), 0, 1);
    }
    // Make all grid lines white
    for (int i = 0; i < 64; i++)
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

//-------------------------------------------------------------------------------------------------------------------
// Init playing board
void initBoard()
{
    // *** Generate the geometric data
    point4 boardpoints[BOARD_WIDTH*BOARD_HEIGHT*3*2];

    // Each cell is a square (2 triangles with 6 vertices)
    for (int i = 0; i < 20; i++){
        for (int j = 0; j < 10; j++)
        {       
            point4 p1 = point4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
            point4 p2 = point4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
            point4 p3 = point4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
            point4 p4 = point4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
            
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardpoints, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    // Grid cell vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_BOARD_COLOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardpoints), boardVertexColors, GL_DYNAMIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, BOARD_WIDTH*BOARD_HEIGHT*3*2*sizeof(point4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    // Current tile vertex colours
    glBindBuffer(GL_ARRAY_BUFFER, vboIDs[VBO_TILE_COLOR]);
    glBufferData(GL_ARRAY_BUFFER, BOARD_WIDTH*BOARD_HEIGHT*3*2*sizeof(color4), NULL, GL_DYNAMIC_DRAW);
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
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
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

    glUniform1i(locxsize, xsize); 
    // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
    glUniform1i(locysize, ysize);

    GLfloat currentFrame = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0;
    deltaTime += (currentFrame - lastFrame);
    lastFrame = currentFrame;

    if (deltaTime > 0.4 && !ifGameStop)
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
            else
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
    }

    glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
    glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)


    // Calculating the tiles/dropTiles number 
    int tileSize = tiles.size();
    for( vector< vector<Tile> >::iterator it = dropTiles.begin(); it != dropTiles.end(); it++ ){
        tileSize += it->size();
    }

    glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
    glDrawArrays(GL_TRIANGLES, 0, tileSize*6); // Draw the current tile (8 triangles)

    glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
    glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)

    glutPostRedisplay();
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
    // Callback function for timer 

    glutMainLoop(); // Start main loop
    return 0;
}
