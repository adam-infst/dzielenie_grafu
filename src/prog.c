#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "row_list.h"

//#define i04 for(int i = 0; i < 5; i++)

// cd Desktop/uhmm3
// prog.exe graff.csrrg
    //printf(" [] "); scanf("%d", &argc);;;

int main(int argc, char** argv)
{
    FILE* in = fopen(argv[1], "r");
    if (in == NULL) {
        printf("Nie znalezionio pliku \n");
        return -1;
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
    graph_t graph = malloc(sizeof(graph));
    graph->head = NULL;
    graph->tail = NULL;
    fseek(in, line[0], SEEK_SET);
    fscanf(in, "%d", &graph->width);
    line[0] = ftell(in);
    int last;
    char c;
    fseek(in, line[2], SEEK_SET); // ustawia kursor w pliku "in" na ostatnie zapisane miejsce w linii 3
    fscanf(in, "%d%c", &last, &c); // wczytuje liczbę i ";" (lub "\n") po niej
    line[2] = ftell(in); // zapisuje aktualne położenie kursora na trzeciej linii
    //printf("last= %d, c= %c \n", last, c);//pomocnicze
    if (last != 0 || c != ';') {
        printf("Zły format danych: last= %d, c= %c \n", last, c);
        return -2;
    }
    //int counter = 1;//pomocnicze
    while(c != '\n' && c != EOF)
    {
        //printf("\nwiersz %d:\n", counter++);//pomocnicze
        int* row = malloc(graph->width * sizeof(int)); //przy każdej iteracji przypisuje nowy obszar pamięci na nowy rząd
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
                //printf("%d: %d, ", current_idx, row[current_idx]);//pomocnicze
                current_idx++;
            }
            row[current_idx] = 1;
            //printf("%d: %d \n", current_idx, row[current_idx]);//pomocnicze
            current_idx++;
        }
        join_row(graph, row);
        last = next;
    }


    printf("[%s] \n", argv[1]);
    print_matrix(graph, stdout);
    
    free_matrix(graph);
    fclose(in);
    return 0;
}



