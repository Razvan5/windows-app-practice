#include <Windows.h>
#include <random>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

using namespace std::chrono;

int main()
{
	HANDLE hMapFileA, hMapFileB;

	hMapFileA = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		L"MutexA");               // name of mapping object
	hMapFileB = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		L"MutexB");

	if (hMapFileA == NULL || hMapFileB == NULL)
	{
		printf("Could not open file mapping object (%d).\n", GetLastError());
		return 1;
	}

	int* pBufA = (int*)MapViewOfFile(
		_In_ hMapFileA,
		_In_ FILE_MAP_ALL_ACCESS,
		_In_ 0,
		_In_ 0,
		_In_ 0);

	int* pBufB = (int*)MapViewOfFile(
		_In_ hMapFileB,
		_In_ FILE_MAP_ALL_ACCESS,
		_In_ 0,
		_In_ 0,
		_In_ 0);

	if (pBufA == NULL || pBufB == NULL)
	{
		printf("Could not map view of file (%d).\n", GetLastError());

		CloseHandle(hMapFileA);
		CloseHandle(hMapFileB);

		return 1;
	}

	HANDLE handleMutex = OpenMutex(
		_In_opt_ NULL,
		_In_	 false,
		_In_opt_ L"mutexSharingAB");
	WaitForSingleObject(handleMutex, INFINITE);
		printf("%d", WaitForSingleObject(handleMutex, INFINITE));
		printf("Program2 -> %d %d \n", *pBufA, *pBufB);
		printf(*pBufA * 2 == *pBufB ? "Program2 -> true\n" : "Program2 -> false\n");
	ReleaseMutex(handleMutex);

	return 0;
}

