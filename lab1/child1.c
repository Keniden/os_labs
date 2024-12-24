#include <stdio.h>
#include <windows.h>
#include <ctype.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void PrintError(const char *msg)
{
    DWORD eNum;
    TCHAR sysMsg[512];
    eNum = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        sysMsg,
        sizeof(sysMsg) / sizeof(TCHAR),
        NULL);
    fprintf(stderr, "%s: %s\n", msg, sysMsg);
}

int main()
{
    char buffer[BUFFER_SIZE];

    HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);
    if (readHandle == INVALID_HANDLE_VALUE)
    {
        PrintError("error diskr input");
        return 1;
    }

    HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (writeHandle == INVALID_HANDLE_VALUE)
    {
        PrintError("error diskr output");
        return 1;
    }

    DWORD readedBytes, writedBytes;

    BOOL readResult = ReadFile(readHandle, buffer, BUFFER_SIZE, &readedBytes, NULL);
    if (!readResult)
    {
        PrintError("error read from std");
        return 1;
    }

    if (readedBytes == 0)
    {
        fprintf(stderr, "no data for proccess\n");
        return 0;
    }

    for (DWORD i = 0; i < readedBytes; i++)
    {
        buffer[i] = toupper((unsigned char)buffer[i]);
    }

    BOOL writeResult = WriteFile(writeHandle, buffer, readedBytes, &writedBytes, NULL);
    if (!writeResult)
    {
        PrintError("error write in std out");
        return 1;
    }

    if (writedBytes != readedBytes)
    {
        fprintf(stderr, "not all bytes were written\n");
    }

    return 0;
}
