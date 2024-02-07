// Date: 2021-04-20
// Windows-C

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

char Sname[] = "PList";
char version[] = "0.1.0";

int my_strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

BOOL print_process()
{
    // Create a snapshot of processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    //tant que l'on peut lire les processus
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // si on ne peut pas lire le premier processus
    // on ferme l'instantané
    if (!Process32First(hSnapshot, &pe32)) {
        printf("Error getting first process.\n");
        CloseHandle(hSnapshot);
        return FALSE;
    }
    // affiche les titres des colonnes
    printf("| %-30s| %5s | %5s | %5s | %7s | %18s | %16s |\n", "Name", "Pid", "Pri", "Thd", "Hnd", "Priv", "CPU Time", "Elapsed Time");
    while (Process32Next(hSnapshot, &pe32)) {
        // affiche les informations sur le processus
        printf("| %-30s| %5lu | %5ld | %5lu | %7d | %18ld | %16ld |\n", \
        pe32.szExeFile, \
        pe32.th32ProcessID, \
        pe32.pcPriClassBase, \
        pe32.th32ParentProcessID, \
        pe32.cntThreads, \
        //A CHANGER
        pe32.pcPriClassBase, \
        //A CHANGER
        pe32.pcPriClassBase);
    }
    //ici on ferme l'instantané 
    CloseHandle(hSnapshot);
    //on affiche un message de confirmation
    printf("First snapshot created successfully.\n");
    return TRUE;
}
/*
int check_arg(char argv1)
{
    if(argv1 == "--help" || argv1 == "-h")
    {
        printf("Help command was sent\n\n");
        printf("%-10s%-10s%s\n","--help","-h","Display this help and exit.");
        printf("%-10s%-10s%s\n","--version","-v","Display this help and exit.");
    }
    else if(argv1 == "--version" || argv1 == "-v"){
        printf("%s\nversion %s\n", Sname, version);
    }
    return 0;
}
*/
int main(int argc, char *argv[]) {

/*  if(argc == 2 && my_strlen(argv[1]) == 2 && argv[1][0] == '-' )
    {
        return(check_arg(argv[1]));
    }
    else*/ if(argc != 2) {
        return(print_process());
    }
    // Close the handle to the snapshot
    return 0;
}
