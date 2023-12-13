#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <iostream>
using namespace std;


#define SIZE 4096

int main() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            SIZE,
            TEXT("SharedMemory"));

    if (hMapFile == NULL) {
        cout << ("Could not create file mapping object (%d).\n", GetLastError());
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile,
                                 FILE_MAP_ALL_ACCESS,
                                 0,
                                 0,
                                 SIZE);

    if (pBuf == NULL) {
        cout << ("Could not map view of file (%d).\n", GetLastError());

        CloseHandle(hMapFile);
        return 1;
    }


    strcpy((char*)pBuf, "Hello, child process!");


    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
            NULL,
            "child_process.exe",
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
    )) {
        cout << ("CreateProcess failed (%d).\n", GetLastError());

        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 1;
    }


    WaitForSingleObject(pi.hProcess, INFINITE);


    cout << ("Child reads: %s\n", pBuf);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}