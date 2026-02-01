#include "offsets.h"
#include "../header/driver_helper.h"

namespace offsets
{
    // storage for externs
    uintptr_t health     = 0;
    uintptr_t ammo       = 0;
    uintptr_t playerBase = 0;


    void init(ioctl::Driver& driver, uintptr_t moduleBase)
    {
        // =========================================
        // OPTION 1 — hardcoded (simple / testing)
        // =========================================

        health     = 0x280;
        ammo       = 0x150C;
        playerBase = 0x08;


        // =========================================
        // OPTION 2 — pattern scanning (future proof)
        // =========================================
        // Uncomment when you add a scanner
        /*
        health     = pattern_scan(driver, moduleBase, "48 8B ?? ?? ?? ?? 89");
        ammo       = pattern_scan(driver, moduleBase, "89 83 ?? ?? ?? ?? 8B");
        playerBase = pattern_scan(driver, moduleBase, "48 8B 05 ?? ?? ?? ?? 48 85 C0");
        */

        printf("[+] Offsets initialized\n");
    }
}