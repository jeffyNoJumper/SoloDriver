#include "../headers/driver_helper.h"

namespace ioctl
{
    // We no longer use CTL_CODEs or handles. 
    // We use a "Magic" number and an Instruction ID.

    // This is the function signature for the Windows syscall we hijacked
    typedef __int64(NTAPI* t_hooked_func)(uintptr_t magic, void* request);

    bool Driver::initialize()
    {
        // We don't "open" the driver anymore. 
        // We just check if our hook is alive by sending a test 'attach' to PID 0.
        driver_request r = { 0 };
        r.instruction = 99; // A special "Ping" code

        auto nt_func = (t_hooked_func)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserCopyAcceleratorTable");
        
        if (!nt_func) return false;

        // If the driver is there, it will catch the MAGIC_CODE and return a specific value
        return nt_func(MAGIC_CODE, &r) == 0xACE; 
    }

    bool Driver::attach_to_process(uint32_t pid)
    {
        driver_request r;
        r.process_id = pid;
        r.instruction = 0; // 0 = ATCH

        auto nt_func = (t_hooked_func)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserCopyAcceleratorTable");
        return nt_func(MAGIC_CODE, &r) == 0;
    }

    uintptr_t Driver::get_module_base(const char* module_name)
    {
        driver_request r;
        r.instruction = 3; // 3 = BASE
        // We pass the module name in the target field (or use a separate buffer)
        r.target = (PVOID)module_name; 

        auto nt_func = (t_hooked_func)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserCopyAcceleratorTable");
        nt_func(MAGIC_CODE, &r);
        
        return (uintptr_t)r.buffer; // Driver writes the base address back into r.buffer
    }
}
