#include <unistd.h>
#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>

int create_process() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork error");
        exit(-1);
    }
    return pid;
}

void create_pipe(int* pipe_fd) {
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation error");
        exit(-1);
    }
}

void dup_fd(int oldfd, int newfd) {
    if (dup2(oldfd, newfd) == -1) {
        perror("dup2 error");
        exit(-1);
    }
}

int main() {
    int pipe1_fd[2], pipe2_fd[2];
    create_pipe(pipe1_fd);
    create_pipe(pipe2_fd);

    pid_t child1 = create_process();
    if (child1 == 0) {
        close(pipe1_fd[1]);
        close(pipe2_fd[0]);

        int pipech_fd[2];
        create_pipe(pipech_fd);

        pid_t child2 = create_process();

        if (child2 == 0) {
            close(pipech_fd[0]);
            close(pipe2_fd[1]);

            dup_fd(pipe1_fd[0], STDIN_FILENO);
            dup_fd(pipech_fd[1], STDOUT_FILENO);

            execl("../build/child2", "../build/child2", nullptr);

            perror("execl failed in child2");
            close(pipech_fd[1]);
            close(pipe1_fd[0]);
            exit(-1);
        } else {
            close(pipe1_fd[0]);
            close(pipech_fd[1]);

            dup_fd(pipech_fd[0], STDIN_FILENO);
            dup_fd(pipe2_fd[1], STDOUT_FILENO);

            execl("../build/child1", "../build/child1", nullptr);

            perror("execl failed in child1");
            close(pipech_fd[0]);
            close(pipe2_fd[1]);
            exit(-1);
        }
    } else {
        close(pipe1_fd[0]);
        close(pipe2_fd[1]);
        char c = getchar();
        while (c != EOF) {
            write(pipe1_fd[1], &c, sizeof(c));
            read(pipe2_fd[0], &c, sizeof(c));
            putchar(c);
            c = getchar();
        }

        close(pipe1_fd[1]);
        close(pipe2_fd[0]);
    }
    
    return 0;
}
