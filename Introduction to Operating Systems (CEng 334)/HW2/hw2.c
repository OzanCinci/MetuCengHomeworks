#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include <semaphore.h>
#include "hw2_output.h"

void printf_matrix(int ** arr, int row, int column)
{
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<column;j++)
            printf("%d ",arr[i][j]);
        printf("\n");
    }
}

//pthread_mutex_t mutex;

typedef struct thread {
    pthread_t tid;
    int array_index;
} th;

typedef struct addition_args {
    int** matrix1;
    int** matrix2;
    int** result_matrix;
    int row_index;
    int row_size;
    int is_left_result;
    int N;
    //int array_index;
} add_args;

typedef struct multiplication_args {
    int** matrix1;
    int** matrix2;
    int** result_matrix;
    int row_index;
    int element_count; // #elements in a row
    int new_row_size;
} mul_args;

typedef struct {
    sem_t mutex;
    sem_t wait_sem;
    int completed_count;
    int required_count;
} monitor_t;

void monitor_init(monitor_t *cv, int required_count) {
    sem_init(&cv->mutex, 0, 1);
    sem_init(&cv->wait_sem, 0, 0);
    cv->completed_count = 0;
    cv->required_count = required_count;
}

void monitor_wait(monitor_t *cv) {
    if (cv->completed_count < cv->required_count) {
        sem_wait(&cv->wait_sem);
    } else {
		sem_post(&cv->wait_sem);
	}
}

void monitor_signal(monitor_t *cv) {
	sem_wait(&cv->mutex);
    cv->completed_count++;
	if (cv->completed_count >= cv->required_count)
		sem_post(&cv->wait_sem);
    sem_post(&cv->mutex);
}

// condition variables for columns
monitor_t* condition_columns;
// semaphore for each rows in left matrix
sem_t* semaphore_rows;


void* multiplication_thread_func(void* arg) {
    // type adjustment
    mul_args* args = (mul_args*) arg; 
    //printf("multiplication geldi-> row_index:(%d) \n",args->row_index);
    // wait for semaphore to complete
    int row_index = args->row_index;
    sem_wait(&semaphore_rows[row_index]);
	//printf("row waiti gecti %d\n", row_index);
    // semaphore is useless now -> destroy it
    sem_destroy(&semaphore_rows[row_index]);

    int element_count = args->element_count;
    int new_row_size = args->new_row_size;
    int accumulator;

    int **matrix1=args->matrix1;
    int **matrix2=args->matrix2;
    int **result_matrix = args->result_matrix;
    for(int i=0;i<new_row_size;i++)
    {
        // i = decides which column
        accumulator = 0;
        //printf("column bekliyor:(%d) \n",i);
        monitor_wait(&condition_columns[i]);
        //printf("column gecti: (%d) \n",i);
        monitor_signal(&condition_columns[i]);
        for(int j=0;j<element_count;j++)
        {
            //printf("(%d)x(%d)\n", matrix1[row_index][j], matrix2[j][i]);
            accumulator += matrix1[row_index][j] * matrix2[j][i];
        }
        hw2_write_output(2,row_index+1,i+1,accumulator);
        result_matrix[row_index][i]=accumulator;
    }

    // terminate the current thread
    pthread_exit(NULL);
}


void* addition_thread_func(void* arg) {
    // type adjustment
    add_args* args = (add_args*) arg; 

    int row_index = args->row_index;
    int row_size = args->row_size;
    int output_index = (args->is_left_result==1)?0:1;
    int calculated_value;
    int **matrix1=args->matrix1;
    int **matrix2=args->matrix2;
    int **result_matrix = args->result_matrix;
    // calculate addition and insert the result into temp result matrix
    for(int i=0;i<row_size;i++)
    {
        calculated_value = matrix1[row_index][i] + matrix2[row_index][i];
        result_matrix[row_index][i] = calculated_value;
        hw2_write_output(output_index,row_index+1,i+1,calculated_value);
        // if it is right matrix, then it could potentially
        // complete a column
        if (args->is_left_result==0)
        {
            monitor_signal(&condition_columns[i]);
        }
    }

    // update the semaphore
    if(args->is_left_result==1)
    {
        sem_post(&semaphore_rows[row_index]);
    }

    // terminate the current thread
    pthread_exit(NULL);
}


int main()
{
    // init output
    hw2_init_output();

    // init required variables
    int N,M,K, temp;
    N=0;
    M=0;
    K=0;
    // init threads array
    th* first_addition_threads;
    th* second_addition_threads;
    th* multiplication_threads;

    // read matrix1
    scanf("%d %d",&N,&M);
    int** matrix1 = (int**) malloc(sizeof(int*)*N);
    for(int i=0;i<N;i++)
    {
        matrix1[i] = (int*) malloc(sizeof(int)*M);
        for(int j=0;j<M;j++)
        {
            scanf("%d", &temp);
            matrix1[i][j] = temp;
        }
    }
    // read matrix2
    scanf("%d %d",&N,&M);
    int** matrix2 = (int**) malloc(sizeof(int*)*N);
    for(int i=0;i<N;i++)
    {
        matrix2[i] = (int*) malloc(sizeof(int)*M);
        for(int j=0;j<M;j++)
        {
            scanf("%d", &temp);
            matrix2[i][j] = temp;
        }
    }
    // read matrix3
    scanf("%d %d",&M,&K);
    int** matrix3 = (int**) malloc(sizeof(int*)*M);
    for(int i=0;i<M;i++)
    {
        matrix3[i] = (int*) malloc(sizeof(int)*K);
        for(int j=0;j<K;j++)
        {
            scanf("%d", &temp);
            matrix3[i][j] = temp;
        }
    }
    // read matrix4
    scanf("%d %d",&M,&K);
    int** matrix4 = (int**) malloc(sizeof(int*)*M);
    for(int i=0;i<M;i++)
    {
        matrix4[i] = (int*) malloc(sizeof(int)*K);
        for(int j=0;j<K;j++)
        {
            scanf("%d", &temp);
            matrix4[i][j] = temp;
        }
    }

    // init semaphore_rows
    // it represents whether a row in left matrix is calculated or not
    semaphore_rows = (sem_t*) malloc(sizeof(sem_t)*N);
    for(int i=0;i<N;i++)
        sem_init(&semaphore_rows[i], 0, 0);

    // init condition variables
    // it represents whether a column in right matrix is calculated or not
    condition_columns = (monitor_t*) malloc(sizeof(monitor_t)*K);
    for(int i=0;i<K;i++)
        monitor_init(&condition_columns[i],M);


    //
    //  create result matrices for temporary storage
    //

    // create result of addition1
    int** result_addition_1 = (int**) malloc(sizeof(int*)*N);
    for(int i=0;i<N;i++)
        result_addition_1[i] = (int*) malloc(sizeof(int)*M);
    // create result of addition2is_right_matrix
    int** result_addition_2 = (int**) malloc(sizeof(int*)*M);
    for(int i=0;i<M;i++)
        result_addition_2[i] = (int*) malloc(sizeof(int)*K);
    // create result of multiplication
    int** result_multiplication = (int**) malloc(sizeof(int*)*N);
    for(int i=0;i<N;i++)
        result_multiplication[i] = (int*) malloc(sizeof(int)*K);


    // create (N+M+N) threads
    // the first N calculates the addition of the first two matrices
    // the M calculates the addition of the last two matrices
    // the second N calculates multiplication so waits for row&column status

    // create threads for first addition
    first_addition_threads = (th*) malloc(sizeof(th)*N);
    add_args arguments1[N];
    for(int i=0;i<N;i++)
    {
        // create arguments for thread function
        arguments1[i].matrix1 = matrix1;
        arguments1[i].matrix2 = matrix2;
        arguments1[i].result_matrix = result_addition_1;
        arguments1[i].row_index = i;
        arguments1[i].row_size = M;
        arguments1[i].is_left_result = 1;
        arguments1[i].N = N;

        // create thread
        first_addition_threads[i].array_index = i;
        pthread_create(&first_addition_threads[i].tid, NULL, addition_thread_func, &arguments1[i]);
    }

    // create threads for second addition
    second_addition_threads = (th*) malloc(sizeof(th)*M);
    add_args arguments2[M];
    for(int i=0;i<M;i++)
    {
        // create arguments for thread function
        arguments2[i].matrix1 = matrix3;
        arguments2[i].matrix2 = matrix4;
        arguments2[i].result_matrix = result_addition_2;
        arguments2[i].row_index = i;
        arguments2[i].row_size = K;
        arguments2[i].is_left_result = 0;
        arguments2[i].N = N;

        // create thread
        second_addition_threads[i].array_index = i;
        pthread_create(&second_addition_threads[i].tid, NULL, addition_thread_func, &arguments2[i]);
    }

     // create threads for multiplication
    multiplication_threads = (th*) malloc(sizeof(th)*N);
    mul_args arguments3[N];
    for(int i=0;i<N;i++)
    {
        // create arguments for thread function
        arguments3[i].matrix1 = result_addition_1;
        arguments3[i].matrix2 = result_addition_2;
        arguments3[i].result_matrix = result_multiplication;
        arguments3[i].row_index = i;
        arguments3[i].element_count = M;
        arguments3[i].new_row_size = K;

        // create thread
        multiplication_threads[i].array_index = i;
        pthread_create(&multiplication_threads[i].tid, NULL, multiplication_thread_func, &arguments3[i]);
    }

    // wait addition threads to complete
    for(int i=0;i<N;i++)
        pthread_join(first_addition_threads[i].tid, NULL);
    for(int i=0;i<M;i++)
        pthread_join(second_addition_threads[i].tid, NULL);
    // wait for multiplication threads to finish
    for(int i=0;i<N;i++)
        pthread_join(multiplication_threads[i].tid, NULL);


    printf_matrix(result_multiplication,N,K);
    return 0;
}
