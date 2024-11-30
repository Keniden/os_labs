#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define SHM_NAME "/shm_example"
#define MAX_LEN 1024

typedef struct
{
    char input_str[MAX_LEN];
    char upper_str[MAX_LEN];
    char final_str[MAX_LEN];
    int parent_ready;
    int child1_done;
    int child2_done;
} shared_data;

int main()
{
    int shm_fd;
    shared_data *shm_ptr;
    pid_t pid1, pid2;
    char *input_str = NULL;
    size_t len = 0;
    ssize_t read;

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(shared_data)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    shm_ptr = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shm_ptr->parent_ready = 0;
    shm_ptr->child1_done = 0;
    shm_ptr->child2_done = 0;

    printf("Введите строку: ");
    read = getline(&input_str, &len, stdin);
    if (read == -1)
    {
        perror("getline");
        exit(EXIT_FAILURE);
    }
    if (input_str[read - 1] == '\n')
        input_str[read - 1] = '\0';

    strncpy(shm_ptr->input_str, input_str, MAX_LEN);

    shm_ptr->parent_ready = 1;

    pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    {
        execl("./child1", "./child1", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    waitpid(pid1, NULL, 0);

    pid2 = fork();
    if (pid2 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid2 == 0)
    {
        execl("./child2", "./child2", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    waitpid(pid2, NULL, 0);

    // while (shm_ptr->child2_done == 0)
    // {
    //     sleep(1000);
    // }

    printf("Результат после обработки: %s\n", shm_ptr->final_str);

    munmap(shm_ptr, sizeof(shared_data));
    shm_unlink(SHM_NAME);
    free(input_str);

    return 0;
}

// strace - f - o ouput.txt parent`

