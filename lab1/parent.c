#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

HANDLE g_hChild1_IN_Rd = NULL;
HANDLE g_hChild1_OUT_Wr = NULL;

HANDLE g_hChild2_IN_Rd = NULL;
HANDLE g_hChild2_OUT_Wr = NULL;

HANDLE g_hParent_IN_Rd = NULL;
HANDLE g_hParent_OUT_Wr = NULL;

int CreateChildProcess();
void PrintError(const char *msg);

int main()
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&g_hChild1_IN_Rd, &g_hParent_OUT_Wr, &saAttr, 0))
    {
        PrintError("Eror pipe g_hChild1_IN_Rd and g_hParent_OUT_Wr");
        return 1;
    }

    if (!CreatePipe(&g_hChild2_IN_Rd, &g_hChild1_OUT_Wr, &saAttr, 0))
    {
        PrintError("Eror pipe g_hChild2_IN_Rd and g_hChild1_OUT_Wr");
        return 1;
    }

    if (!CreatePipe(&g_hParent_IN_Rd, &g_hChild2_OUT_Wr, &saAttr, 0))
    {
        PrintError("Eror pipe g_hParent_IN_Rd and g_hChild2_OUT_Wr");
        return 1;
    }

    if (CreateChildProcess() != 0)
    {
        return 1;
    }

    char input[BUFFER_SIZE];
    printf("Write: ");
    if (fgets(input, BUFFER_SIZE, stdin) == NULL)
    {
        perror("Error input");
        return 1;
    }

    DWORD dwWritten, dwRead;

    if (!WriteFile(g_hParent_OUT_Wr, input, strlen(input), &dwWritten, NULL))
    {
        PrintError("error write in g_hParent_OUT_Wr");
        return 1;
    }

    CloseHandle(g_hParent_OUT_Wr);

    char result[BUFFER_SIZE];

    if (!ReadFile(g_hParent_IN_Rd, result, BUFFER_SIZE, &dwRead, NULL))
    {
        PrintError("error read from g_hParent_IN_Rd");
        return 1;
    }

    printf("result: %s", result);

    CloseHandle(g_hParent_IN_Rd);

    return 0;
}

int CreateChildProcess()
{
    PROCESS_INFORMATION pi1, pi2;
    STARTUPINFO si1, si2;

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    si1.hStdInput = g_hChild1_IN_Rd;
    si1.hStdOutput = g_hChild1_OUT_Wr;
    si1.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    si2.hStdInput = g_hChild2_IN_Rd;
    si2.hStdOutput = g_hChild2_OUT_Wr;
    si2.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(
            NULL,
            "child1.exe",
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &si1,
            &pi1))
    {
        PrintError("error create child1.exe");
        return 1;
    }

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);

    if (!CreateProcess(
            NULL,
            "child2.exe",
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &si2,
            &pi2))
    {
        PrintError("error create child2.exe");
        return 1;
    }

    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}

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
