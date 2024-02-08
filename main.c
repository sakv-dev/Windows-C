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

BOOL print_process() {
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
    	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
    	DWORD handleCount = 0;
    	if (hProcess != NULL) {
        	if (GetProcessHandleCount(hProcess, &handleCount)) {
            	printf("| %-32s | %6lu | %4ld | %4lu | %7lu |\n",
                   	pe32.szExeFile,
                   	pe32.th32ProcessID,
                   	pe32.pcPriClassBase,
                   	pe32.cntThreads,
                   	handleCount); // marche pas 
        	}
        	CloseHandle(hProcess);
    	} else {
        	printf("| %-32s | %6lu | %4ld | %4lu | %7s |\n",
               	pe32.szExeFile,
               	pe32.th32ProcessID,
               	pe32.pcPriClassBase,
               	pe32.cntThreads,
               	"N/A");
    	}
	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
	printf("Snapshot of processes completed successfully.\n");
	return TRUE;
}

int main() {
	print_process();
	return 0;
}
