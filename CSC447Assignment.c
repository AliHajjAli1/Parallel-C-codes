#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

void printArray(int* arr, int length);

int main(int argc, char** argv) {
    int rank, size;
    int* primes;
    int n, num;
    int* localPrimes;
    int* allPrimes;
    int localCount = 0;
    int finalCount = 0;
    double startSq,endSq,startPrll,endPrll,timeSq,timePrll;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank==0){
        printf("Enter n : \n");
        scanf("%d",&n);
        primes=(int*)malloc((n+1)*sizeof(int));
        localPrimes=(int*)malloc((n+1)*sizeof(int));
        allPrimes=(int*)malloc((n + 1)*sizeof(int));
        startSq=MPI_Wtime();
        for(int i=2;i<=n;i++)
            primes[i]=1;
        for(int i=2;i<=sqrt(n);i++) 
            if(primes[i]==1)
                for (int j = i + i; j <= n; j += i)
                    primes[j] = 0;
        endSq=MPI_Wtime();
        timeSq=endSq-startSq;
        printf("Primes in seq: ");
        //printArray(primes,n);
        printf("time = %f\n",timeSq);
        for (int i=3;i<=n;i++)
            MPI_Send(&i,1,MPI_INT,1,0,MPI_COMM_WORLD);
        num=-1;
        MPI_Send(&num,1,MPI_INT,1,0,MPI_COMM_WORLD);
    }
    startPrll=MPI_Wtime(); 
    if(rank!=0){
        localPrimes=(int*)malloc((n+1)*sizeof(int));
        while(1){
            MPI_Recv(&num,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if(num==-1) {
                if(rank!=size-1)
                    MPI_Send(&num,1,MPI_INT,rank+1,0,MPI_COMM_WORLD);
                break;
            }
            bool isPrime = true;
            for (int i=2;i<=sqrt(num);i++)
                if (num % i == 0) {
                    isPrime = false;
                    break;
                }
            if(isPrime){
                localPrimes[localCount++]=num;
                if(rank!=size-1)
                  MPI_Send(&num,1,MPI_INT,rank+1,0,MPI_COMM_WORLD);
            }
        }
        MPI_Send(&localCount,1,MPI_INT,0,0,MPI_COMM_WORLD);
        MPI_Send(localPrimes,localCount,MPI_INT,0,0,MPI_COMM_WORLD);
    }
    endPrll=MPI_Wtime();
    timePrll=endPrll-startPrll;
    if (rank==0) {
        allPrimes[finalCount++]=2;
        for (int i=1;i<size;i++) {
            int tmp;
            MPI_Recv(&tmp,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Recv(localPrimes,tmp,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for (int j=0;j<tmp;j++)
                allPrimes[finalCount++]=localPrimes[j];
        }
        printf("Prime parallel: ");
        /*for (int i=0;i<finalCount;i++)
            printf("%d ", allPrimes[i]);
        printf("\n");*/
        printf("time prll: %f\n",timePrll);
        free(primes);
        free(localPrimes);
        free(allPrimes);
    }
    MPI_Finalize();
    return 0;
}
void printArray(int* arr, int l) {
    for (int i = 2; i < l; i++)
        if (arr[i] > 0)
            printf("%d ", i);
    printf("\n");
}