#include <ntifs.h>

extern "C" {
    NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
    NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T bufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);

    // This function handles the IOCTLs from your user-mode error_helper.cpp
    NTSTATUS io_controller(PDEVICE_OBJECT device_obj, PIRP irp) {
        UNREFERENCED_PARAMETER(device_obj);
        
        PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
        ULONG control_code = stack->Parameters.DeviceIoControl.IoControlCode;

        // Implement your logic for codes 0x696, 0x697, etc. here
        
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }

    // Required to allow CreateFileA and CloseHandle in user-mode
    NTSTATUS create_close(PDEVICE_OBJECT device_obj, PIRP irp) {
        UNREFERENCED_PARAMETER(device_obj);
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }

    NTSTATUS driver_main(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registry_path) {
        UNREFERENCED_PARAMETER(registry_path);
        
        UNICODE_STRING dev_name, sym_link;
        RtlInitUnicodeString(&dev_name, L"\\Device\\SoloKnight");
        RtlInitUnicodeString(&sym_link, L"\\DosDevices\\SoloKnight");

        // Create the device
        IoCreateDevice(driver_obj, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &driver_obj->DeviceObject);
        IoCreateSymbolicLink(&sym_link, &dev_name);

        // Set up the "Communication" functions
        driver_obj->MajorFunction[IRP_MJ_CREATE] = create_close;
        driver_obj->MajorFunction[IRP_MJ_CLOSE] = create_close;
        driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = io_controller;

        driver_obj->Flags |= DO_BUFFERED_IO;
        driver_obj->Flags &= ~DO_DEVICE_INITIALIZING;

        DbgPrint("[+] SoloKnight: Driver Initialized.\n");
        return STATUS_SUCCESS;
    }

    NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registry_path) {
        UNREFERENCED_PARAMETER(driver_obj);
        UNREFERENCED_PARAMETER(registry_path);

        UNICODE_STRING driver_name = {};
        RtlInitUnicodeString(&driver_name, L"\\Driver\\SoloKnight");

        // Use IoCreateDriver to jump to our main logic
        return IoCreateDriver(&driver_name, &driver_main);
    }
}
