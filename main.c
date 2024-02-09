#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>

char Sname[] = "PList";
char version[] = "0.1.0";

int my_strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

BOOL print_process(BOOL detailFlag, const char* filterName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to create process snapshot.\n");
        return FALSE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        printf("Failed to get the first process.\n");
        CloseHandle(hSnapshot);
        return FALSE;
    }

    printf("| %-32s | %6s | %4s | %4s | %7s |\n", "Process Name", "PID", "Pri", "Thd", "Hnd");

    do {
        if (filterName == NULL || _tcsstr(pe32.szExeFile, filterName) != NULL) { // Applique le filtre ici
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            DWORD handleCount = 0;
            if (hProcess != NULL) {
                if (GetProcessHandleCount(hProcess, &handleCount)) {
                    printf("| %-32s | %6lu | %4ld | %4lu | %7lu |\n",
                           pe32.szExeFile,
                           pe32.th32ProcessID,
                           pe32.pcPriClassBase,
                           pe32.cntThreads,
                           handleCount);
                }
                CloseHandle(hProcess);
            } else {
                printf("| %-32s | %6lu | %4ld | %4lu | %7s |\n",
                       pe32.szExeFile,
                       pe32.th32ProcessID,
                       pe32.pcPriClassBase,
                       pe32.cntThreads,
                       "Permissions Denied");
            }
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    // printf("Snapshot of processes completed successfully.\n");
    return TRUE;
}

int main(int argc, char *argv[]) {
    BOOL helpFlag = FALSE;
    BOOL detailFlag = FALSE;
    char* filterName = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            helpFlag = TRUE;
        } else if (strcmp(argv[i], "-d") == 0) {
            detailFlag = TRUE;
        } else {
            filterName = argv[i];
        }
    }

    if (helpFlag) {
        printf("Usage: %s [options] [name]\n\n", argv[0]);
        printf("Options:\n");
        printf(" -h \t\tDisplay this help message and exit.\n");
        printf(" -d \t\tShow thread detail.\n");
        printf(" name \t\tShow information about processes that begin with the name specified.\n\n");
		printf("Abbreviation key:\n");
		printf(" PID \t\tProcess Identifier\n");
		printf(" Pri \t\tPriority\n");
		printf(" Thd \t\tNumber of Threads\n");
		printf(" Hnd \t\tNumber of Handles\n");
		printf(" CPU Time \t\t a remplir\n");
		printf(" Elapsed Time \t\t a remplir\n");
        return 0;
    }

    if (!print_process(detailFlag, filterName)) {
        printf("An error occurred while printing the process list.\n");
    }
    return 0;
}
