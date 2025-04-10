#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "row_list.h"

//printf(" [1] "); scanf("%d", &argc);;;


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

void init_L(int N,L_t L)
{
	L->group = malloc(N*sizeof(int*));
	for(int i=0; i<N;i++)
	{
		L->group[i] = malloc (N*sizeof(int));
	}
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<0;j++)
		{
			L->group[i][j]=0;
		}
	}
	L->cg=0;
	L->mg=0;
	L->g_nc = malloc(N*sizeof(int));
	L->ng = malloc(N*sizeof(int));
	L->n_cc = malloc(N*sizeof(int));
	for(int i=0;i<N;i++)
	{
		L->g_nc[i]=0;
		L->ng[i]=0;
		L->n_cc[i]=0;
	}
}

float Q_count(int N, L_t L, graph_t graph)
{
	float Q=0;
	for(int i=0; i<N; i++)
	{
		for(int j=0; j<N; j++)
		{

			Q+=(graph->connections[i][j]-1.1*(L->n_cc[i]*L->n_cc[j]/(2*L->m)))*(L->ng[i] == L->ng[j]);
		}
	}
	return Q; 
}

void move_group(int N, L_t L)
{
	for(int i=0;i<N;i++)
	{
		if(L->ng[i]==L->cg)
		{
			L->group[L->mg][i]+=L->group[L->cg][i];
			L->group[L->cg][i]--;
			L->ng[i]=L->mg;
		}
	}
	L->g_nc[L->mg]+=L->g_nc[L->cg];
	L->g_nc[L->cg]=0;
	L->cg=L->mg;
}

void set_L_value(int N,L_t L1, L_t L2)
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			L2->group[i][j]=L1->group[i][j];
		}
		L2->g_nc[i]=L1->g_nc[i];
		L2->ng[i]=L1->ng[i];
		L2->n_cc[i]=L1->n_cc[i];
	}
	L2->m=L1->m;
	L2->Q=L1->Q;
	L2->cg=L1->cg;

}

void free_L(int N, L_t L)
{
	for(int i=0;i<N;i++)
	{
		free(L->group[i]);
	}
	free(L->group);
	free(L->g_nc);
	free(L->ng);
	free(L->n_cc);
	free(L);
}

void Leiden(int N, L_t L1, graph_t graph)
{
	float Q_max;
	int n_max;
	L1->Q=Q_count(N,L1,graph);
	L_t L2 = malloc(sizeof(*L2));
	init_L(N,L2);
	set_L_value(N,L1,L2);
	Q_max=L1->Q;
	for(int i=0;i<N;i++)
	{
		if(L1->g_nc[i]!=0)
		{
			Q_max=L1->Q;
			L1->cg=i;
			for(int j=0; j<N; j++)
			{
				if(L1->g_nc[j]!=0)
				{
					if((L1->g_nc[i]+L1->g_nc[j])<((N/5)+2) && (N/4!=N/5))
					{
						set_L_value(N,L1,L2);
						L2->mg=j;
						move_group(N,L2);
						L2->Q=Q_count(N,L2,graph);
						if(L2->Q>Q_max)
						{
							Q_max=L2->Q;
							n_max=j;
						}
					}else if((L1->g_nc[i]+L1->g_nc[j])<=((N/4)-1) && (N/4==N/5))
					{
						set_L_value(N,L1,L2);
						L2->mg=j;
						move_group(N,L2);
						L2->Q=Q_count(N,L2,graph);
						if(L2->Q>Q_max)
						{
							Q_max=L2->Q;
							n_max=j;
						}

					}

				}
			}
			if(Q_max>L1->Q)
			{
				L1->mg=n_max;
				move_group(N,L1);
			}
		}
	}
free_L(N,L2);

}


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
	//printf("node_count %d \n", graph->node_count);
    init_connections(graph); //zwraca już wyzerowaną macierz

    print_matrix(graph, out);

    fseek(in, line[4], SEEK_SET); // tu będzie jeszcze zrobiony refactoring
    fscanf(in, "%d%c", &last, &c);
    line[4] = ftell(in);

    if (last != 0 || c != ';') {
        printf("Zły format danych: last= %d, c= %c \n", last, c);
        return 2;
    }
    while(c != '\n' && c != EOF ) //uwaga linia 5 kończy się wcześniej niż linia 4
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
            graph->connections[index][current_node] = 1;
			fprintf(out, "%d - %d\n", current_node, index);
        }
        current_node++;
        last = next;
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
			printf("\nnode count:%d connections to other groups: %d\n",L->g_nc[i],igc);
		}
	}
	printf("suma polaczen miedzy grupami to:%d\n",tigc/2);

    //print_connection_table(graph, out);
    free_L(N,L);
    free_graph(graph);
    fclose(in);
    return 0;
}




