// Includes all source codes for robot Arm rendering and controling 
#pragma once
#include "FruitTetris.hpp"

// Shader transformation matrices
mat4  model_view;
GLuint ModelView, Projection;

// Const Parameters Settings that controls the size of the Robot's arm
const GLfloat BASE_HEIGHT      = 2.0;
const GLfloat BASE_WIDTH       = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 5.0;
const GLfloat LOWER_ARM_WIDTH  = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 5.0;
const GLfloat UPPER_ARM_WIDTH  = 0.5;

const int ARMVERTICES = 36;

// Array of rotation angles (in degrees) for each rotation axis
enum { LowerArm = 0, UpperArm = 1, NumAngles = 2 };
int      Axis = LowerArm;
GLfloat  Theta[NumAngles] = { 0.0 };

// void drawBase(glm::mat4 model)
// {
//     glm::mat4 instance = glm::translate( glm::vec3(0.0, 0.5 * BASE_HEIGHT, 0.0) ) * glm::scale( glm::vec3(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH) ) ;

//     glUniformMatrix4fv( loc_model, 1, GL_FALSE, glm::value_ptr( glm::dot(model, instance) ) );

//     glDrawArrays( GL_TRIANGLES, 0, ARMVERTICES );
// }

// void drawUpperArm(glm::mat4 model)
// {
//     glm::mat4 instance = glm::translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) * glm::scale( UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT,UPPER_ARM_WIDTH );
    
//     glUniformMatrix4fv( ModelView, 1, GL_FALSE, glm::value_ptr( model_view * instance) ) );
//     glDrawArrays( GL_TRIANGLES, 0, ARMVERTICES );
// }

// void drawLowerArm(glm::mat4 model)
// {
//     glm::mat4 instance = glm::translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) * glm::scale( LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH );
    
//     glUniformMatrix4fv( ModelView, 1, GL_FALSE, glm::value_ptr( model_view * instance ) );
//     glDrawArrays( GL_TRIANGLES, 0, ARMVERTICES );
// }