#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256
#define INFO_SIZE 500

struct InfoProces {
	DWORD pid;
	DWORD ppid;
	char  exeName[256];
};

struct ProcessList {
	int         count;
	InfoProces  procese[2048];
};

int infoSize;
int contor = 0;
int terminate_contor = 0;

bool hasParent(ProcessList infoProcesList, int index) {

	for (int i = 0; i < infoProcesList.count; ++i) {
		if (infoProcesList.procese[index].ppid == infoProcesList.procese[i].pid && i != index)
			return true;
	}

	return false;

}

void PrintProcessTree(ProcessList infoProcesList, int index, char* extraSpace) {

	printf("%s ppid: [%d] pid: %d  %s \n", extraSpace, infoProcesList.procese[index].ppid, infoProcesList.procese[index].pid, infoProcesList.procese[index].exeName);

	char extraSpaceInside[500] = "";
	sprintf(extraSpaceInside, "%s---->", extraSpace);

	for (int i = 0; i < infoProcesList.count; ++i) {
		if (infoProcesList.procese[index].pid == infoProcesList.procese[i].ppid && i != index) {
			PrintProcessTree(infoProcesList, i, extraSpaceInside);
		}
	}

}

void PrintProcessTreeRoots(ProcessList infoProcesList, int index) {

	printf("\n\n[Arborele Nr. %d]\n\n", contor);
	contor++;
	printf("pid: %d  %s \n", infoProcesList.procese[index].pid, infoProcesList.procese[index].exeName);
	char extraSpace[500] = "---->";

	for (int i = 0; i < infoProcesList.count; i++)
	{
		if (infoProcesList.procese[index].pid == infoProcesList.procese[i].ppid && i != index)
			PrintProcessTree(infoProcesList, i, extraSpace);
	}
}

void TerminateProcessTree(ProcessList infoProcesList, int index) {

	for (int i = 0; i < infoProcesList.count; ++i) {
		if (infoProcesList.procese[index].pid == infoProcesList.procese[i].ppid && i != index) {
			TerminateProcessTree(infoProcesList, i);
			
			HANDLE processHandle = OpenProcess(
				PROCESS_TERMINATE | PROCESS_VM_READ,
				FALSE,
				infoProcesList.procese[i].pid);

			BOOL result = TerminateProcess(processHandle, NULL);
			WaitForSingleObject(processHandle, 1000);
			printf("%s\n",infoProcesList.procese[i].exeName);
			CloseHandle(processHandle);
		}
	}

}

void TerminateProcessTreeRoots(ProcessList infoProcesList, int index) {

	printf("\n\n[Arborele Nr. %d]\n\n", terminate_contor);
			
	TerminateProcessTree(infoProcesList, index);

	HANDLE processHandle = OpenProcess(PROCESS_TERMINATE | PROCESS_VM_READ,
		FALSE, infoProcesList.procese[index].pid);
			
	BOOL result = TerminateProcess(processHandle, NULL);
	WaitForSingleObject(processHandle, 1000);
	printf("%s\n",infoProcesList.procese[index].exeName);
	CloseHandle(processHandle);

	
}

int main()
{
	HANDLE hMapFile;
	ProcessList* pBuf;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		L"ProcessTree");               // name of mapping object

	if (hMapFile == NULL)
	{
		printf("Could not open file mapping object (%d).\n",GetLastError());
		return 1;
	}

	pBuf = (ProcessList*)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		0);

	if (pBuf == NULL)
	{
		printf("Could not map view of file (%d).\n",GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}

	printf("Buffer size: %d", pBuf[0].count);
	for (int k = 0; k < pBuf[0].count; ++k) {
		if (!hasParent(pBuf[0], k)) {
			PrintProcessTreeRoots(pBuf[0], k);
		}
	}

	int number = 0;
	printf("Introdu un numar intre 0 si %d", contor - 1);
	scanf("%d", &number);
	do {

		for (int k = 0; k < pBuf[0].count; ++k) {
			if (!hasParent(pBuf[0], k)) {
				if (terminate_contor == number) {
					TerminateProcessTreeRoots(pBuf[0], k);
				}
				terminate_contor++;
				printf("%d\n", terminate_contor);
			}
		}
		printf("Introdu un numar intre 0 si %d\n", contor - 1);
		scanf("%d", &number);
	}
	while (number != -1);
	

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}