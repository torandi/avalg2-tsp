#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <limits.h>
#include <vector>

using namespace std;

int calc_dist(int i, int n);
int total_dist();
void nearest_neighbor();

int **dist;

struct edge_t;

struct node_t {
	float x;
	float y;
	int id;
	edge_t* e[2];

	node_t(int _id) : id(_id) { };
};

struct edge_t {
	node_t* n[2];
	bool out;

	edge_t(node_t* node1, node_t* node2) {
		n[0] = node1;
		n[1] = node2;
		out = 1;
	}

	int cost() {
		return dist[n[0]->id][n[1]->id];
	}

	void print() {
		fprintf(stderr,"e: %i -> %i\n", start_node()->id, end_node()->id); 
	}

	node_t * &start_node() {
		return n[!out];
	}

	node_t * &end_node() {
		return n[out];
	}
};

int num_points;

vector<node_t> nodes;
vector<edge_t> edges;




int main() {

	if(scanf("%i", &num_points)!=1) {
		printf("Invalid input\n");
		exit(1);
	}

	for(int i = 0; i < num_points; ++i) {
		node_t node(i);
		if(scanf("%f %f", &node.x, &node.y)!=2) {
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
			edges.push_back(edge_t(&nodes[cur_node], &nodes[nn]));
			nodes[cur_node].e[1] = &edges.back();
			nodes[nn].e[0] = &edges.back();
			cur_node = nn;
			visited[nn] = 1;
		} else {
			edges.push_back(edge_t(&nodes[cur_node], &nodes[0]));
			nodes[cur_node].e[1] = &edges.back();
			nodes[0].e[0] = &edges.back();
		}
	}

	//Perform opts

	//Output


	fprintf(stderr, "Dist: %i\n", total_dist());
}

int calc_dist(int p1, int p2) {
	return (int)round(sqrt(pow(nodes[p1].x-nodes[p2].x,2)+pow(nodes[p1].y-nodes[p2].y,2)));
}

int total_dist() {
	int sum = 0;
	vector<edge_t>::iterator it;
	for(it = edges.begin(); it != edges.end(); ++it ) {
		it->print();
		sum+=it->cost();
	}	
	return sum;
}
