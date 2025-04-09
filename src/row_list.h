#ifndef ROW_LIST_H
#define ROW_LIST_H

typedef struct list {
	int* row;
	struct list* next;
} *list_t;

typedef struct {
    int width;
    int node_count;
    int** connections;
    list_t matrix_head;
    list_t matrix_tail;
} *graph_t;

int join_row(graph_t graph, int* new_row);
void print_matrix(graph_t graph, FILE* out);
void free_matrix(graph_t graph);
int init_connections(graph_t graph);
void free_connections(graph_t graph);
void print_connection_matrix(graph_t graph, FILE* out);

#endif