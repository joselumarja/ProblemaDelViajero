#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"

/*Declaraciones de constantes, tipos, variables y macros*/

#define true 1
#define false 0
#define StartNode 0

typedef struct{
    int* pobl;
    int contador;
    int coste;
}tour_struct;
typedef tour_struct *tour_t;

typedef struct{
    tour_t *list;
    int list_sz;
}stack_struct;
typedef stack_struct *my_stack_t;

int N_CITIES;
int StackSize;  
int *digraph;

tour_t best;
tour_t tour;
my_stack_t pila;

/*Definiciones de funciones usadas*/
tour_t pop(my_stack_t pila);
void push(my_stack_t pila, tour_t tour);

void printBest();
void best_tour(tour_t tour);

int factible(tour_t tour);
int estaEnElRecorrido(tour_t tour, int pob);
tour_t anadir_pob(tour_t tour, int pob);

void Rec_en_profund(my_stack_t pila);

void freeTour(tour_t tour);


int main(int argc, char *argv[]){
    FILE *diagraph_file;
    double inicio, fin;

    if ((diagraph_file = fopen(argv[1],"r"))==NULL){
        printf("Error abriendo el archivo.\n");
        exit(EXIT_FAILURE);
    }

    /*Leer de diagraph_file el número de poblaciones, n;*/
    fscanf(diagraph_file,"%d", &N_CITIES);
    printf("Numero de ciudades: %d\n\n", N_CITIES);

    StackSize=(N_CITIES*((N_CITIES-3)/2))+2;

    /*Reservar espacio para diagraph;*/
    digraph=(int*) malloc(sizeof(int)*N_CITIES*N_CITIES);

    /*Leer de diagraph_file el grafo, diagraph;*/
    for(int i=0; i < N_CITIES; i++)
    {
        for(int j=0; j < N_CITIES; j++)
        {
            int NeighbourCost;
            fscanf(diagraph_file, "%d", &NeighbourCost);
            digraph[(i*N_CITIES)+j]=NeighbourCost;
            printf("%d ",NeighbourCost);
        }
        printf("\n");
    }

    /*Inicializar tour y besttour;*/
    tour=(tour_t) malloc(sizeof(tour_struct));
    best=(tour_t) malloc(sizeof(tour_struct));
    pila=(my_stack_t) malloc(sizeof(stack_struct));
    pila->list=(tour_t*) malloc(sizeof(tour_t)*StackSize);

    tour->pobl=(int *) malloc(sizeof(int));
    tour->pobl[0]=StartNode;
    tour->contador=1;
    tour->coste=0;

    best->coste=N_CITIES*100;
    best->contador=-1;
    best->pobl=(int *) malloc(sizeof(int));

    pila->list[0]=tour;
    pila->list_sz=1;

    GET_TIME(inicio);
    Rec_en_profund(pila);
    GET_TIME(fin);

    /*Imprimir resultados: besttour, coste y tiempo*/
    int tiempo=fin-inicio;  /*calculamos tiempo*/

    printf("Tiempo empleado en la ejecucion %d s\n",tiempo);

    /*best tour*/
    printf("Best tour:\n");
    printBest();

    /*Liberar memoria dinámica asignada*/
    free(pila->list);
    free(pila);
    freeTour(best);
}

tour_t pop(my_stack_t pila)
{
    tour_t tour=NULL;

    if(pila->list_sz>0)
    {
        pila->list_sz--;
        tour=pila->list[pila->list_sz];
        pila->list[pila->list_sz]=NULL;
    }
    
    return tour;
}

void push(my_stack_t pila, tour_t tour)
{
    pila->list[pila->list_sz]=tour;
    pila->list_sz++;
}

tour_t anadir_pob(tour_t tour, int pob)
{
    int poblation_offset=(tour->pobl[tour->contador-1]*N_CITIES)+pob;

    tour_t newTour=(tour_t) malloc(sizeof(tour_struct));
    newTour->pobl=(int *) malloc(sizeof(int)*(tour->contador+1));

    /*Copia de los valores anteriores del tour*/
    memcpy(newTour->pobl,tour->pobl,sizeof(int)*(tour->contador));

    newTour->pobl[tour->contador]=pob;
    newTour->contador=tour->contador+1;
    newTour->coste=tour->coste+digraph[poblation_offset];

    return newTour;
}

void best_tour(tour_t tour){
    if(tour->coste<best->coste){
        freeTour(best);
        best=tour;
        printf("New best:\n");
        printBest();
    }
    else
    {
        freeTour(tour);
    }
}

void printBest()
{
    for(int i=0; i<best->contador-1;i++)
    {
        printf("%d -> ",best->pobl[i]);
    }
    printf("%d\n",best->pobl[best->contador-1]);
    printf("Coste: %d\n\n",best->coste);
}
void Rec_en_profund(my_stack_t pila)
{
    tour_t tour;
    tour_t nuevo_tour;
    while(pila->list_sz>0)
    {
        tour=pop(pila);
        if(tour->contador==N_CITIES)
        {
            int pobOffset=(tour->pobl[tour->contador-1]*N_CITIES)+StartNode;

            if(digraph[pobOffset]>0)
            {
                tour_t checkTour=anadir_pob(tour,StartNode);
                best_tour(checkTour);
            }
        }
        else
        {
            int i=tour->pobl[tour->contador-1];
            for(int pobId=0;pobId<N_CITIES;pobId++)
            {
                if((digraph[(i*N_CITIES)+pobId]>0)&&(estaEnElRecorrido(tour,pobId)==false))
                {
                    nuevo_tour=anadir_pob(tour,pobId);
                    if(factible(nuevo_tour)==true)
                    {
                        push(pila,nuevo_tour);
                    }
                    else
                    {
                        freeTour(nuevo_tour);
                    }
                }
            }
        }
        freeTour(tour);
    }
}

int estaEnElRecorrido(tour_t tour, int pob)
{
    for(int i=0; i<tour->contador; i++)
    {
        if(tour->pobl[i]==pob) return true;
    }

    return false;
}

void freeTour(tour_t tour)
{
    free(tour->pobl);
    free(tour);
}

int factible(tour_t tour)
{
    if(tour->coste>best->coste)
    {
        return false;
    }

    return true;
}
