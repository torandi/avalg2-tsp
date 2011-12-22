#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cassert>
#include <limits.h>
#include <vector>
#include <list>
#include <ctime>
#include <algorithm>


#define SAFE 0
#define EXTREME_DEBUG 0
#define DEBUG 0
#define GFX 0

#define TIME_LIMIT 0.5 * CLOCKS_PER_SEC

using namespace std;

int calc_dist(int i, int n);
int total_dist();
void nearest_neighbor();
bool two_opt(int e1, int e2);
bool check_path();
int graham_scan();

int **dist;
int *min_dist;

int num_points;

#if GFX
	#include "render.h"
	bool run=true;
	void render_loop(int iterations, int mark_nodes=0, int mark_edges=0);
#endif

#if DEBUG
   void print_edges();
#endif

#include "tsp.h"

edge_t::edge_t(node_t* node1, node_t* node2) {
	n[0] = node1;
	n[1] = node2;
	out = 1;
   changed = true;
}

int edge_t::cost() {
   if(changed) {
      _cost = dist[n[0]->id][n[1]->id];
      changed = false;
   }
   return _cost;
}

void edge_t::print(bool newline) {
	fprintf(stderr,"e: %i -> %i", start_node()->id, end_node()->id); 
	if(newline) {
		fprintf(stderr,"\n");
	}
}

node_t * &edge_t::start_node() {
	return n[!out];
}

node_t * &edge_t::end_node() {
	return n[out];
}

void edge_t::swap_direction() {
	out = !out;
}

edge_t * edge_t::next() {
	return end_node()->out_edge(this);
}

node_t::node_t(const node_t &n) {
	printf("copy %i\n", n.id);
	id = n.id;
	e[0] = n.e[0];
	e[1] = n.e[1];
	x = n.x;
	y = n.y;
}

node_t::node_t(int _id) : id(_id) { };

/*
 * Returns the edge that one would use to continue a traversing from this node
 * given that one entered on edge in. 
 */
edge_t * node_t::out_edge(edge_t * in) {
	if(in == e[1]) {
		return e[0];
	} else {
		#if SAFE
			assert(in == e[0]);
		#endif
		return e[1];
	}
}
;
void node_t::print(bool newline) {
	fprintf(stderr,"n: %i (%f, %f)", id, x, y); 
	if(newline) {
		fprintf(stderr,"\n");
	}
}

/*
 * Perform edge change, replaces org_edge with new_edge
 */
void node_t::change_edge(edge_t * org_edge, edge_t * new_edge) {
	if(e[0] == org_edge) {
		e[0] = new_edge;
	} else {
		#if SAFE
			assert(org_edge == e[1]) ;
		#endif
		e[1] = new_edge;
	}
}

vector<node_t*> nodes;
vector<edge_t*> edges;

int main() {

	if(scanf("%i", &num_points)!=1) {
		printf("Invalid input\n");
		exit(1);
	}

	for(int i = 0; i < num_points; ++i) {
		node_t * node = new node_t(i);
		if(scanf("%f %f", &node->x, &node->y)!=2) {
			printf("Invalid input\n");
			exit(1);
		}
		nodes.push_back(node);
	}	

	if(num_points <= 2) {
		for(int i=0; i < num_points; ++i) {
			printf("%i\n", i);
		}
		exit(0);
	}

	//Calculate distances
	dist = new int*[num_points];

	char * visited = new char[num_points];

#if GFX
	int min_w = INT_MAX;
	int min_h = INT_MAX;
   int map_w = 0;
   int map_h = 0;
#endif

	for(int i = 0; i<num_points; ++i) {
		dist[i] = new int[num_points];
		visited[i] = 0;
#if GFX
		if(min_w > nodes[i]->x)
			min_w = nodes[i]->x;

		if(min_h > nodes[i]->y)
			min_h = nodes[i]->y;

      if(map_w < nodes[i]->x)
         map_w = nodes[i]->x;

      if(map_h < nodes[i]->y)
         map_h = nodes[i]->y;
#endif
		for(int n = 0; n < num_points; ++n) {
			if(n < i) {
				dist[i][n] = dist[n][i];
			} else if(n == i) {
				dist[i][i] = 0;
			} else {
				dist[i][n] = calc_dist(i,n);
			}
		}
	}

#if GFX
	render_init(800,600,min_w, min_h, map_w, map_h);
#endif

	//find the convex hull
	int M = graham_scan();
	//int M = 2;

	for(int i=1; i<M; ++i) {
		edges.push_back(new edge_t(nodes[i-1], nodes[i]));
	}
	edges.push_back(new edge_t(nodes[M-1], nodes[0]));

#if GFX
		render_loop(10000,M);
#endif

#if DEBUG
	fprintf(stderr,"Graham scan complete\n");
#endif


	list<node_t*> remaining;

	 min_dist  = new int[nodes.size()];

	//Insert remaining into remaining list
	for(int i=M; i<nodes.size(); ++i) {
		remaining.push_back(nodes[i]);
		int *n_dist = dist[nodes[i]->id];
		int min=n_dist[0];
		for(int n=1;n<M; ++n) {
			if(n_dist[n] < min) {
				min = n_dist[n];
			}
		}
		min_dist[nodes[i]->id] = min;
	}
	
	while(remaining.size() > 0) {
#if GFX
		render_loop(1000);
#endif
		node_t* best=remaining.front();
		list<node_t*>::iterator best_node_it = remaining.begin(); 
		for(list<node_t*>::iterator it=remaining.begin(); it!=remaining.end();++it) {
			if(min_dist[(*it)->id] > min_dist[best->id]) {
				best = *it;
				best_node_it = it;
			}
		}
		
		//Find best place to insert:
		int min_incr = INT_MAX;
		edge_t * best_edge = NULL; 
		int * n_dist = dist[best->id];
		for(vector<edge_t*>::iterator it=edges.begin(); it != edges.end(); ++it) {
			int incr = n_dist[(*it)->start_node()->id]+n_dist[(*it)->end_node()->id]-(*it)->cost();
			if(incr < min_incr) {
				min_incr = incr;
				best_edge = (*it);
			}
		}
		//Modify edge
		edge_t * ne = new edge_t(best, best_edge->end_node());
		best_edge->end_node() = best;
		best_edge->changed = true;
		edges.push_back(ne);

		//Remove edge from remaining list
		remaining.erase(best_node_it);

		//Update min costs for remaining:
		for(list<node_t*>::iterator it=remaining.begin(); it!=remaining.end();++it) {
			if(n_dist[(*it)->id] < min_dist[(*it)->id]) {
				min_dist[(*it)->id] = n_dist[(*it)->id];
			}
		}
#if GFX
		render_loop(1000);
#endif
	}

	//Set the correct edges in the nodes
	for(vector<edge_t*>::iterator it=edges.begin(); it!=edges.end(); ++it) {
		(*it)->start_node()->e[1] = *it;
		(*it)->end_node()->e[0] = *it;
	}

   #if EXTREME_DEBUG
      print_edges();
   #endif 

   #if SAFE
	   assert(check_path());
   #endif

   do {
      for(int e1=0; e1<edges.size(); ++e1) {
         for(int e2 = 0; e2 < edges.size(); ++e2) {
            two_opt(e1, e2);
         }
      }
   } while(clock() < TIME_LIMIT);

#if GFX
		render_loop(-1);
#endif

	//Output

	edge_t* cur_edge = edges[0];
   printf("%d\n", cur_edge->start_node()->id);
   int sum = 0;
	for(; cur_edge->end_node() != edges[0]->start_node(); cur_edge = cur_edge->next()) {
      sum += cur_edge->cost();
      printf("%d\n", cur_edge->end_node()->id);
	}
   sum += cur_edge->cost(); //Add the last one
   
   fprintf(stderr, "Dist: %i\n", sum);
}

int calc_dist(int p1, int p2) {
	return (int)round(sqrt(pow(nodes[p1]->x-nodes[p2]->x,2)+pow(nodes[p1]->y-nodes[p2]->y,2)));
}


bool check_path() {
	bool * visited = new bool[num_points];
	memset(visited, 0, num_points*sizeof(bool));
	edge_t * cur_edge = edges[0];
   #if EXTREME_DEBUG
      fprintf(stderr,"-------------------------\n");
   #endif
	for(;!visited[cur_edge->start_node()->id] && cur_edge->end_node() != edges[0]->start_node(); cur_edge = cur_edge->next()) {
		visited[cur_edge->start_node()->id] = true;
      #if EXTREME_DEBUG
         cur_edge->print();
      #endif
	}
	bool full_path = (cur_edge->end_node() == edges[0]->start_node());
   #if EXTREME_DEBUG
      cur_edge->print();
      fprintf(stderr, "v: %d, en==sn = %d\n", visited[cur_edge->start_node()->id], cur_edge->end_node() == edges[0]->start_node());
      if(full_path)
         fprintf(stderr,"-----------correct-------\n");
      else 
         fprintf(stderr,"---------incorrect-------\n");
   #endif

	return full_path;
}

#if DEBUG
void print_edges() {
	fprintf(stderr, "Edges: \n");
	for(vector<edge_t*>::iterator it=edges.begin(); it!=edges.end();++it) {
		(*it)->print();
	}
}

void print_nodes() {
	fprintf(stderr, "Nodes: \n");
	for(vector<node_t*>::iterator it=nodes.begin(); it!=nodes.end();++it) {
		(*it)->print();
	}
}
#endif

bool two_opt(int e1, int e2) {

	edge_t *t1 = edges[e1];
	edge_t *t2 = edges[e2];

	if(t1->start_node() == t2->end_node() || t1->end_node() == t2->start_node()) 
       //Don't opt, edges have common node
		return true;

	int old_cost = t1->cost() + t2->cost();


	/**
	* Swap nodes 
	*/
	edge_t *t1n = new edge_t(t1->start_node(), t2->start_node());
	edge_t *t2n = new edge_t(t1->end_node(), t2->end_node());


	int new_cost = (t1n->cost()+t2n->cost());
	if(old_cost <= new_cost) {
      delete t1n;
      delete t2n;
		return true;
	}
   #if DEBUG
      fprintf(stderr,"Swap edges from: { \n");
      t1->print(false);
      fprintf(stderr,", ");
      t2->print(false);
      fprintf(stderr,"}\n");
      fprintf(stderr, "to: edges: {");
      t1n->print(false);
      fprintf(stderr,", ");
      t2n->print(false);
      fprintf(stderr,"}\n");

      fprintf(stderr,"Improvment: %d\n",old_cost-new_cost);
   #endif

	/**
	* Cycle through nodes until end node is  t2->end_node() 
	*/
   edge_t * cur_edge = NULL;
	edge_t * next_edge = t1->next();

	while(next_edge->end_node() != t2->end_node()) {
      cur_edge = next_edge;
      next_edge = cur_edge->next();
      #if EXTREME_DEBUG
         fprintf(stderr, "Swap edge ");
         cur_edge->print();
      #endif
		cur_edge->swap_direction();
	}

	//Perform the real swap and delete the old edges
   /*
    * What we do is to move the end of t1 to the start of t2
    * and then move the start of t2 to the end of t1
    */

   //Swap edge pointers in nodes
   t1->end_node()->change_edge(t1, t2);
   t2->start_node()->change_edge(t2, t1);

   //Swap nodes in edges
	t1->end_node() = t1n->end_node();
	t2->start_node() = t2n->start_node();

   //Mark changed
   t1->changed = true;
   t2->changed = true;

   //delete tmp variables

	delete t1n;
	delete t2n;
	
	return true;

}

float * cos_val;

bool node_cos_compare(node_t * n1, node_t * n2) {
	return cos_val[n1->id] > cos_val[n2->id];
}

/* Three points are a counter-clockwise turn if ccw > 0, clockwise if
 * ccw < 0, and collinear if ccw = 0 because ccw is a determinant that
 * gives the signed area of the triangle formed by p1, p2 and p3.
 */
int ccw(node_t * p1, node_t* p2,node_t* p3) {
	if(p1->x == p2->x && p1->x == p3->x)
		return 1;
	if(p1->y == p2->y && p1->y == p3->y)
		return 1;
	return (p2->x - p1->x)*(p3->y - p1->y) - (p2->y - p1->y)*(p3->x - p1->x);
}

int graham_scan() {
	node_t * p = nodes.front();
	for(vector<node_t*>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
		if((*it)->y > p->y ||  ( (*it)->y == p->y && (*it)->x < p->x )) {
			p = *it;
		} /* else if((*it)->y == p.y && (*it)->x == p.x)  // Maybe we should care? */
	}

	cos_val = new float[nodes.size()];
	int * p_dist = dist[p->id];
	for(vector<node_t*>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
		if(p_dist[(*it)->id] > 0) {
			cos_val[(*it)->id] = (float)(p->x-(*it)->x)/p_dist[(*it)->id];
      } else {
         cos_val[(*it)->id] = 0;
		}
	}
	cos_val[p->id] = 2;

	std::sort(nodes.begin(), nodes.end(), node_cos_compare);

	int M = 2; //Number of nodes in convex hull
	
	node_t * tmp;

#if DEBUG
	fprintf(stderr, "Sorted nodes. P: %i\n", p->id);
	for(vector<node_t*>::iterator it=nodes.begin(); it!=nodes.end();++it) {
		(*it)->print(false);
      fprintf(stderr, " - cos: %f, p_dist: %i\n", cos_val[(*it)->id], p_dist[(*it)->id]);
	}
#endif


	delete[] cos_val;

	for(int i=2; i<nodes.size(); ++i) {
		while (i < nodes.size() && ccw(nodes[M-2], nodes[M-1],nodes[i]) <= 0)  {
			if(M == 2) {
#if DEBUG
				fprintf(stderr,"M==2, move %i -> %i\n", M-1, i);
#endif
				tmp = nodes[M-1];
				nodes[M-1] = nodes[i];
				nodes[i] = tmp;
				++i;
			} else {
				--M;
#if DEBUG
				fprintf(stderr,"--M, M = %i\n", M);
#endif
			}
		}
		if(i >= nodes.size())
			break;
		++M;
#if DEBUG
		printf(stderr,"++M, move %i -> %i\n", M-1, i);
#endif
		tmp = nodes[M-1];
		nodes[M-1] = nodes[i];
		nodes[i] = tmp;
	}

	return M;
}

#if GFX
void render_loop(int iterations, int mark_nodes, int mark_edges) {
	while(run && iterations!=0) {
		--iterations;
		render(nodes, edges, mark_nodes, mark_edges);
		poll(&run);
	}
}
#endif
