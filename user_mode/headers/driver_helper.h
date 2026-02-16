#pragma once
#include <Windows.h>
#include <iostream>

// We no longer need a HANDLE to a device!
// extern HANDLE driver_handle; 

namespace ioctl
{
    // The "Magic" code that tells the hooked function to run our cheat logic
    #define MAGIC_CODE 0xDEADBEEF69

    #pragma pack(push, 1)
    struct driver_request
    {
        uint32_t process_id;
        PVOID target;
        PVOID buffer;
        SIZE_T size;
        uint32_t instruction; // 0 for attach, 1 for read, 2 for write
    };
    #pragma pack(pop)

    // This points to the Windows function we hijacked (e.g., NtUserCopyAcceleratorTable)
    typedef __int64(NTAPI* t_hooked_func)(uintptr_t magic, void* request);

    template <typename T>
    T read(uintptr_t address)
    {
        T buffer{ };
        driver_request r;
        r.target = reinterpret_cast<PVOID>(address);
        r.buffer = &buffer;
        r.size = sizeof(T);
        r.instruction = 1; // Read code

        // Get the function pointer from win32u.dll
        static auto nt_func = (t_hooked_func)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserCopyAcceleratorTable");
        
        nt_func(MAGIC_CODE, &r);
        return buffer;
    }

    template <typename T>
    void write(uintptr_t address, T value)
    {
        driver_request r;
        r.target = reinterpret_cast<PVOID>(address);
        r.buffer = &value;
        r.size = sizeof(T);
        r.instruction = 2; // Write code

        static auto nt_func = (t_hooked_func)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserCopyAcceleratorTable");
        
        nt_func(MAGIC_CODE, &r);
    }

    inline bool attach(uint32_t pid)
    {
        driver_request r;
        r.process_id = pid;
        r.instruction = 0; // Attach code

        static auto nt_func = (t_hooked_func)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserCopyAcceleratorTable");
        
        // If the driver is loaded and hooked, it returns 0 (or your custom success code)
        return nt_func(MAGIC_CODE, &r) == 0;
    }
}
