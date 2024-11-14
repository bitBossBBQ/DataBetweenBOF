#include <Windows.h>
//#include "base\helpers.h"

#ifdef __cplusplus
#define DFR(module, function) \
    DECLSPEC_IMPORT decltype(function) module##$##function;

#define DFR_LOCAL(module, function) \
    DECLSPEC_IMPORT decltype(function) module##$##function; \
    decltype(module##$##function) *##function = module##$##function;
#endif // end of __cplusplus

extern "C" {
#include "beacon.h"
    DFR(KERNEL32, GetLastError);
    DFR(KERNEL32, GetProcessHeap);
    DFR(KERNEL32, HeapAlloc);
    DFR(KERNEL32, HeapFree);
    DFR(NTDLL, memcpy);
#define GetLastError KERNEL32$GetLastError
#define GetProcessHeap KERNEL32$GetProcessHeap
#define HeapAlloc KERNEL32$HeapAlloc
#define HeapFree KERNEL32$HeapFree
#define memcpy NTDLL$memcpy

    void setCounterH() {
        HANDLE hHeap = GetProcessHeap();
        LPVOID pCounter;
        int counter = 0;

        if (hHeap == INVALID_HANDLE_VALUE) {
            BeaconPrintf(CALLBACK_ERROR, "Failed to get heap handle. Error:  %d", GetLastError());
            return;
        }
        pCounter = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 4);
        memcpy(pCounter, &counter, 4);

        BeaconAddValue("pCounter", pCounter);
        BeaconPrintf(CALLBACK_OUTPUT, "Counter: %d", counter);
        BeaconCloseHandle(hHeap);
    }

    void setCounter() {
        LPVOID pCounter;
        int counter = 0;

        pCounter = BeaconVirtualAlloc(NULL, 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        memcpy(pCounter, &counter, 4);

        BeaconAddValue("pCounter", pCounter);
        BeaconPrintf(CALLBACK_OUTPUT, "Counter: %d", counter);
    }

    void incrementCounter() {
        int temp = 0;
        LPVOID pCounter = BeaconGetValue("pCounter");

        if (pCounter != NULL) {
            BeaconPrintf(CALLBACK_OUTPUT, "Dereferenced: %d", *(PINT)pCounter);
            temp = *(PINT)pCounter;
            temp++;
            memcpy(pCounter, &temp, 4);
            BeaconPrintf(CALLBACK_OUTPUT, "Counter: %d", temp);
            BeaconPrintf(CALLBACK_OUTPUT, "Dereferenced: %d", *(PINT)pCounter);
        }
        else {
            BeaconPrintf(CALLBACK_ERROR, "Failed to retrieve counter.");
        }

    }

    void cleanCounterH() {
        LPVOID pCounter = BeaconGetValue("pCounter");
        HANDLE hHeap = GetProcessHeap();

        if (pCounter != NULL) {
            if (!HeapFree(hHeap, 0, pCounter)) {
                BeaconPrintf(CALLBACK_ERROR, "Failed to free memory.");
                return;
            }
            BeaconRemoveValue("pCounter");
        }
        else {
            BeaconPrintf(CALLBACK_ERROR, "Failed to retrieve counter.");
        }
        BeaconCloseHandle(hHeap);
    }

    void cleanCounter() {
        LPVOID pCounter = BeaconGetValue("pCounter");

        if (pCounter != NULL) {
            BeaconVirtualFree(pCounter, 0, MEM_RELEASE);
            BeaconRemoveValue("pCounter");
        }
        else {
            BeaconPrintf(CALLBACK_ERROR, "Failed to retrieve counter.");
        }
    }

    void go(char* args, int len) {
        setCounter();
        incrementCounter();
        incrementCounter();
        incrementCounter();
        incrementCounter();
        incrementCounter();
        cleanCounter();

        setCounterH();
        incrementCounter();
        incrementCounter();
        incrementCounter();
        cleanCounterH();
    }
