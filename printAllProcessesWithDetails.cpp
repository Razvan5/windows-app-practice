#define _CRT_SECURE_NO_WARNINGS
#define INFO_SIZE 500
#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>     //for using the function sleep

struct InfoProces {
	DWORD pid;
	DWORD ppid;
	char  exeName[256];
};

struct ProcessList {
	int         count;
	InfoProces  procese[2048];
};


int contor = 0;

int infoSize;

bool hasParent(InfoProces infoProcesList[INFO_SIZE], int index) {

	for (int i = 0; i < infoSize; ++i) {
		if (infoProcesList[index].ppid == infoProcesList[i].pid && i!=index)
			return true;
	}

	return false;

}

void PrintProcessTree(InfoProces infoProcesList[INFO_SIZE], int index , char* extraSpace) {

	printf("%s ppid: [%d] pid: %d  %s \n", extraSpace, infoProcesList[index].ppid, infoProcesList[index].pid, infoProcesList[index].exeName);

	char extraSpaceInside[1000] = "" ;
	sprintf(extraSpaceInside, "%s---->", extraSpace);

	for (int i = 0; i < infoSize; ++i) {
		if (infoProcesList[index].pid == infoProcesList[i].ppid && i != index) {
			PrintProcessTree(infoProcesList, i, extraSpaceInside);
		}
	}

}
void PrintProcessTreeRoots(InfoProces infoProcesList[INFO_SIZE], int index) {

	printf("\n\n[Arborele Nr. %d]\n\n", contor);
	contor++;
	printf("pid: %d  %s \n", infoProcesList[index].pid, infoProcesList[index].exeName);
	char extraSpace[1000] = "---->";

	for (int i = 0; i < infoSize; i++)
	{
		if (infoProcesList[index].pid == infoProcesList[i].ppid && i != index)
			PrintProcessTree(infoProcesList, i , extraSpace);
	}
}



int main() {
	PROCESS_INFORMATION processInformation;
	PROCESSENTRY32 processEntry32;

	HANDLE processesSnapShot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0
	);

	if (processesSnapShot == INVALID_HANDLE_VALUE) {
		printf("CreateToolHelp32Snapshot has failed : %d \n", GetLastError());
		return -1;
	}

	processEntry32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(processesSnapShot, &processEntry32)) {
		printf("The first entry of the process has destroyed this app :( %d \n", GetLastError());
		return -2;
	}

	int index = 0;
	//InfoProces processVector[INFO_SIZE];
	ProcessList processList[1];

	processList[0].count=0;

	do {
		InfoProces ip;
		//ip.pid = processEntry32.th32ProcessID;
		//ip.ppid = processEntry32.th32ParentProcessID;
		//sprintf(ip.exeName, "%ws\0", processEntry32.szExeFile);
		printf("%d", processList[0].count);
		processList[0].procese[processList[0].count].pid = processEntry32.th32ProcessID;
		processList[0].procese[processList[0].count].ppid = processEntry32.th32ParentProcessID;
		sprintf(processList[0].procese[processList[0].count].exeName, "%ws\0", processEntry32.szExeFile);
		processList[0].count++;

	}while (Process32Next(processesSnapShot, &processEntry32));
	

	for (int i = 0; i < processList[0].count - 1; ++i) {
		for (int j = i+1; j < processList[0].count; ++j) {
			if (processList[0].procese[i].ppid > processList[0].procese[j].ppid) {
				InfoProces aux = processList[0].procese[i];
				processList[0].procese[i] = processList[0].procese[j];
				processList[0].procese[j] = aux;
			}
		}
	}
	
	HANDLE hFileMapping =  CreateFileMapping(
		_In_	  INVALID_HANDLE_VALUE,
		_In_opt_  NULL,
		_In_      PAGE_READWRITE,
		_In_      NULL,
		_In_      sizeof processList,
		_In_opt_  L"ProcessTree");

	if (hFileMapping == INVALID_HANDLE_VALUE) {
		printf("Eroare cu handle la fisierul mapat %d", GetLastError());
	}

	printf("%d", hFileMapping);

	for (int i = 0; i < processList[0].count; i++) {
		printf("_____________________________________________\n");
		printf("Process ID  : %u\n", processList[0].procese[i].pid);
		printf("Parent  ID  : %u\n", processList[0].procese[i].ppid);
		printf("Process Name: %s\n", processList[0].procese[i].exeName);
		printf("_____________________________________________\n");
	}

	ProcessList* pBuf = 
		(ProcessList*)MapViewOfFile(
		_In_ hFileMapping,
		_In_ FILE_MAP_ALL_ACCESS,
		_In_ 0,
		_In_ 0,
		_In_ 0);

	printf("%d\n", sizeof processList[0].procese);
	printf("%d\n", sizeof pBuf);

	

	memcpy(pBuf, processList, sizeof processList);

	for (int i = 0; i < processList[0].count; i++) {
		printf("_____________________________________________\n");
		printf("Process ID  : %u\n", processList[0].procese[i].pid);
		printf("Parent  ID  : %u\n", processList[0].procese[i].ppid);
		printf("Process Name: %s\n", processList[0].procese[i].exeName);
		printf("_____________________________________________\n");
	}

	bool keepAlive = 1;

	while (keepAlive)
	{
		scanf("%d", &keepAlive);
	}

	Sleep(100000000);

	CloseHandle(hFileMapping);
	CloseHandle(processesSnapShot);
	
	return 0;
}