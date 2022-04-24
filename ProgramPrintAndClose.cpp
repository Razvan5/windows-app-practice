#define _CRT_SECURE_NO_WARNINGS
#define INFO_SIZE 500
#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>

struct InfoProces {
	DWORD pid;
	DWORD ppid;
	char  exeName[256];
};


int main()
{
	/*
	BOOL SetPrivilege(
		HANDLE hToken,          // access token handle
		LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
		BOOL bEnablePrivilege   // to enable or disable privilege
	);
	*/

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		L"ProcessTree");        // name of mapping object

	if (hMapFile == NULL)
	{
		printf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		printf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}

	MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}
