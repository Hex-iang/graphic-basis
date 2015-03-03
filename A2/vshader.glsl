#version 130

in vec4 vPosition;
in vec4 vColor;
out vec4 color;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() 
{

	// First, center the image by translating each vertex by half of the original window size
	// Then, multiply by the scale matrix to maintain size after the window is reshaped
	gl_Position = projection * view * model * vPosition;
	// gl_Position = projection * view * vPosition;

	color = vColor;	
} 
