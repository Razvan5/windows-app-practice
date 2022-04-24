#include <Windows.h>
#include <stdio.h>

void f(DWORD* dw) {

}

void g(LPDWORD dw) {

}

void searchRegistry(HKEY hKeyRoot, char* currentFolderTree) {
	DWORD keysNumber = 0;
	DWORD maxSubKeyLen = 0;
	DWORD valuesNumber = 0;
	DWORD maxValueLen = 0;

	RegQueryInfoKey(
		hKeyRoot,			//  _In_ HKEY hKey,
		NULL,			//	_Out_writes_to_opt_(*lpcchClass,*lpcchClass + 1) LPWSTR lpClass,
		NULL,			//	_Inout_opt_ LPDWORD lpcchClass,
		NULL,			//	_Reserved_ LPDWORD lpReserved,
		&keysNumber,		//	_Out_opt_ LPDWORD lpcSubKeys,
		&maxSubKeyLen,			//	_Out_opt_ LPDWORD lpcbMaxSubKeyLen,
		NULL,			//	_Out_opt_ LPDWORD lpcbMaxClassLen,
		&valuesNumber,	//	_Out_opt_ LPDWORD lpcValues,
		&maxValueLen,			//	_Out_opt_ LPDWORD lpcbMaxValueNameLen,
		NULL,			//	_Out_opt_ LPDWORD lpcbMaxValueLen,
		NULL,			//	_Out_opt_ LPDWORD lpcbSecurityDescriptor,
		NULL			//	_Out_opt_ PFILETIME lpftLastWriteTime
	);

	char valueName[1024];
	DWORD valueLen = 1024;

	for (DWORD i = 0; i < valuesNumber; ++i) {
		RegEnumValueA
		(hKeyRoot,
			i,
			valueName,
			&valueLen,
			0,
			NULL,
			NULL, //vector info extra
			NULL //dim vector anterior
		);
		printf("Index %d:\n", i);
		printf("Nume Valoare: %s\n", valueName);
		printf("Lungime Valoare:%d\n", valueLen);
		char changedValue[10000] = "";
		strcat_s(changedValue, sizeof changedValue, currentFolderTree);
		strcat_s(changedValue, sizeof changedValue, valueName);
		strcat_s(changedValue, sizeof changedValue, "\\");

		printf("Folder Tree VALUES: %s \n", changedValue);
		valueName[valueLen - 1] = '\0';

		HANDLE h = CreateFileA(
			changedValue,         // lpFileName
			GENERIC_ALL,			// dwDesiredAccess
			0,                      // dwShareMode
			NULL,                   // lpSecurityAttributes
			OPEN_ALWAYS,			// dwCreationDisposition
			FILE_ATTRIBUTE_NORMAL,  // dwFlagsAndAttributes
			INVALID_HANDLE_VALUE);  // hTemplateFile

		if (h == INVALID_HANDLE_VALUE) {
			printf("Cannot open file. Error code: %d", GetLastError());
		}

		valueLen = 1024;

		CloseHandle(h);
	}

	char keyName[1024];
	DWORD keyLen = 1024;

	for (DWORD i = 0; i < keysNumber; ++i) {
		RegEnumKeyExA
		(hKeyRoot,
			i,
			keyName,
			&keyLen,
			0,
			NULL,
			NULL, //vector info extra
			NULL //dim vector anterior
		);
		printf("Index %d:\n", i);
	
		char changedKey[10000] = "" ;
		strcat_s(changedKey, sizeof changedKey, currentFolderTree);
		strcat_s(changedKey, sizeof changedKey, keyName);
		strcat_s(changedKey, sizeof changedKey, "\\");
		printf("Folder Tree: %s \n", changedKey);

		CreateDirectoryA(
			changedKey,       // lpFileName
			NULL			  // lpSecurityAttributes
		);

		HKEY hSubKey;
		RegOpenKeyExA(
			hKeyRoot,
			keyName,
			0,
			KEY_READ,
			&hSubKey
		);
	
		searchRegistry(hSubKey, changedKey);

		RegCloseKey(hSubKey);
		keyLen = 1024;

	}

	

}

int main() {

	HKEY hKeyRoot;
	HKEY hKey;
	DWORD disposition = 0;
	DWORD error_code = 0;

	if ((error_code = RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\CSSO",
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKeyRoot,
		&disposition)) != ERROR_SUCCESS) {
		printf("Nu s-a putut creea cheia. Cod eroare: %d\n", error_code);
		return 0;
	}
	
	if ((error_code = RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\CSSO\\tema1\\un director",
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&disposition)) != ERROR_SUCCESS) {
		printf("Nu s-a putut creea cheia. Cod eroare: %d\n", error_code);
		return 0;
	}
	if ((error_code = RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\CSSO\\tema1\\alt director\\subdirector1\\subdirector2",
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&disposition)) != ERROR_SUCCESS) {
		printf("Nu s-a putut creea cheia. Cod eroare: %d\n", error_code);
		return 0;
	}
	DWORD size = 10; // dummy value
	RegSetValueEx(
		hKey,
		L"7z.exe",
		0,
		REG_DWORD,
		(const BYTE*)& size,
		sizeof(DWORD)
	);

	const char* s1 = "This is a test.";
	RegSetValueEx(
		hKey,
		L"7-zip.chm",
		0,
		REG_SZ,
		(const BYTE*)s1,
		strlen(s1)
	);

	RegSetValueEx(
		hKey,
		L"calc.exe",
		0,
		REG_SZ,
		(const BYTE*)& size,
		sizeof(DWORD)
	);

	DWORD numarChei = 0;
	DWORD numarValori = 0;

	RegQueryInfoKey(
		hKey,			//  _In_ HKEY hKey,
		NULL,			//	_Out_writes_to_opt_(*lpcchClass,*lpcchClass + 1) LPWSTR lpClass,
		NULL,			//	_Inout_opt_ LPDWORD lpcchClass,
		NULL,			//	_Reserved_ LPDWORD lpReserved,
		&numarChei,		//	_Out_opt_ LPDWORD lpcSubKeys,
		NULL,			//	_Out_opt_ LPDWORD lpcbMaxSubKeyLen,
		NULL,			//	_Out_opt_ LPDWORD lpcbMaxClassLen,
		&numarValori,	//	_Out_opt_ LPDWORD lpcValues,
		NULL,			//	_Out_opt_ LPDWORD lpcbMaxValueNameLen,
		NULL,			//	_Out_opt_ LPDWORD lpcbMaxValueLen,
		NULL,			//	_Out_opt_ LPDWORD lpcbSecurityDescriptor,
		NULL			//	_Out_opt_ PFILETIME lpftLastWriteTime
	);

	printf("Subchei: %d\n", numarChei);
	printf("Valori : %d\n", numarValori);


	char numeValoare[1024];
	DWORD lungimeNume = 1024;

	for (DWORD i = 0; i < numarValori; ++i) {
		//numeValoare[0] = '\0';
		RegEnumValueA
		(hKey,
			i,
			numeValoare,
			&lungimeNume,
			0,
			NULL,
			NULL, //vector info extra
			NULL //dim vector anterior
		);
		printf("Valoare %d:\n", i);
		printf("Nume Valoare: %s\n", numeValoare);
		printf("lungime:%d\n", lungimeNume);
		numeValoare[lungimeNume - 1] = '\0';

		lungimeNume = 1024;
	}


	if ((error_code = RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\CSSO\\tema1\\alt director\\subdirector2",
		0,
		NULL,
		REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&disposition)) != ERROR_SUCCESS) {
		printf("Nu s-a putut creea cheia. Cod eroare: %d\n", error_code);
		return 0;
	}

	char rootDirectory[10000] = "H:\\";
	searchRegistry(hKeyRoot, rootDirectory);

	RegCloseKey(hKey);
	RegCloseKey(hKeyRoot);

	return 0;
}