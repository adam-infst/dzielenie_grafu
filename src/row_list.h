#ifndef ROW_LIST_H
#define ROW_LIST_H

typedef struct list {
	int* row;
	struct list* next;
} *list_t;

typedef struct {
    int width;
    list_t head;
    list_t tail;
} *graph_t;

int join_row(graph_t graph, int* new_row);
void print_matrix(graph_t graph, FILE* out);
void free_matrix(graph_t graph);

#endif