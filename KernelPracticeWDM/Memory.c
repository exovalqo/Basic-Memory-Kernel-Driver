#include "Memory.h"

NTSTATUS KeRead(PEPROCESS process, PVOID source, PVOID dest, SIZE_T size)
{
	SIZE_T bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(process, source, PsGetCurrentProcess(), dest, size, KernelMode, &bytes)))
		return STATUS_SUCCESS;
	return STATUS_ACCESS_DENIED;
}
NTSTATUS KeWrite(PEPROCESS process, PVOID source, PVOID dest, SIZE_T size)
{
	SIZE_T bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), source, process, dest, size, KernelMode, &bytes)))
		return STATUS_SUCCESS;
	return STATUS_ACCESS_DENIED;
}