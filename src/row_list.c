#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "row_list.h"

graph_t init_graph()
{
    graph_t graph = malloc(sizeof(*graph));
    graph->matrix_head = NULL;
    graph->matrix_tail = NULL;
    graph->connections = NULL;
    graph->node_count = 0; //to będzie liczone podczas uzupełniania macierzy
    graph->width = 0;
    return graph;
}

int join_row(graph_t graph, int* new_row)
{
    list_t new_el = malloc(sizeof(*new_el));
    if (!new_el) {
        return 1;
    }
    new_el->row = new_row;
    if (graph->matrix_head == NULL) {
        graph->matrix_head = new_el;
        graph->matrix_tail = new_el;
    }
    else {
    graph->matrix_tail->next = new_el;
    graph->matrix_tail = new_el;
    }
    return 0;
}

void print_matrix(graph_t graph, FILE* out)
{
    list_t list = graph->matrix_head;
    while (list != NULL)
    {
        fprintf(out, "[  ");
        for (int i = 0; i < graph->width; i++)
        {
            fprintf(out, "%d  ", list->row[i]);
        }
        fprintf(out, "] \n");
        list = list->next;
    }
}

void free_matrix(graph_t graph)
{
    list_t list = graph->matrix_head;
    while (list != NULL)
    {
        list_t temp = list;
        list = list->next;
        free(temp);
    }
}

int init_connections(graph_t graph) //zwraca wyzerowaną macierz
{
    if(graph->node_count == 0) {
        printf("Wartość zmiennej node_count równa zero podczas tworzenia macierzy połączeń.\n");
		return 1;
    }
    graph->connections = calloc(graph->node_count, sizeof(int*));
	if(graph->connections == NULL) {
        printf("Nie udało się przypisać pamięci dla tablicy reprezentującej graf.\n");
		return 2;
	}
	for (int i = 0; i < graph->node_count; i++) {
		graph->connections[i] = calloc(graph->node_count, sizeof(int));
		if(graph->connections[i] == NULL){
            printf("Nie udało się przypisać pamięci dla tablicy reprezentującej graf.\n");
			return 3;
		}
	}
    return 0;
}

void free_connections(graph_t graph)
{
    for (int i = 0; i < graph->node_count; i++)
    {
		free(graph->connections[i]);
	}
    free(graph->connections);
}

void print_sorted_connections(graph_t graph, FILE* out)
{
    for (int i = 0; i < graph->node_count; i++)
    {
        for (int j = 0; j < graph->node_count; j++)
        {
            if (graph->connections[i][j] == 1)
            {
                fprintf(out, "%d - %d\n", i, j);
            }
        }
    }
}

void print_connection_table(graph_t graph, FILE* out) //sprawdzenie czy dobrze się zapisały dane w strukturze
{
    for (int i = 0; i < graph->node_count; i++)
    {
        fprintf(out, "[ ");
        for (int j = 0; j < graph->node_count; j++)
        {
            fprintf(out, "%d", graph->connections[i][j]);
        }
        fprintf(out, " ] \n");
    }
}

void free_graph(graph_t graph)
{
    free_connections(graph);
    free_matrix(graph);
}
