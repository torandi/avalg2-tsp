#ifndef RENDER_H
#define RENDER_H
#include "tsp.h"
#include <vector>

void render_init(int w, int h, int map_min_w, int map_min_h, int map_w, int map_h);
void render(const std::vector<node_t*> &nodes, const std::vector<edge_t*> &edges);
void poll(bool *run);

#endif /* render_H */
