#include <iostream>
#include <unistd.h>

int main() {
    char inputChar;
    ssize_t bytesRead;
    while ((bytesRead = read(STDIN_FILENO, &inputChar, sizeof(inputChar))) > 0) {
        if (bytesRead == sizeof(inputChar)) {
            inputChar = toupper(inputChar);
            write(STDOUT_FILENO, &inputChar, sizeof(inputChar));
        }
    }
    if (bytesRead == -1) {
        perror("Error reading input");
    }

    return 0;
}
