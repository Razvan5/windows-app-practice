#include <Windows.h>
#include <random>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

using namespace std::chrono;

struct rand_int {
	int random_number;
	int his_double;
};

int main()
{
	rand_int sent;
	HANDLE hFileMappingA = CreateFileMapping(
		_In_	  INVALID_HANDLE_VALUE,
		_In_opt_  NULL,
		_In_      PAGE_READWRITE,
		_In_      NULL,
		_In_      sizeof sent,
		_In_opt_  L"MutexA");
	HANDLE hFileMappingB = CreateFileMapping(
		_In_	  INVALID_HANDLE_VALUE,
		_In_opt_  NULL,
		_In_      PAGE_READWRITE,
		_In_      NULL,
		_In_      sizeof sent,
		_In_opt_  L"MutexB");

	if (hFileMappingA == INVALID_HANDLE_VALUE || hFileMappingB == INVALID_HANDLE_VALUE) {
		printf("Eroare cu handle la fisierul mapat %d", GetLastError());
	}
	int* pBufA = (int*)MapViewOfFile(
			_In_ hFileMappingA,
			_In_ FILE_MAP_ALL_ACCESS,
			_In_ 0,
			_In_ 0,
			_In_ 0);
	int* pBufB = (int*)MapViewOfFile(
		_In_ hFileMappingB,
		_In_ FILE_MAP_ALL_ACCESS,
		_In_ 0,
		_In_ 0,
		_In_ 0);

	HANDLE handleMutex = CreateMutex(
		_In_opt_ NULL,
		_In_	 true,
		_In_opt_ L"mutexSharingAB");


	int prime_vector[100] = { 52489, 1153, 1297, 1459, 2593, 2917, 3889, 10369, 12289, 17497, 3457, 18433, 39367 };
	for (int i = 0; i <= 500; ++i) {

		milliseconds ms = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
			);
		unsigned long miliseconds = ms.count();

		srand((miliseconds + i) % prime_vector[i%13]);

		WaitForSingleObject(handleMutex, INFINITE);
			int a = rand();
			int b = 2 * a;

			printf("Program1 -> a: %d  b:%d \n", a, b);

			sent.random_number = a;
			sent.his_double = b;

			memcpy(pBufA, &a, sizeof a);
			memcpy(pBufB, &b, sizeof b);

			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);

			if (!CreateProcess(L"H:\\8.CSSO\\Tema Completa 3 (5-6)\\Tema3(5-6)Part2\\Tema3(5-6)Part2\\Debug\\Tema3(5-6)Part2.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
				printf("Cannot create process.\n");
				return 0;
			}
		ReleaseMutex(handleMutex);


		//printf("%d %d\n", pBuf->random_number,pBuf->his_double);

		/*
		if (pBuf->random_number * 2 == pBuf->his_double) {
			printf("Corect \n");
		}
		else {
			printf("Gresit \n");
		}
		*/
		
	}


	UnmapViewOfFile(pBufA);
	UnmapViewOfFile(pBufB);
	CloseHandle(hFileMappingA);
	CloseHandle(hFileMappingB);
	return 0;
}

