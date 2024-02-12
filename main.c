#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>


int my_strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}
// Cette fonction retourne bool:
// Elle a comme paramètres : un boolean en detailflag et une chaine de caratère pour filtrer les processus par leur nom
BOOL print_process(BOOL detailFlag, const char* filterName) {
    // Elle créée un clicjés instantané sur les processus en cours d'execution
    // Le second argument, 0, spécifie que l'instantané concerne tous les processus du système. La fonction retourne un HANDLE vers l'instantané créé.
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to create process snapshot.\n");
        return FALSE;
    }

    //ici PROCESSENTRY32  déclare une varibale pe32. Cette strcuture contient plusieurs champs pouvant stocker des informations sur le processus (PID, Threads etc..)
    PROCESSENTRY32 pe32;
    //Cette étape est obligatoire : elle servira a utiliser Process32First ou Process32Next avec pe32
    //dwSize est déninie avec la taille de la structure de PROCESSENTRY32
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Le Process32Firsrt se remplit  par la suite
    // Si le Process32First n'existe pas alors, il fermera le Handle et passera au prochaine processus
    if (!Process32First(hSnapshot, &pe32)) {
        printf("Failed to get the first process.\n");
        CloseHandle(hSnapshot);
        return FALSE;
    }
    
    //cette ligne imprime l'en-tête d'une table qui sera utilisé pour afficher les informations sur chaque processus
    //Les spécificateurs de format dans printf sont utilisés pour allouer un espace fixe à chaque colonne.
    printf("| %-32s | %6s | %4s | %4s | %4s | %12s | %12s |\n", "Process Name", "PID", "Pri", "Thd", "Hnd", "CPU Time", "Elapsed Time");

    // on utilise SYSTEMTIME pour représenter une date et une heure dans un format facilement compréhensible( année, mois , jour, heures, minutes, secondes et les milisecondes)
	SYSTEMTIME st;
    // il sert aussi à représenter les dates et heures mais est moins intuitifs, mais elle est utiliser par les API de Windows 
    FILETIME ftNow;
    //Cette appelle de fonction permet de remplir la structure de st avec l'heure système actuelle ²
    GetSystemTime(&st);
    //convertir l'heure du systeme en FILETIME
    SystemTimeToFileTime(&st, &ftNow);
// Le do while (faire tant que) sert à exécuter tant que le condition est valide
do {
        //verification si existance du filtre
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

                    printf("| %-32s | %6lu | %4ld | %4lu | %7lu | %02u:%02u:%02u.%03u | %02u:%02u:%02u |\n",
                           pe32.szExeFile,
                           pe32.th32ProcessID,
                           pe32.pcPriClassBase,
                           pe32.cntThreads,
                           handleCount,
                           hours, minutes, seconds, milliseconds,
                           eHours, eMinutes, eSeconds);
                } else {
					printf("| %-32s | %6lu | %4ld | %4lu | %7lu | %02u:%02u:%02u.%03u | %02u:%02u:%02u |\n",
                           pe32.szExeFile,
                           pe32.th32ProcessID,
                           pe32.pcPriClassBase,
                           pe32.cntThreads,
                           0,
                           0,0,0,0,
                           0,0,0);
                }
                CloseHandle(hProcess);
            } else {
                    printf("| %-32s | %6lu | %4ld | %4lu | %7lu | %02u:%02u:%02u.%03u | %02u:%02u:%02u |\n",
                       pe32.szExeFile,
                       pe32.th32ProcessID,
                       pe32.pcPriClassBase,
                       pe32.cntThreads,
                       0,
                       0,0,0,0,
                       0,0,0);
            }
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    // printf("Snapshot of processes completed successfully.\n");
    return TRUE;
}

BOOL print_thread_details(DWORD processID) {
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    THREADENTRY32 te32;
    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    SYSTEMTIME stKernel, stUser;
    ULARGE_INTEGER uiKernel, uiUser;

    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(hThreadSnap == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    te32.dwSize = sizeof(THREADENTRY32);

    if(!Thread32First(hThreadSnap, &te32)) {
        CloseHandle(hThreadSnap);
        return FALSE;
    }
    printf("| %-6s | %-3s | %-6s | %-6s | %-12s | %-12s | %-12s |\n", "Tid", "Pri", "Cswtch", "State", "User Time", "Kernel Time", "Elapsed Time");
    do {
        if(te32.th32OwnerProcessID == processID) {
            HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
            if(hThread && GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                FileTimeToSystemTime(&ftKernel, &stKernel);
                FileTimeToSystemTime(&ftUser, &stUser);

                uiKernel.LowPart = ftKernel.dwLowDateTime;
                uiKernel.HighPart = ftKernel.dwHighDateTime;
                uiUser.LowPart = ftUser.dwLowDateTime;
                uiUser.HighPart = ftUser.dwHighDateTime;
                printf("| %-6lu | %-3ld | %-6s | %-6s | %02u:%02u:%02u.%03u | %02u:%02u:%02u.%03u | %-12s |\n",
                       te32.th32ThreadID,
                       te32.tpBasePri,
                       "N/A",  //pas réussi à récupérer
                       "N/A",  //pas réussi à récupéer
                       stUser.wHour, stUser.wMinute, stUser.wSecond, stUser.wMilliseconds,
                       stKernel.wHour, stKernel.wMinute, stKernel.wSecond, stKernel.wMilliseconds,
                       "N/A");  //pas réussi à récupérer
            }
            if (hThread) {
                CloseHandle(hThread);
            }

        }

    } while(Thread32Next(hThreadSnap, &te32));
    CloseHandle(hThreadSnap);
    return TRUE;
}



int main(int argc, char *argv[]) {
    BOOL helpFlag = FALSE;
    BOOL detailFlag = FALSE;
    char* filterName = NULL;
    DWORD pid = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            helpFlag = TRUE;
        } else if (strcmp(argv[i], "-d") == 0) {
            detailFlag = TRUE;
            if(i + 1 < argc) {
                pid = atoi(argv[i + 1]); 
                i++; 
            }
        } else {
            filterName = argv[i];
        }
    }

    if (helpFlag) {
        // Display help
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
        printf(" CPU Time \tCPU Time in seconds\n");
        printf(" Elapsed Time \t\tElapsed Time in seconds\n");
        return 0;
    }

    if (detailFlag && pid != 0) {
        if (!print_thread_details(pid)) { // Correction ici
            printf("An error occurred while printing the thread list.\n");
        }
        return 0; 
    }   

    if (!print_process(detailFlag, filterName)) {
        printf("An error occurred while printing the process list.\n");
    }
    return 0;
}


