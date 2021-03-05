#define _POSIX_C_SOURCE 199309L 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
typedef struct arg{  
	int st;
   int end;
   int * arr;
}arg;




void selectionsort(int * arr, int s ,int e)
{   int  i , j , index;

	for(int i=s;i<e;i++)
      {  index = i; 
		 for(int j = s+1;j<=e;j++)
		 	if(arr[j] < arr[index])
		 	 index = j;
         int temp = arr[i];
         arr[i] = arr[index];
         arr[index] =temp;  
     }

return;

}

void *selectionsort_usingthread(void * a)
{    int s = ((arg*)a)->st;
	 int e = ((arg*)a)->end;
	 int *arr = ((arg*)a)->arr;
      int index;
	for(int i=s;i<e;i++)
      {  index = i; 
		 for(int j = s+1;j<=e;j++)
		 	if(arr[j] < arr[index])
		 	 index = j;
         int temp = arr[i];
         arr[i] = arr[index];
         arr[index] =temp;  
     }

}


int * memsharing(size_t size)
{       int shm_id = shmget(IPC_PRIVATE,size,IPC_CREAT | 0666);
	     return (int *)shmat(shm_id,NULL,0);
}
void merge(int *arr,int l,int m, int r)
{
    int a=0,b=0,c=l;
    int r1=m-l+1;
    int r2=r-m;
    int L[r1],R[r2];
    for(a=0;a<r1;a++)
        L[a]=arr[l+a];
    for(b=0;b<r2;b++)
        R[b]=arr[m+1+b];
    a=0;b=0;c=l;
    while((a<r1) &&(b<r2))
    {   if(L[a]<=R[b])
       { arr[c]=L[a];
         a++;}
       else
       { arr[c]=R[b];
          b++;}
          c++;
    }
    while(a<r1)
    {arr[c]=L[a];
     a++;
     c++;}
    while(b<r2)
    {arr[c]=R[b];
        b++;
        c++;}

}

void mergeSort_normal(int *arr,int l, int r)
{  if(l<r)
	{ int m = l +(r-l)/2;
	        mergeSort_normal(arr,l,m);
          mergeSort_normal(arr,m+1,r);
      merge(arr,l,m,r);
    }
}



void mergeSort_usingprocesses(int * arr, int l, int r)
{
    if (l < r)
    {   int pid1 = fork();
        int pid2;
        int m = l+(r-l)/2;
        
       if(pid1 == 0)
        {  if(m-l+1 < 5)
        	{selectionsort(arr,l,m);
        	 _exit(1);}
            else
        	{mergeSort_usingprocesses(arr, l, m);
            _exit(1); }
        
        }
        else
        {  pid2 =fork(); 
          
           if(pid2==0)
           { if(r-(m+1)+1<5)
           	 { selectionsort(arr,m+1,r);
           	   _exit(1); }
            else
           	{ mergeSort_usingprocesses(arr, m+1, r);
             _exit(1); }
           }
         
        }
        int stat;
        waitpid(pid1,&stat,0);
        waitpid(pid2,&stat,0);
        merge(arr, l, m, r);

        return;

    }
}


void printArray(int A[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}



void *mergeSort_usingthread(void * a)
{    int start = ((arg*)a)->st;
	 int end = ((arg*)a)->end;
	 int *arr = ((arg*)a)->arr;

	 if(start < end)
	 {  int m = start + (end-start)/2;
        arg a1;
        a1.st =start;
        a1.end = m;
        a1.arr = arr;
        pthread_t thrd1;
 
        pthread_create(&thrd1,NULL,mergeSort_usingthread,&a1);

        arg a2;
        a2.st =m+1;
        a2.end = end;
        a2.arr = arr;
        pthread_t thrd2;

        pthread_create(&thrd1,NULL,mergeSort_usingthread,&a2);

 	
        pthread_join(thrd1,NULL);
        pthread_join(thrd2,NULL);
        merge(arr,start,m,end);
        //merging
	 }



}



int main()
{   int n;
	struct timespec t;

    printf("Input array size:");
    scanf("%d",&n);
    
    int *arr = memsharing(sizeof(int)*n);
    int *brr = malloc(sizeof(int)*n);
    int *crr =malloc(sizeof(int)*n);

    for(int i=0;i<n;i++)
    { scanf("%d",&arr[i]);
         brr[i] = arr[i];
         crr[i] = arr[i];
    }

    printf("\n");
    printf("Running concurrent merge sort using processes:\n");
    clock_gettime(CLOCK_MONOTONIC_RAW,&t);
    long double start = t.tv_nsec/(1e9) + t.tv_sec; 
    mergeSort_usingprocesses(arr, 0,n-1);
    clock_gettime(CLOCK_MONOTONIC_RAW,&t);
    long double end = t.tv_nsec/(1e9) + t.tv_sec;     
    printf("Sorted array is \n");
    printArray(arr,n);
    long double tim1 = end-start;
    printf("time = %Lf\n",tim1);


    printf("\n"); 
    pthread_t thrd;
     arg a;
     a.st = 0;
     a.end = n-1;
     a.arr = brr;
    printf("Running concurrent merge sort using threads\n");
    clock_gettime(CLOCK_MONOTONIC_RAW,&t);
    start = t.tv_nsec/(1e9)+t.tv_sec;
    pthread_create(&thrd, NULL,mergeSort_usingthread, &a);
    pthread_join(thrd, NULL);
    clock_gettime(CLOCK_MONOTONIC_RAW,&t);
    end=t.tv_nsec/(1e9)+t.tv_sec;
    printf("Sorted array is \n");
    printArray(a.arr,n);
    long double tim2= end-start;
    printf("time = %Lf\n",tim2);



    printf("\n");
    printf("Running normal merge sort\n");
    clock_gettime(CLOCK_MONOTONIC_RAW,&t);
    start = t.tv_nsec/(1e9)+t.tv_sec;
    mergeSort_normal(crr,0,n-1);
    clock_gettime(CLOCK_MONOTONIC_RAW,&t);
    end=t.tv_nsec/(1e9)+t.tv_sec;
    printf("Sorted array is \n");
    printArray(crr,n);
    long double tim3= end-start;
    printf("time = %Lf\n",tim3);


   printf("normal_mergesort ran:\n\t[ %Lf ] times faster than process_concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n\n", tim1/tim3, tim2/tim3);
   shmdt(arr);

    
   
  


    return 0;
}
