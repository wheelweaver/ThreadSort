#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>       /* log2 */


/* Global mutex */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Declaration of thread condition variable 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
 



 
int slice;
int n;  // size of array
int m; // # of threads

// Barrier: 
//int remain = n;
//pthread_cond_t cv;


// array for random numbers
int *array; 

struct tsk {
	int id;
	int low;
	int high;
};

// merge two parts
// merge function for merging two parts
void merge(int low, int mid, int high)
{   
    int* left = (int*) malloc( (mid - low + 1) * sizeof(int));
    int* right = (int*) malloc( (high - mid) * sizeof(int));


    // n1 is size of left part and n2 is size
    // of right part
    int n1 = mid - low + 1,
    n2 = high - mid,
    i, j;

    // storing values in left part
    for (i = 0; i < n1; i++)
        left[i] = array[i + low];

    // storing values in right part
    for (i = 0; i < n2; i++)
        right[i] = array[i + mid + 1];

    int k = low;
    i = j = 0;

    // merge left and right in ascending order
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            array[k++] = left[i++];
        else
            array[k++] = right[j++];
    }

    // insert remaining values from left
    while (i < n1) {
        array[k++] = left[i++];
    }

    // insert remaining values from right
    while (j < n2) {
        array[k++] = right[j++];
    }

    free(left);
    free(right);
}



// merge sort
void merge_sort(int low, int high){
	int mid = low + (high - low) / 2;
	
	if( low < high){
		// calling first half
		merge_sort(low, mid);
		
		// calling middle part
		merge_sort(mid+1, high);
		
		// merge the two halfs
		merge(low,mid,high);
	}
}

void localSync(int n){
	int bob = n +1;
	merge(0, (n / 2 - 1) / 2, n / 2 - 1);
	merge(n / 2, n / 2 + (n - 1 - n / 2) / 2, n - 1);
	merge(0, (n - 1) / 2, n - 1);
}

void *start(void *arg){
	struct tsk *tsk = arg;
	int low;
	int high;
	int levels = log2 (m); // gets the # of levels 
	// get low and high of each thread
	low = tsk->low;
	high=tsk->high;
	//printf("%d", low);
	
	// mid-point
	int mid = low + (high - low) / 2;
	if (low < high){
		merge_sort(low,mid);
		merge_sort(mid+1,high);
		merge(low, mid, high);
		
	}
	int i;
	
	/* Local Synchronization
	for(i = 0; i < levels; i++){
		
		if(id % pow(i+1,2)  ==  0){
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond, &mutex); 
			pthread_mutex_unlock(&mutex);		
		}else{
			pthread_cond_singal(&cond1)
		}
	}*/
	
	
	
	/*Barrier Synchronization
	pthread_mutex_lock(&mutex);
	remain--; 
	if (remain == 0) { pthread_cond_broadcast(&cond; }
	else {
		while (remain != 0) { pthread_cond_wait(&cond, &m); }
	}
	pthread_mutex_unlock(&mmutex;
	
	
	*/
}
	


int main(int argc, char** argv) 
{ 
	
	n = atoll(argv[1]);
	m = atoll(argv[2]);

	 FILE *fptr;
	 fptr = fopen(argv[3],"w");
	if(fptr == NULL)
	{
	  printf("Error!");   
	  exit(1);             
	}
	
 
	
	slice = n / m;
	printf("The size of array is: %d\n",n);
	printf("The # of threads is: %d\n",m);

	
	
	array = malloc(sizeof(int) * n);
	struct tsk *tsk;
	srand(time(NULL));
	
	int i;
	/* Unsorted Array */
	printf("Unsorted Array\n");
	for(i = 0; i < n; i++){
		array[i] = rand() % 100;
		printf("%d ",array[i]);
	}
	printf("\n");
	
	 pthread_t tid[m];  /* the thread identifier */
	 struct tsk tsklist[m];
	
	for(i=0; i<n;i++){
		tsk = &tsklist[i];
		tsk->id = i;
		tsk->low = i * slice;
		tsk->high = (i+1) * slice - 1;		
	}
	
	/*Create the threads*/
    for(int i=0;i<m;i++){
		tsk = &tsklist[i];		
        pthread_create(&tid[i],NULL,start,tsk);
    }
	
    /* wait for the thread to exit */
    for (int i=0; i<m; i++){
         pthread_join(tid[i], NULL);
    }
	
	
	/* 1st Round Sorted Array */
	printf("1st Round Sorted Array \n");
	for(i = 0; i < n; i++){
		printf("%d ",array[i]);
	}
	printf("\n");
	
	
	// Local Synchronization with mutext after all threads have sorted their section of the data
	//localSync(n);
	
	/*
	merge(0, (n / 2 - 1) / 2, n / 2 - 1);
	merge(n / 2, n / 2 + (n - 1 - n / 2) / 2, n - 1);
	merge(0, (n - 1) / 2, n - 1);
	*/
	/* Final Sorted Array */
	printf("Final Sorted Array \n");
	int temp;
	for(i = 0; i < n; i++){
		temp  = array[i];
		printf("%d ",array[i]);
		fprintf(fptr,"%d\n",temp);
	}
	printf("\n");

   fclose(fptr);


	return 0; 
} 





/*






1	  2     3    4   5        6      7          8
12 34    56 78 9,10  11,12  13,14 15,16
















*/
