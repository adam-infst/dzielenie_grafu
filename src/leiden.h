#ifndef LEIDEN_H
#define LEIDEN_H

#include "row_list.h"

typedef struct L
{
	int **group;
	int m;// aMount of connections
	int *g_nc;//group node count
	int *ng;//node group
	float Q;//modularity
	int cg;//curr. group
	int mg;//move group
	int *n_cc;//node connection count
	
}*L_t;

void init_L(int N,L_t L);
float Q_count(int N, L_t L, graph_t graph);
void move_group(int N, L_t L);
void set_L_value(int N,L_t L1, L_t L2);
void free_L(int N, L_t L);
void Leiden(int N, L_t L1, graph_t graph);

#endif
