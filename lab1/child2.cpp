#include <iostream>
#include <unistd.h>

int main() {
    char inputChar;
    ssize_t bytesRead;

    while ((bytesRead = read(STDIN_FILENO, &inputChar, sizeof(inputChar))) > 0) {
        if (inputChar == ' ') {
            inputChar = '_';
        }
        if (write(STDOUT_FILENO, &inputChar, sizeof(inputChar)) == -1) {
            perror("Error writing output");
            return 1;
        }
    }

    if (bytesRead == -1) {
        perror("Error reading input");
        return 1;
    }

    return 0;
}
