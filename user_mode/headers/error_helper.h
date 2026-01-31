#pragma once

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <Windows.h>

// Convert Error code Into String
std::string GetErrorString(DWORD errorCode);