#include "../headers/error_helper.h"

std::string GetErrorString(DWORD errorCode)
{
    char* errorMsg = nullptr;

    DWORD dwSize = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,         // Source is SYSTEM
        errorCode,      // The error code
        0,               // Language ID (0 means use default)
        (LPSTR)&errorMsg, // Output string
        0,               // Size of buffer
        nullptr);        // Arguments (none)

    if (dwSize == 0) {
        return "Unknown error";
    }

    std::string result(errorMsg);

    LocalFree(errorMsg);

    return result;
}