#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "row_list.h"

int join_row(graph_t graph, int* new_row)
{
    list_t new_el = malloc(sizeof(new_el));
    if (!new_el) {
        return 1;
    }
    new_el->row = new_row;
    if (graph->head == NULL) {
        graph->head = new_el;
        graph->tail = new_el;
    }
    else {
    graph->tail->next = new_el;
    graph->tail = new_el;
    }
    return 0;
}

void print_matrix(graph_t graph, FILE* out)
{
    list_t list = graph->head;
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
    list_t list = graph->head;
    while (list != NULL)
    {
        list_t temp = list;
        list = list->next;
        free(temp);
    }
}