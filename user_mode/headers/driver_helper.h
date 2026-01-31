#pragma once
#include <Windows.h>
#include <iostream>

// The shared handle for communication
extern HANDLE driver_handle;

namespace ioctl
{
    namespace codes
    {
        // Definitions must exist in driver_helper.cpp
        extern ULONG attach;
        extern ULONG read;
        extern ULONG write;
    }

    // Force 1-byte alignment so Kernel and User mode see the struct the same way
    #pragma pack(push, 1)
    struct driver_request
    {
        uint32_t process_id;
        PVOID target;
        PVOID buffer;
        SIZE_T size;
    };
    #pragma pack(pop)

    // Template for Reading Memory
    template <typename T>
    T read(uintptr_t address)
    {
        T buffer{ };
        driver_request r;
        r.target = reinterpret_cast<PVOID>(address);
        r.buffer = &buffer;
        r.size = sizeof(T);

        DeviceIoControl(driver_handle, codes::read, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
        return buffer;
    }

    // Template for Writing Memory
    template <typename T>
    void write(uintptr_t address, T value)
    {
        driver_request r;
        r.target = reinterpret_cast<PVOID>(address);
        r.buffer = &value;
        r.size = sizeof(T);

        DeviceIoControl(driver_handle, codes::write, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
    }

    // Function to attach to a process
    inline bool attach(uint32_t pid)
    {
        driver_request r;
        r.process_id = pid;

        return DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
    }
}
