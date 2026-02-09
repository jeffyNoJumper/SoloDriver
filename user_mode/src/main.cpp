#include <iostream>
#include <cstdint>
#include <map>

#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>

#include "../headers/process_helper.h"
#include "../headers/driver_helper.h"
#include "../headers/error_helper.h"
#include "../headers/offsets.h"


int main()
{
    const char* process_name = "cod.exe";

    ioctl::Driver driver;

    if (driver.driver_handle == INVALID_HANDLE_VALUE)
    {
        std::cout << "[-] Failed to create driver handle\n";
        return 1;
    }

    std::cout << "[*] Waiting for game...\n";

    DWORD pid = 0;

    // Wait for real game process

    while (!pid)
    {
        pid = Process::GetProcessIdByName(process_name);
        Sleep(1000); // don't burn CPU
    }

    std::cout << "[+] Found process PID: " << pid << "\n";

    // Attach driver
    if (!driver.attach_to_process(pid))
    {
        std::cout << "[-] Failed attaching driver\n";
        return 1;
    }

    std::cout << "[+] Attached to process\n";

    // Wait for module base
	
    uintptr_t base = 0;

    while (!base)
    {
        base = driver.get_module_base(process_name);
        Sleep(500);
    }

    std::cout << "[+] Module base: 0x" << std::hex << base << "\n";

    // Init + loop
    
    offsets::init(driver, base);

    while (true)
    {
        features::tick(driver, base);
        Sleep(10);
    }

    return 0;
}
