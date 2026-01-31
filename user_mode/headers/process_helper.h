#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>
#include <string>
#include <map>

class Process
{
public:
    const char* name;         // Name of the process
    DWORD pid;                // Process ID
    HANDLE handle;            // Handle to the process
    std::map<std::string, uintptr_t> modules; // Map to store modules and their base addresses

public:
    // Get Process by name
    Process(const char* name);

    // Function to get all loaded modules of the process
    bool GetModules();

    // Prints all the Modules that are in the "modules" map, with addresses optionally
    void PrintAllModules(bool print_addresses=false);

    // Close handle if on object Destruction.
    ~Process();
};

#endif // PROCESS_H