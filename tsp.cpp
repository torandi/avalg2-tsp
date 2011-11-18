#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <limits.h>

int calc_dist(int i, int n);
int total_dist();
void nearest_neighbor();

struct coordinate {
	float x;
	float y;
};

int num_points;

coordinate * points;



int **dist;
int *tour;

int main() {
	if(scanf("%i", &num_points)!=1) {
		printf("Invalid input\n");
		exit(1);
	}
	points = new coordinate[num_points];
	tour = new int[num_points];

	for(int i = 0; i < num_points; ++i) {
		if(scanf("%f %f", &points[i].x, &points[i].y)!=2) {
			printf("Invalid input\n");
			exit(1);
		}
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
	tour[0] = 0;
	visited[0] = 1;
	for(int i = 0; i < num_points-1; ++i) {	
		float nn_dist = INT_MAX;
		int nn = -1;
		for(int n = 0; n < num_points; ++n) {
			if(visited[n] == 0 && nn_dist > dist[i][n] ) {
				nn_dist = dist[i][n];
				nn = n;
			}
		}
		tour[i+1] = nn;
		visited[nn] = 1;
	}

	//Perform opts

	//Output

	for(int i = 0; i < num_points; ++i) {
		printf("%i\n", tour[i]);
	}

	fprintf(stderr, "Dist: %i\n", total_dist());
}

int calc_dist(int p1, int p2) {
	return (int)round(sqrt(pow(points[p1].x-points[p2].x,2)+pow(points[p1].y-points[p2].y,2)));
}

int total_dist() {
	int sum = dist[tour[num_points-1]][tour[0]];
	for(int i=1; i < num_points; ++i) {
		sum+=dist[tour[i-1]][tour[i]];	
	}
	return sum;
}
