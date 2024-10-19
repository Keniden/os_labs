#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <float.h>

// gcc -o main main.c -lpthread
// ./main 100 100 200 2

typedef struct
{
    int rows;
    int cols;
    float **data;
} matrix_t;

typedef struct
{
    matrix_t *input;
    matrix_t *output;
    int start_row;
    int end_row;
} thread_args_t;

sem_t semaphore;

matrix_t *initialize_matrix(int rows, int cols)
{
    matrix_t *mat = malloc(sizeof(matrix_t));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++)
    {
        mat->data[i] = malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++)
        {
            mat->data[i][j] = ((float)rand()) / RAND_MAX * 100.0f;
        }
    }
    return mat;
}

void free_matrix(matrix_t *mat)
{
    for (int i = 0; i < mat->rows; i++)
    {
        free(mat->data[i]);
    }
    free(mat->data);
    free(mat);
}

void print_matrix(matrix_t *mat)
{
    for (int i = 0; i < mat->rows; i++)
    {
        for (int j = 0; j < mat->cols; j++)
        {
            printf("%.2f ", mat->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

float erosion(matrix_t *input, int i, int j)
{
    float min_val = FLT_MAX;
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            int newi = i + x;
            int newj = j + y;
            if (newi >= 0 && newi < input->rows && newj >= 0 && newj < input->cols)
            {
                if (input->data[newi][newj] < min_val)
                    min_val = input->data[newi][newj];
            }
        }
    }
    return min_val;
}

float dilation(matrix_t *input, int i, int j)
{
    float max_val = -FLT_MAX;
    for (int di = -1; di <= 1; di++)
    {
        for (int dj = -1; dj <= 1; dj++)
        {
            // printf("before-");
            // fflush(stdout);
            int ni = i + di;
            int nj = j + dj;
            if (ni >= 0 && ni < input->rows && nj >= 0 && nj < input->cols)
            {
                if (input->data[ni][nj] > max_val)
                    max_val = input->data[ni][nj];
            }
            // printf("after");
            // fflush(stdout);
        }
    }
    return max_val;
}

void *thread_erosion(void *args)
{
    thread_args_t *targs = (thread_args_t *)args;
    for (int i = targs->start_row; i < targs->end_row; i++)
    {
        for (int j = 0; j < targs->input->cols; j++)
        {
            targs->output->data[i][j] = erosion(targs->input, i, j);
        }
    }
    sem_post(&semaphore);
    pthread_exit(NULL);
}

void *thread_dilation(void *args)
{
    thread_args_t *targs = (thread_args_t *)args;
    for (int i = targs->start_row; i < targs->end_row; i++)
    {
        for (int j = 0; j < targs->input->cols; j++)
        {
            targs->output->data[i][j] = dilation(targs->input, i, j);
        }
    }
    sem_post(&semaphore);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: %s <rows> <cols> <K> <max_threads>\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int K = atoi(argv[3]);
    int max_threads = atoi(argv[4]);

    if (rows <= 0 || cols <= 0 || K <= 0 || max_threads <= 0)
    {
        printf("Неправильные аргументы\n");
        return 1;
    }

    srand(time(NULL));

    matrix_t *A = initialize_matrix(rows, cols);
    matrix_t *B = initialize_matrix(rows, cols);
    matrix_t *C = initialize_matrix(rows, cols);

    sem_init(&semaphore, 0, max_threads); // 0 - междупотоками процесса

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int k = 0; k < K; k++)
    {

        int num_threads = max_threads;
        int rows_per_thread = (rows + num_threads - 1) / num_threads;
        pthread_t threads[num_threads];
        thread_args_t targs[num_threads];

        for (int t = 0; t < num_threads; t++)
        {
            int start_row = t * rows_per_thread;
            int end_row = (t + 1) * rows_per_thread;
            if (end_row > rows)
                end_row = rows;
            if (start_row >= rows)
                break;

            targs[t].input = A;
            targs[t].output = B;
            targs[t].start_row = start_row;
            targs[t].end_row = end_row;

            sem_wait(&semaphore);
            if (pthread_create(&threads[t], NULL, thread_erosion, &targs[t]) != 0)
            {
                perror("ошибка создания потока");
                return 1;
            }
        }

        for (int t = 0; t < num_threads; t++)
        {
            if (targs[t].start_row < rows)
            {
                pthread_join(threads[t], NULL);
            }
        }

        for (int t = 0; t < num_threads; t++)
        {
            int start_row = t * rows_per_thread;
            int end_row = (t + 1) * rows_per_thread;
            if (end_row > rows)
                end_row = rows;
            if (start_row >= rows)
                break;

            targs[t].input = B;
            targs[t].output = C;
            targs[t].start_row = start_row;
            targs[t].end_row = end_row;

            sem_wait(&semaphore);
            if (pthread_create(&threads[t], NULL, thread_dilation, &targs[t]) != 0)
            {
                perror("ошибка создания потока");
                return 1;
            }
        }

        for (int t = 0; t < num_threads; t++)
        {
            if (targs[t].start_row < rows)
            {
                pthread_join(threads[t], NULL);
            }
        }

        matrix_t *temp = A;
        A = C;
        C = temp;
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec);
    elapsed_time += (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Эрозия и наращивание выполнены %d итераций.\n", K);
    printf("Время выполнения: %.6f секунд\n", elapsed_time);
    printf("Число потоков: %d \n", max_threads);

    // printf("Resulting Matrix after Erosion (A):\n");
    // print_matrix(A);
    // printf("Resulting Matrix after Dilation (C):\n");
    // print_matrix(C);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);

    sem_destroy(&semaphore);

    return 0;
}
// matrix 2k x 2k 200=k

// Speedup 2 = 13.13 / 6.66 = 1,9714
// Efficiency 2 = 1,9714 / 2 = 0,9857

// Speedup 4 = 13.13 / 3.61 = 3,6371
// Efficiency 4 = 3,6371 / 4 = 0,909

// Speedup 16 = 13.13 / 1.55 = 8,4709
// Efficiency 16 = 8,4709 / 16 = 0,52943

// Speedup 32 = 13.13 / 1.55 = 8,4709
// Efficiency 16 = 8,4709 / 32 = 0,26471
