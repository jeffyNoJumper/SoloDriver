#include "../headers/process_helper.h"
#include <iostream>
#include <tlhelp32.h>

Process::Process(const char* name)
    : name(name), pid(0), handle(INVALID_HANDLE_VALUE)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    wchar_t wideName[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, name, -1, wideName, MAX_PATH);

    // Start looping through processes
    if (Process32First(snapshot, &entry))
    {
        do
        {

            if (_wcsicmp(entry.szExeFile, wideName) == 0)
            {
                pid = entry.th32ProcessID;

                handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                this->GetModules(); // Load Modules into map
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
}


bool Process::GetModules()
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    // Create Snapshot
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);

    // Loop through modules
    if (Module32First(snapshot, &entry))
    {
        do
        {
            char moduleName[256];
            WideCharToMultiByte(CP_ACP, 0, entry.szModule, -1, moduleName, sizeof(moduleName), NULL, NULL);

            modules[moduleName] = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
        } while (Module32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return true;
}


void Process::PrintAllModules(bool print_addresses)
{
    for (const auto& [moduleName, baseAddr] : modules)
    {
        std::cout << "Module: " << moduleName;

        if (print_addresses)
        {
            std::cout << " | Base Address: 0x" << std::hex << baseAddr << std::dec;
        }

        std::cout << '\n';
    }
}


Process::~Process()
{
    if (handle != nullptr)
    {
        CloseHandle(handle);
    }
}