#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "row_list.h"
#include "leiden.h"

//printf(" [1] "); scanf("%d", &argc);;;

int main(int argc, char** argv)
{
    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen("graph_to_matrix.txt", "w");

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

    graph_t graph = init_graph();

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
    while(c != '\n')
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
    int scanned = 2;
    while(scanned == 2) //uwaga linia 5 kończy się wcześniej niż linia 4
    {
        int next;
        fseek(in, line[4], SEEK_SET);
        scanned = fscanf(in, "%d%c", &next, &c); //z jakiegoś powodu fscanf %d%c nie umie wczytać EOF do char więc nie da się zrobić pętli z takim warunkiem
        line[4] = ftell(in);
        //ustawiamy scanned na początku pętli, więc logika w środku wykona się raz dla scanned != 2, ale tutaj akurat chcemy żeby tak było
        //printf(" next= %d  last= %d  c5= %c  scanned= %d\n", next, last, c, scanned);;;

        int current_node;
        fseek(in, line[3], SEEK_SET);
        fscanf(in, "%d%c", &current_node, &c);
        line[3] = ftell(in);
        
        for (int i = 0; i < next - last - 1; i++)
        {
            int index;
            fseek(in, line[3], SEEK_SET);
            fscanf(in, "%d%c", &index, &c);
            line[3] = ftell(in);

            graph->connections[current_node][index] = 1;
            graph->connections[index][current_node] = 1;
			fprintf(out, "%d - %d\n", current_node, index);
        }
        last = next;
    }
	//skończyła się już linia 5, ale trzeba jeszcze doczytać do końca linię 4
    int last_node;
    fseek(in, line[3], SEEK_SET);
    fscanf(in, "%d%c", &last_node, &c);
    line[3] = ftell(in);
        
    while(c != '\n')
    {
        int index;
        fseek(in, line[3], SEEK_SET);
        scanned = fscanf(in, "%d%c", &index, &c);
        line[3] = ftell(in);

        graph->connections[last_node][index] = 1;
        graph->connections[index][last_node] = 1;
        fprintf(out, "%d - %d\n", last_node, index);
    }
	

	int N=graph->node_count;//amount of nodes
	L_t L = malloc(sizeof(*L));
	init_L(N,L);
	for(int i=0;i<N;i++)
	{
		L->n_cc[i]=0;
		for(int j=0;j<N;j++)
		{
			L->n_cc[i]+=graph->connections[i][j];
		}
		L->m+=L->n_cc[i];
		L->group[i][i]=1;
		L->g_nc[i]=1;
		L->ng[i]=i;
	}

	Leiden(N,L,graph);

	int igc=0;//inter group connections
	int tigc=0;//total -||-
	for(int i=0;i<N;i++)
	{
		if(i==N-12)
		{

		}
		if(L->g_nc[i]>0)
		{

			igc=0;
			for(int j=0;j<N;j++)
			{
				if(L->ng[j]==i)
				{
					printf("%d,",j);
				for(int k=0;k<N;k++)
				{
					if(L->ng[j] != L->ng[k] && graph->connections[j][k]==1)
					{
						igc++;
					}
				}
				}
			}
			tigc+=igc;
			printf("\nnode count:%d connections to other groups: %d\n\n",L->g_nc[i],igc);
		}
	}
	printf("suma polaczen miedzy grupami to:%d\n",tigc/2);

    //print_connection_table(graph, out);
    free_L(N,L);
    free_graph(graph);
    fclose(in);
    return 0;
}




