#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

int my_strlen(const char *str) {
    int i = 0;
    while (str[i] != '\\0') {
        i++;
    }
    return i;
}

BOOL print_process(BOOL detailFlag, const char* filterName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to create process snapshot.\\n");
        return FALSE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        printf("Failed to get the first process.\\n");
        CloseHandle(hSnapshot);
        return FALSE;
    }

    printf("| %-32s | %6s | %4s | %4s | %7s | %15s | %12s |\\n", "Process Name", "PID", "Pri", "Thd", "Hnd", "CPU Time", "Elapsed Time");

    SYSTEMTIME st;
    FILETIME ftNow;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ftNow);

    do {
        if (filterName == NULL || _tcsstr(pe32.szExeFile, filterName) != NULL) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            DWORD handleCount = 0;
            FILETIME ftCreation, ftExit, ftKernel, ftUser;
            if (hProcess != NULL) {
                if (GetProcessHandleCount(hProcess, &handleCount) && GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                    ULARGE_INTEGER uliKernel, uliUser, uliCreation, uliNow;
                    uliKernel.HighPart = ftKernel.dwHighDateTime;
                    uliKernel.LowPart = ftKernel.dwLowDateTime;
                    uliUser.HighPart = ftUser.dwHighDateTime;
                    uliUser.LowPart = ftUser.dwLowDateTime;
                    uliCreation.HighPart = ftCreation.dwHighDateTime;
                    uliCreation.LowPart = ftCreation.dwLowDateTime;
                    uliNow.HighPart = ftNow.dwHighDateTime;
                    uliNow.LowPart = ftNow.dwLowDateTime;

                    ULONGLONG totalCPUTime100ns = uliKernel.QuadPart + uliUser.QuadPart;
                    ULONGLONG processElapsedTime100ns = uliNow.QuadPart - uliCreation.QuadPart;

                    // Conversion en millisecondes
                    ULONGLONG totalCPUTimeMillisec = totalCPUTime100ns / 10000ULL;
                    ULONGLONG processElapsedTimeMillisec = processElapsedTime100ns / 10000ULL;

                    // Calcul des heures, minutes, secondes et millisecondes pour le CPU Time
                    UINT hours = (UINT)(totalCPUTimeMillisec / (1000 * 60 * 60));
                    UINT minutes = (UINT)((totalCPUTimeMillisec / (1000 * 60)) % 60);
                    UINT seconds = (UINT)((totalCPUTimeMillisec / 1000) % 60);
                    UINT milliseconds = (UINT)(totalCPUTimeMillisec % 1000);

                    // Calcul des heures, minutes, secondes pour Elapsed Time
                    UINT eHours = (UINT)(processElapsedTimeMillisec / (1000 * 60 * 60));
                    UINT eMinutes = (UINT)((processElapsedTimeMillisec / (1000 * 60)) % 60);
                    UINT eSeconds = (UINT)((processElapsedTimeMillisec / 1000) % 60);

                    printf("| %-32s | %6lu | %4ld | %4lu | %7lu | %02u:%02u:%02u.%03u | %02u:%02u:%02u |\\n",
                           pe32.szExeFile,
                           pe32.th32ProcessID,
                           pe32.pcPriClassBase,
                           pe32.cntThreads,
                           handleCount,
                           hours, minutes, seconds, milliseconds,
                           eHours, eMinutes, eSeconds);
                } else {
                    printf("| %-32s | %6lu | %4ld | %4lu | %7s | %15s | %12s |\\n",
                           pe32.szExeFile,
                           pe32.th32ProcessID,
                           pe32.pcPriClassBase,
                           pe32.cntThreads,
                           "N/A",