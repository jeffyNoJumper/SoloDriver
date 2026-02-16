#include "../headers/process_helper.h"
#include "../headers/driver_helper.h" // We will need this to call the driver
#include <iostream>

Process::Process(const char* name)
    : name(name), pid(0)
{
    // 1. Get PID safely without Opening a Handle
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // Convert char* name to Wide String for comparison
    wchar_t wideName[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, name, -1, wideName, MAX_PATH);

    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (_wcsicmp(entry.szExeFile, wideName) == 0)
            {
                pid = entry.th32ProcessID;
                // DO NOT call OpenProcess here. 
                // DO NOT call GetModules() here.
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
}

// We redefine this to ask our stealthy driver for the base address
uintptr_t Process::GetModuleBase(const char* module_name)
{
    // This calls the "Magic" Syscall we set up in driver_helper.cpp
    ioctl::Driver driver;
    return driver.get_module_base(module_name);
}

// Simplified destructor since there is no handle to close
Process::~Process()
{
}
