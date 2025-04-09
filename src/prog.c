#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "row_list.h"

//printf(" [1] "); scanf("%d", &argc);;;

int main(int argc, char** argv)
{
    FILE* in = fopen(argv[1], "r");
    FILE* out = stdout; //jescze nie wersja ostateczna
    if (in == NULL || out == NULL) {
        printf("Nie znalezionio pliku \n");
        return 1;
    }
    long line[5]; // wskażniki na miejsce w danym wierszu, long jest potrzebny dla ftell i fseek
    for (int i = 0; i < 5; i++)
    {
        line[i] = ftell(in); //ważne: wiersze 1-5 z pliku mają indeksy 0-4
        int c = fgetc(in);
        while(c != '\n' && c != EOF)
        {
            c = fgetc(in);
        }
    }
    graph_t graph = malloc(sizeof(*graph));
    graph->matrix_head = NULL;
    graph->matrix_tail = NULL;
    graph->connections = NULL;
    graph->node_count = 0; //to będzie liczone podczas uzupełniania macierzy

    fseek(in, line[0], SEEK_SET);
    fscanf(in, "%d", &graph->width);
    line[0] = ftell(in);
    
    int last;
    char c;
    fseek(in, line[2], SEEK_SET);   // ustawia kursor w pliku "in" na ostatnie zapisane miejsce w linii 3
    fscanf(in, "%d%c", &last, &c);  // wczytuje liczbę i ";" (lub "\n") po niej
    line[2] = ftell(in);            // zapisuje aktualne położenie kursora na trzeciej linii

    if (last != 0 || c != ';') {
        printf("Zły format danych: last= %d, c= %c \n", last, c);
        return 2;
    }
    while(c != '\n' && c != EOF)
    {
        int* row = malloc(graph->width * sizeof(int)); //przy każdej iteracji przypisuje nowy obszar pamięci na nowy rząd
        if(row == NULL) {
            printf("Błąd alokacji pamięci.\n");
            return 3;
        }
        int current_idx = 0;
        int next;
        fseek(in, line[2], SEEK_SET);
        fscanf(in, "%d%c", &next, &c);
        line[2] = ftell(in);
        for (int i = 0; i < next - last; i++) //wyszukuje położenie następnego węzła next-last razy
        {
            int index;
            fseek(in, line[1], SEEK_SET);
            fscanf(in, "%d%c", &index, &c);
            line[1] = ftell(in);
            while(current_idx < index)
            {
                row[current_idx] = 0;
                current_idx++;
            }
            row[current_idx] = 1;
            graph->node_count++;
            current_idx++;
        }
        join_row(graph, row);
        last = next;
    }
    init_connections(graph); //zwraca już wyzerowaną macierz

    print_matrix(graph, out);

    fseek(in, line[4], SEEK_SET); // tu będzie jeszcze zrobiony refactoring
    fscanf(in, "%d%c", &last, &c);
    line[4] = ftell(in);

    if (last != 0 || c != ';') {
        printf("Zły format danych: last= %d, c= %c \n", last, c);
        return 2;
    }
    while(c != '\n' && c != EOF)
    {
        int next;
        fseek(in, line[4], SEEK_SET);
        fscanf(in, "%d%c", &next, &c);
        line[4] = ftell(in);
        
        int current_node;
        fseek(in, line[3], SEEK_SET); // wczytujemy nr węzła
        fscanf(in, "%d%c", &current_node, &c);
        line[3] = ftell(in);
        
        for (int i = 0; i < next - last - 1; i++) //wyszukuje położenie następnego węzła next-last razy
        {
            int index;
            fseek(in, line[3], SEEK_SET);
            fscanf(in, "%d%c", &index, &c);
            line[3] = ftell(in);

            graph->connections[current_node][index] = 1;
            fprintf(out, "%d - %d\n", current_node, index);
        }
        current_node++;
        last = next;
    }

    //print_connection_matrix(graph, out);
    //printf("[%s] \n", argv[1]);
    
    free_connections(graph);
    free_matrix(graph);
    fclose(in);
    return 0;
}




