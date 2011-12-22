#include "render.h"
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <math.h>


void render_init(int w, int h, int map_w, int map_h){
  /* create window */
  SDL_Init(SDL_INIT_VIDEO);
  int flags = SDL_OPENGL | SDL_DOUBLEBUF;
  SDL_SetVideoMode(w, h, 0, flags);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* orthographic projection */
  glOrtho(0, map_w, 0, map_h, -1.0, 1.0);
  glScalef(1, -1, 1);
  glTranslated(0, -h, 0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, w, h);

  /* setup opengl */
  glClearColor(0,0,0,1);
  glColor3f(1,1,1);
  glPointSize(5);
}

void render(const std::vector<node_t*> &nodes, const std::vector<edge_t*> &edges) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for(std::vector<node_t*>::const_iterator it=nodes.begin(); it!=nodes.end(); ++it) {
		glVertex3f((*it)->x, (*it)->y, 0);
	}
	glEnd();
	SDL_GL_SwapBuffers();
}
