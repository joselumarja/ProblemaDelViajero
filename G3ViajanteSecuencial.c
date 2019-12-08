#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"

//Declaraciones de constantes, tipos, variables y macros

#define true 1
#define false 0
#define StartNode 0
#define InitialQueueBlock 2

typedef struct{
    int* pobl;
    int contador;
    int coste;
}tour_struct;
typedef tour_struct *tour_t;

typedef struct{
    tour_t tour;
    struct queue_struct_element *next;
}queue_struct_element;

typedef struct{
    int n_elements;
    queue_struct_element *first_element;
    queue_struct_element *last_element;
}queue_struct;
typedef queue_struct *my_queue_t;

int N_CITIES;
int *digraph;

tour_t best;
tour_t tour;
my_queue_t queue;

//Definiciones de funciones usadas
tour_t pop(my_queue_t queue);
void push(my_queue_t queue, tour_t tour);

void printBest();
void bestTour(tour_t tour);

int estaEnElRecorrido(tour_t tour, int pob);
int containsTour(tour_t tourQueue, tour_t newTour);
int checkTourViability(tour_t tour, my_queue_t queue);
tour_t anadirPoblacion(tour_t tour, int pob);

void recorrerEnAnchura(my_queue_t queue);

void freeTour(tour_t tour);


int main(int argc, char *argv[]){
    FILE *diagraph_file;
    double inicio, fin;

    if ((diagraph_file = fopen(argv[1],"r"))==NULL){
        printf("Error abriendo el archivo.\n");
        exit(EXIT_FAILURE);
    }

    //Leer de diagraph_file el número de poblaciones, n; 
    fscanf(diagraph_file,"%d", &N_CITIES);
    printf("Numero de ciudades: %d\n\n", N_CITIES);

    //Reservar espacio para diagraph;
    digraph=(int*) malloc(sizeof(int)*N_CITIES*N_CITIES);

    //Leer de diagraph_file el grafo, diagraph;
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

    //Inicializar tour y besttour;
    tour=(tour_t) malloc(sizeof(tour_struct));
    best=(tour_t) malloc(sizeof(tour_struct));
    queue=(my_queue_t) malloc(sizeof(queue_struct));

    tour->pobl=(int *) malloc(sizeof(int));
    tour->pobl[0]=StartNode;
    tour->contador=1;
    tour->coste=0;

    best->coste=N_CITIES*100;
    best->contador=-1;
    best->pobl=(int *) malloc(sizeof(int));

    queue->n_elements=0;
    push(queue,tour);
    
    GET_TIME(inicio);
    recorrerEnAnchura(queue);
    GET_TIME(fin);

    //Imprimir resultados: besttour, coste y tiempo
    int tiempo=fin-inicio;  //calculamos tiempo 

    printf("Tiempo empleado en la ejecucion %d s\n",tiempo);

    //best tour
    printf("Best tour:\n");
    printBest();

    //Liberar memoria dinámica asignada
    free(queue);
    freeTour(best);
}

tour_t pop(my_queue_t queue)
{
    tour_t tour=NULL;
    queue_struct_element *element;

    if(queue->n_elements > 0)
    {
        element=queue->first_element;
        tour=element->tour;
        queue->first_element=element->next;

        queue->n_elements--;

        free(element);
    }
    return tour;
}

void push(my_queue_t queue, tour_t tour)
{
    //printf("%d\n",queue->n_elements);
    queue_struct_element *element;
    element=(queue_struct_element*) malloc(sizeof(queue_struct_element));
    element->tour=tour;
    element->next=NULL;

    if(queue->n_elements>0)
    {
        queue->last_element->next=element;
        queue->last_element=element;
    }
    else
    {
        queue->first_element=element;
        queue->last_element=element;
    }
    
    queue->n_elements++;
}

tour_t anadirPoblacion(tour_t tour, int pob)
{
    int poblation_offset=(tour->pobl[tour->contador-1]*N_CITIES)+pob;

    tour_t newTour=(tour_t) malloc(sizeof(tour_struct));
    newTour->pobl=(int *) malloc(sizeof(int)*(tour->contador+1));

    //Copia de los valores anteriores del tour
    memcpy(newTour->pobl,tour->pobl,sizeof(int)*(tour->contador));

    newTour->pobl[tour->contador]=pob;
    newTour->contador=tour->contador+1;
    newTour->coste=tour->coste+digraph[poblation_offset];

    return newTour;
}

void bestTour(tour_t tour){
    if(tour->coste<best->coste){
        freeTour(best);
        best=tour;
        printf("New best found:\n");
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

void recorrerEnAnchura(my_queue_t queue)
{
    tour_t tour;
    tour_t nuevo_tour;
    while(queue->n_elements>0)
    {
        tour=pop(queue);
        if(tour->contador==N_CITIES)
        {
            int pobOffset=(tour->pobl[tour->contador-1]*N_CITIES)+StartNode;

            if(digraph[pobOffset]>0)
            {
                tour_t checkTour=anadirPoblacion(tour,StartNode);
                freeTour(tour);
                tour=checkTour;
                bestTour(tour);
            }
        }
        else
        {
            int i=tour->pobl[tour->contador-1];
            for(int pobId=0;pobId<N_CITIES;pobId++)
            {
                if((digraph[(i*N_CITIES)+pobId]>0)&&(estaEnElRecorrido(tour,pobId)==false))
                {
                    nuevo_tour=anadirPoblacion(tour,pobId);

                    if(checkTourViability(nuevo_tour,queue)==true)
                        push(queue,nuevo_tour);
                    else
                        freeTour(nuevo_tour);
                    
                }
            }
            freeTour(tour);
        }
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

int checkTourViability(tour_t tour, my_queue_t queue)
{
    int viable=true;
    queue_struct_element *element;

    if(queue->n_elements>0)
    {
        element=queue->first_element;
        do
        {
            if(containsTour(element->tour,tour)==true)
            {
                if(element->tour->coste<tour->coste)
                {
                    viable=false;
                    break;
                }  
            }

            element=element->next;
        } while (element!=NULL);
        
    }

    return viable;
}

int containsTour(tour_t tourQueue, tour_t newTour)
{
    int included=true;

    for(int i=0;i<newTour->contador;i++)
    {
        int aux=false;
        for(int j=0; j<tourQueue->contador; j++)
        {
            if(newTour->pobl[i]==tourQueue->pobl[j])
            {
                aux=true;
                break;
            }
        }
        if(aux==false)
        {
            included=false;
            break;
        }
    }

    return included;
}

void freeTour(tour_t tour)
{
    free(tour->pobl);
    free(tour);
}
