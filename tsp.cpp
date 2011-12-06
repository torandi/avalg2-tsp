#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cassert>
#include <limits.h>
#include <vector>

using namespace std;

int calc_dist(int i, int n);
int total_dist();
void nearest_neighbor();
bool two_opt(int e1, int e2);
bool check_path();

int **dist;

struct node_t;

struct edge_t {
	node_t* n[2];
	bool out;
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

	edge_t * out_edge(edge_t * in) {
		if(in == e[1]) {
			return e[0];
		} else {
			return e[1];
		}
	}
};


edge_t::edge_t(node_t* node1, node_t* node2) {
	n[0] = node1;
	n[1] = node2;
	out = 1;
}

int edge_t::cost() {
	return dist[n[0]->id][n[1]->id];
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

	assert(check_path());

	//Perform opts
	
	for(int i=0;i<1000;++i) {
		int e1, e2;
		e1 = rand() % edges.size();
		e2 = rand() % edges.size();
		two_opt(e1, e2);
		assert(check_path());
	}

	//Output
/*
	for(vector<node_t*>::iterator it=nodes.begin(); it!=nodes.end() ; ++it) {
		fprintf(stderr,"{ ");
		if((*it)->e[0] != NULL) {
			(*it)->e[0]->print(false);
		}
		fprintf(stderr," , ");
		if((*it)->e[1] != NULL) {
			(*it)->e[1]->print(false);
		}
		fprintf(stderr," }\n");
	}*/

	node_t* current_node = nodes[0];
	edge_t* last_edge = NULL;
	if(current_node->out_edge(last_edge)->end_node() == nodes[0]) {
		last_edge = current_node->out_edge(last_edge);
	}
	do {
		edge_t * next_edge = current_node->out_edge(last_edge);
		current_node = next_edge->end_node();
	} while(current_node != nodes[0]);

	fprintf(stderr, "Dist: %i\n", total_dist());
}

int calc_dist(int p1, int p2) {
	return (int)round(sqrt(pow(nodes[p1]->x-nodes[p2]->x,2)+pow(nodes[p1]->y-nodes[p2]->y,2)));
}

int total_dist() {
	int sum = 0;
	vector<edge_t*>::iterator it;
	for(it = edges.begin(); it != edges.end(); ++it ) {
		sum+=(*it)->cost();
	}	
	return sum;
}

bool check_path() {
	bool * visited = new bool[num_points];
	memset(visited, 0, num_points*sizeof(bool));
	edge_t * cur_edge = edges[0];
	fprintf(stderr,"-------------------------\n");
	for(;!visited[cur_edge->start_node()->id] && cur_edge->end_node() != edges[0]->start_node(); cur_edge = cur_edge->next()) {
		visited[cur_edge->start_node()->id] = true;
		cur_edge->print();
	}
	cur_edge->print();
	fprintf(stderr, "v: %d, en==sn = %d\n", visited[cur_edge->start_node()->id], cur_edge->end_node() == edges[0]->start_node());
	bool full_path = (cur_edge->end_node() == edges[0]->start_node());
	if(full_path)
		fprintf(stderr,"-----------correct-------\n");
	else 
		fprintf(stderr,"---------incorrect-------\n");

	return full_path;
}
/*
void print_edges() {
	fprintf(stderr, "Edges: \n");
	for(vector<edge_t*>::iterator it=edges.begin(); ++it) {
		(*it)->print();
	}
}*/

bool two_opt(int e1, int e2) {

	edge_t *t1 = edges[e1];
	edge_t *t2 = edges[e2];

	if(t1->start_node() == t2->start_node() || t1->end_node() == t2->end_node()) 
		return false; //Don't opt, edges have common node

	int old_cost = t1->cost() + t2->cost();


	/**
	* Swap nodes 
	*/
	edge_t *t1n = new edge_t(t1->start_node(), t2->start_node());
	edge_t *t2n = new edge_t(t1->end_node(), t2->end_node());

	fprintf(stderr,"Swap edges from: { \n");
	t1->print(false);
	fprintf(stderr,", ");
	t2->print(false);
	fprintf(stderr,"}");

	int new_cost = (t1n->cost()+t2n->cost());
	if(old_cost <= new_cost) {
		fprintf(stderr," - NOPE: +%d\n", new_cost-old_cost);
		return false;
	}
	fprintf(stderr," - Better: -%d\n",old_cost-new_cost);

	/**
	* Cycle through nodes until end node is  t2->end_node() 
	*/
	edge_t * cur_edge = t1->next();
	for(;cur_edge->end_node() != t2->end_node(); cur_edge = cur_edge->next()) {
		cur_edge->swap_direction();
	}

	//Perform the real swap and delete the old edges
	t1->start_node() = t1n->start_node();
	t1->end_node() = t1n->end_node();

	t2->start_node() = t2n->start_node();
	t2->end_node() = t2n->end_node();

	delete t1n;
	delete t2n;
	
	return true;

}
