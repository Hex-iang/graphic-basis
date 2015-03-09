#pragma once

// -----------------------------------------------------------------------------------------------------------------------------------------------
// Rendering text string to the window using glut bitmap string 
void RenderString(float x, float y, void *font, const char* string, glm::vec4 const& rgb)
{  
  char *c;

  glColor3f(rgb.x, rgb.y, rgb.z); 
  glRasterPos2f(x, y);

  glutBitmapString(font, (unsigned char*)string);
}
