#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>

// Obliczanie 
double leibnizPi(float n) {
    double pi=1.0;
    int i;
    int N;
    for (i=3, N=2*n+1; i<=N; i+=2)
        pi += ((i&2) ? -1.0 : 1.0) / i;
    return 4*pi;
    
}
    // Funkcja
    double wzorf(double x)
    {
    return pow(x, 2);
    }
    // Potrzebne do obliczeń
    double wysokosc(double a, double b, double n){
        return (b - a) / n;
    }

int main(int argc,char **argv){

    // Rozpisuje wszystkie zmienne
    int rank;
    int num_proc;
    double leib;
    int tag = 1999;
    MPI_Status status;
    // Nasze przediały do liczenia
    double a = 5;
    double b = 10;


    // Inicjuje MPI + pobieram ranking i liczbe procesów
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    if(rank == 0)
    {
        // W procesie 0 obliczam leibniza i podaje do kolejnego procesu +1
       leib = leibnizPi(num_proc);
        printf(" %d rank leibniz = %f \n",rank, leib);
        MPI_Send(&leib,1,MPI_DOUBLE,rank+1,tag,MPI_COMM_WORLD);
    }else if(rank+1 == num_proc) { // Ostatni proces
        MPI_Recv(&leib,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD,&status);
        printf(" %d rank leibniz = %f \n",rank+1, leib);
    }else{// Reszta procesów, przekazuje z poprzednich i je wyświetlam
        MPI_Recv(&leib,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD,&status);
        printf(" %d rank leibniz = %f \n",rank, leib);
        MPI_Send(&leib,1,MPI_DOUBLE,rank+1,tag,MPI_COMM_WORLD);
    }


    // Metoda prostokątów

     double suma = 0;
     double dx = ( b - a) / num_proc;
        if(rank == num_proc - 1 ){  // Od ostatniego procesu, obliczam sumę i wysyłam do rank-1
            
        suma += wzorf(a + rank * dx );
        
        
        MPI_Send(&suma,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD);

        }
        else if(rank != 0 && rank < num_proc - 1)  { // Proces który nie jest ostatni ani pierwszy przechwytuje sume i ją dodaje
            MPI_Recv(&suma,1,MPI_DOUBLE,rank+1,tag,MPI_COMM_WORLD,&status);
            suma += wzorf(a + rank * dx );
            
            
            MPI_Send(&suma,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD);
        }else{// Ostatni proces przechwytuje sumę i pomnaża ją przez dx + wypisuje
            MPI_Recv(&suma,1,MPI_DOUBLE,rank+1,tag,MPI_COMM_WORLD,&status);
            suma *= dx;
            printf("suma prostokatow = %f\n", suma);
 
                   
        }

    //Metoda Trapezów
   
        
        double xi = 0;

        if(rank == num_proc - 1 ){ // Od ostatniego procesu obliczam i wysyłam xi do kolejnych

            xi += (wzorf(a + (((double) rank / (double) num_proc) * (b - a))));
        MPI_Send(&xi,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD);

        }
        else if(rank != 0 && rank < num_proc - 1)  { // Jeżeli nie jest ostatni ani pierwszy przechwytuje xi i dodaje i wysyła dalej
            MPI_Recv(&xi,1,MPI_DOUBLE,rank+1,tag,MPI_COMM_WORLD,&status);
            xi += (wzorf(a + (((double) rank / (double) num_proc) * (b - a))));           
            MPI_Send(&xi,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD);
        }else{ // W ostatnim procesie 0 pobieram xi z 1 i obliczam sumę
            MPI_Recv(&xi,1,MPI_DOUBLE,rank+1,tag,MPI_COMM_WORLD,&status);
            suma = wysokosc(a,b,num_proc) * (wzorf(a) / 2 + xi + wzorf(b)/2);
            printf("rank  %d , suma trapezów =  %f \n", rank, suma);
        }

    MPI_Finalize();
    return 0;
}