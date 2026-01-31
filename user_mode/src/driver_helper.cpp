#include "../headers/driver_helper.h"

// Define the handle that the header's 'extern' refers to
HANDLE driver_handle = nullptr;

namespace ioctl
{
    namespace codes
    {
        ULONG attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        ULONG read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        ULONG write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    }

    // Call this first to connect to your SoloKnight driver
    bool initialize()
    {
        // Must match the SymLink L"\\DosDevices\\SoloKnight" in your main.cpp
        driver_handle = CreateFileA("\\\\.\\SoloKnight", 
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, 
            nullptr, 
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL, 
            nullptr);

        if (driver_handle == INVALID_HANDLE_VALUE) {
            return false;
        }
        return true;
    }

    // Definition for the attach logic
    bool attach_to_process(uint32_t pid)
    {
        driver_request r;
        r.process_id = pid;

        // Sends the request to the kernel's IRP_MJ_DEVICE_CONTROL handler
        return DeviceIoControl(
            driver_handle, 
            codes::attach, 
            &r, sizeof(r), 
            &r, sizeof(r), 
            nullptr, 
            nullptr
        );
    }
}
