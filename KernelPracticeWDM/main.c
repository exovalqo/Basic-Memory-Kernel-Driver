#include "Memory.h"

#define IOCTL_ReadMemory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WriteMemory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

UNICODE_STRING devName;
UNICODE_STRING symName;
PDEVICE_OBJECT pDevObject;

NTSTATUS DispatchCreateOrClose(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS DispatchCustom(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	ULONG information = 0; //Remember this is needed for I/O Manager to copy this amount of bytes back
	UNREFERENCED_PARAMETER(pDeviceObject);
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG code = pStack->Parameters.DeviceIoControl.IoControlCode;

	if (code == IOCTL_ReadMemory)
	{
		PPACKET_REQUEST pReq = (PPACKET_REQUEST)pIrp->AssociatedIrp.SystemBuffer;
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(pReq->procId, &process)))
			if (NT_SUCCESS(KeRead(process, pReq->destAddr, &pReq->value, sizeof(pReq->value))))
			{
				information = sizeof(*pReq);
				DbgPrint("Successfully Read Memory!");
			}
	}
	else if (code == IOCTL_WriteMemory)
	{
		PPACKET_REQUEST pReq = (PPACKET_REQUEST)pIrp->AssociatedIrp.SystemBuffer;
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(pReq->procId, &process)))
			if (NT_SUCCESS(KeWrite(process, &pReq->value, pReq->destAddr, sizeof(pReq->value))))
			{
				DbgPrint("Successfully Wrote to Memory!");
			}
	}
	
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = information;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS Unload(PDRIVER_OBJECT pDriverObject)
{
	IoDeleteDevice(pDevObject);
	IoDeleteSymbolicLink(&symName);
	DbgPrint("Unloaded Driver");
}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;
	//Create Strings
	RtlInitUnicodeString(&devName, L"\\Device\\Exovalqo");
	RtlInitUnicodeString(&symName, L"\\??\\Exovalqo");

	//Create Device
	IoCreateDevice(pDriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDevObject);
	
	//Create Symbolic Link
	IoCreateSymbolicLink(&symName, &devName);

	//Set MajorFunctions
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateOrClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateOrClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchCustom;

	
	return STATUS_SUCCESS;
}

