#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <map>

class Process
{
public:
    const char* name;
    DWORD pid;
    // Removed: HANDLE handle; // Opening a handle to cod.exe is a detection vector!

    Process(const char* name);

    // Static helper to find PID using snapshots (safe)
    static DWORD GetProcessIdByName(const char* process_name) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (snapshot == INVALID_HANDLE_VALUE) return 0;

        if (Process32First(snapshot, &entry)) {
            while (Process32Next(snapshot, &entry)) {
                if (_stricmp(entry.szExeFile, process_name) == 0) {
                    CloseHandle(snapshot);
                    return entry.th32ParentProcessID; // Return the ID
                }
            }
        }

        CloseHandle(snapshot);
        return 0;
    }

    // We rely on the driver to get module bases now
    uintptr_t GetModuleBase(const char* module_name);

    ~Process();
};

#endif
