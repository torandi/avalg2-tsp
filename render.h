#ifndef RENDER_H
#define RENDER_H
#include "tsp.h"
#include <vector>

void render_init(int w, int h, int map_w, int map_h);
void render(const std::vector<node_t*> &nodes, const std::vector<edge_t*> &edges);

#endif /* render_H */
