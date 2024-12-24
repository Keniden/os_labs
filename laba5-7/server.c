#include "tree.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <zmq.h>

int accessed = 0;
int locker = 0;
char message[256];

// Рекурсивное удаление узлов и отправка команды kill
void kill_with_children(node *tree, void *publisher)
{
    if (!tree)
    {
        return;
    }

    sprintf(message, "kill %d", tree->key);
    zmq_send(publisher, message, strlen(message), 0);
    memset(message, 0, sizeof(message));

    kill_with_children(tree->left, publisher);
    kill_with_children(tree->right, publisher);
}

int main()
{
    // Инициализация ZeroMQ
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://127.0.0.1:5555");

    char input[256];
    char command[100];
    char id_str[sizeof(int)];
    int arg1 = 0, arg2 = 0, left_count = 0, right_count = 0;
    pid_t id;
    bool is_parent = false;

    // Создание управляющего узла с ключом 5
    node *tree = create(NULL, 5);

    while (1)
    {
        memset(message, 0, sizeof(message));
        memset(command, 0, sizeof(command));

        // Чтение команды
        if (!fgets(input, sizeof(input), stdin))
        {
            break;
        }

        sscanf(input, "%s", command);

        if (strcmp(command, "create") == 0)
        { // Команда создания узла
            sscanf(input, "%*s %d %d", &arg1, &arg2);
            node *parent = search(tree, arg2);
            node *child = search(tree, arg1);

            if (!parent)
            {
                printf("Error: No such parent\n");
                continue;
            }
            if (child)
            {
                printf("Error: Already exists\n");
                continue;
            }

            // Проверка ограничения числа дочерних узлов
            if (arg1 > arg2)
                right_count++;
            else
                left_count++;

            if (left_count > 1 || right_count > 1)
            {
                printf("Nope\n");
                continue;
            }

            tree = add(tree, arg1);
            print_tree(tree, 0);

            id = fork();
            if (id == -1)
            {
                perror("fork");
                return -1;
            }
            if (id == 0)
            { // Потомок
                sprintf(id_str, "%d", arg1);
                execl("./user", "./user", id_str, NULL);
                perror("execl");
            }
            else
            { // Родитель
                printf("Ok: %d\n", id);
            }
        }
        else if (strcmp(command, "exec") == 0)
        { // Команда exec
            sscanf(input, "%*s %d", &arg1);

            if (!search(tree, arg1))
            {
                printf("Error: id Not found\n");
                continue;
            }

            zmq_send(publisher, input, strlen(input), 0);
            printf("Command sent to id %d\n", arg1);
        }
        else if (strcmp(command, "kill") == 0)
        { // Команда kill
            sscanf(input, "%*s %d", &arg1);
            node *target = search(tree, arg1);

            if (!target)
            {
                printf("Error: id Not Found\n");
                continue;
            }

            if (arg1 == 5)
            {
                printf("You can't kill the control process\n");
                continue;
            }

            kill_with_children(arg1 > 5 ? target->right : target->left, publisher);
            zmq_send(publisher, input, strlen(input), 0);

            disableNode(arg1 > 5 ? target->right : target->left);
            disableOneNode(target);
        }
        else if (strcmp(command, "ping") == 0)
        { // Команда ping
            sscanf(input, "%*s %d", &arg1);
            node *node_to_check = search(tree, arg1);

            if (arg1 == 5)
            {
                printf("Control process is always available\n");
                continue;
            }
            if (!node_to_check)
            {
                printf("Error: id not found\n");
                continue;
            }

            printf("Ok: %d // Node %d is %s\n", node_to_check->exist, arg1, node_to_check->exist ? "available" : "unavailable");
        }
    }

    // Очистка ресурсов
    zmq_close(publisher);
    zmq_ctx_destroy(context);
}
