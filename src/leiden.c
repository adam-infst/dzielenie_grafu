#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "leiden.h"


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