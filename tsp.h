#ifndef NODE_T
#define NODE_T
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

	node_t(const node_t &n);
	node_t(int _id);
	edge_t * out_edge(edge_t * in);
   void change_edge(edge_t * org_edge, edge_t * new_edge);

};

#endif
