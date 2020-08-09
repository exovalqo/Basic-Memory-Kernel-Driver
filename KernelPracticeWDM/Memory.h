#pragma once
#include "ntos.h"

typedef struct PACKETREQUEST
{
	ULONG procId;
	ULONG destAddr;
	ULONG value;

}PACKET_REQUEST, *PPACKET_REQUEST;

NTSTATUS KeRead(PEPROCESS process, PVOID source, PVOID dest, SIZE_T size);
NTSTATUS KeWrite(PEPROCESS process, PVOID source, PVOID dest, SIZE_T size);