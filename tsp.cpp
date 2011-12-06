#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cassert>
#include <limits.h>
#include <vector>
#include <ctime>

#define SAFE 0
#define EXTREME_DEBUG 0
#define DEBUG 0

#define TIME_LIMIT 1.4 * CLOCKS_PER_SEC

using namespace std;

int calc_dist(int i, int n);
int total_dist();
void nearest_neighbor();
bool two_opt(int e1, int e2);
bool check_path();
#if DEBUG
   void print_edges();
#endif

int **dist;

struct node_t;

struct edge_t {
	node_t* n[2];
	bool out;
   bool changed;
   int _cost;
	edge_t(node_t* node1, node_t* node2);
	int cost();
	void print(bool newline=true);
	node_t * &start_node();
	node_t * &end_node();
	void swap_direction();
	edge_t * next();
};

struct node_t {
	float x;
	float y;
	int id;
	edge_t* e[2];

	node_t(int _id) : id(_id) { };

   /*
    * Returns the edge that one would use to continue a traversing from this node
    * given that one entered on edge in. 
    */
	edge_t * out_edge(edge_t * in) {
		if(in == e[1]) {
			return e[0];
		} else {
         #if SAFE
            assert(in == e[0]);
         #endif
			return e[1];
		}
	};

   /*
    * Perform edge change, replaces org_edge with new_edge
    */
   void change_edge(edge_t * org_edge, edge_t * new_edge) {
      if(e[0] == org_edge) {
         e[0] = new_edge;
      } else {
         #if SAFE
            assert(org_edge == e[1]) ;
         #endif
         e[1] = new_edge;
      }
   }
};


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
	if(newline) {
		fprintf(stderr,"e: %i -> %i\n", start_node()->id, end_node()->id); 
	} else {
		fprintf(stderr,"e: %i -> %i", start_node()->id, end_node()->id); 
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

int num_points;

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

	//Calculate distances and do nearest neigbor
	dist = new int*[num_points];

	char * visited = new char[num_points];

	for(int i = 0; i<num_points; ++i) {
		dist[i] = new int[num_points];
		visited[i] = 0;
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

	//nearest neighbor
	visited[0] = 1;
	int cur_node = 0;
	for(int i = 0; i < num_points; ++i) {	
		float nn_dist = INT_MAX;
		int nn = -1;
		for(int n = 0; n < num_points; ++n) {
			if(visited[n] == 0 && nn_dist > dist[i][n] ) {
				nn_dist = dist[i][n];
				nn = n;
			}
		}
		if(nn!=-1) {
			edges.push_back(new edge_t(nodes[cur_node], nodes[nn]));
			nodes[cur_node]->e[1] = edges.back();
			nodes[nn]->e[0] = edges.back();
			cur_node = nn;
			visited[nn] = 1;
		} else {
			edges.push_back(new edge_t(nodes[cur_node], nodes[0]));
			nodes[cur_node]->e[1] = edges.back();
			nodes[0]->e[0] = edges.back();
		}
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
#endif

bool two_opt(int e1, int e2) {

	edge_t *t1 = edges[e1];
	edge_t *t2 = edges[e2];

	if(t1->start_node() == t2->start_node() || t1->end_node() == t2->end_node()) 
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
