#include <iostream>
#include <cstdint>
#include <Windows.h>

#include "../headers/process_helper.h"
#include "../headers/driver_helper.h"
#include "../headers/error_helper.h"
#include "../headers/offsets.h"

// Note: Ensure features.h is included if features::tick is used
// #include "../headers/features.h"

int main()
{
    const char* process_name = "cod.exe";
    ioctl::Driver driver;

    // 1. Check if our stealth driver is loaded and hooked
    // This replaces the INVALID_HANDLE_VALUE check
    if (!driver.initialize())
    {
        std::cout << "[-] Stealth driver not found or hook failed!\n";
        std::cout << "[!] Ensure the driver is mapped before starting.\n";
        system("pause");
        return 1;
    }

    std::cout << "[+] Communication established via stealth hook.\n";
    std::cout << "[*] Waiting for " << process_name << "...\n";

    DWORD pid = 0;

    // 2. Wait for the game process safely
    while (!pid)
    {
        pid = Process::GetProcessIdByName(process_name);
        if (pid) break;
        Sleep(1000);
    }

    std::cout << "[+] Found process PID: " << pid << "\n";

    // 3. Attach driver (Sends PID to the kernel hook)
    if (!driver.attach_to_process(pid))
    {
        std::cout << "[-] Driver failed to attach to process.\n";
        system("pause");
        return 1;
    }

    std::cout << "[+] Driver attached to " << process_name << "\n";

    // 4. Wait for module base (Driver finds this in Kernel EPROCESS)
    uintptr_t base = 0;
    while (!base)
    {
        base = driver.get_module_base(process_name);
        if (base) break;
        Sleep(500);
    }

    std::cout << "[+] Module base found: 0x" << std::hex << base << std::dec << "\n";

    // 5. Initialize offsets (Passing driver and base)
    offsets::init(driver, base);

    std::cout << "[+] Success! Starting feature loop...\n";

    // 6. Main Loop
    while (true)
    {
        // features::tick(driver, base);

        // Simple connection test for debugging:
        // int health = driver.read<int>(base + 0xoffset);

        Sleep(10);
    }

    return 0;
}
