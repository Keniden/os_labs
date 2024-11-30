#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

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

    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    shm_ptr = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    while (shm_ptr->parent_ready == 0)
    {
        sleep(1000);
    }

    for (int i = 0; shm_ptr->input_str[i] && i < MAX_LEN; i++)
    {
        shm_ptr->upper_str[i] = toupper((unsigned char)shm_ptr->input_str[i]);
    }

    shm_ptr->upper_str[strlen(shm_ptr->input_str)] = '\0';

    shm_ptr->child1_done = 1;

    munmap(shm_ptr, sizeof(shared_data));
    close(shm_fd);

    return 0;
}
