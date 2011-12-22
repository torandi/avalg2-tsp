#include "render.h"
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <math.h>

static int min_w, min_h;

void render_init(int w, int h, int map_min_w, int map_min_h, int map_w, int map_h) {
	
  map_min_h-=100;
  map_min_w-=100;
  map_h+=100;
  map_w+=100;

  /* create window */
  SDL_Init(SDL_INIT_VIDEO);
  int flags = SDL_OPENGL | SDL_DOUBLEBUF;
  SDL_SetVideoMode(w, h, 0, flags);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* orthographic projection */
  //glOrtho(0, map_w, 0, map_h, -1.0, 1.0);
  glOrtho(0 , (map_w-map_min_w), 0 , (map_h-map_min_h), -1.0, 1.0);
  glScalef(1, -1, 1);
  glTranslated(0, -(map_h-map_min_h), 0);


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, w, h);
  glTranslated(-map_min_w, -map_min_h, 0);
  //glScalef(map_w/(float)w, map_h/(float)h,1);

  /* setup opengl */
  glClearColor(0,0,0,1);
  glPointSize(5);

  min_w = map_min_w;
  min_h = map_min_h;
}

void render(const std::vector<node_t*> &nodes, const std::vector<edge_t*> &edges) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	   glColor3f(1,1,1);
		for(std::vector<node_t*>::const_iterator it=nodes.begin(); it!=nodes.end(); ++it) {
			glVertex2f((*it)->x, (*it)->y);
		}
	glEnd();
	glColor3f(1,0,1);
	glBegin(GL_LINES);
		for(std::vector<edge_t*>::const_iterator it=edges.begin(); it!=edges.end(); ++it) {
			glVertex2f((*it)->start_node()->x, (*it)->start_node()->y);
			glVertex2f((*it)->end_node()->x, (*it)->end_node()->y);
		}
	glEnd();
	SDL_GL_SwapBuffers();
}

void poll(bool* run){
	static int last = 0;

	SDL_Event event;
	while ( SDL_PollEvent(&event) ){
		switch (event.type){
         case SDL_QUIT:
            *run = false;
            break;
		}
	}
}

