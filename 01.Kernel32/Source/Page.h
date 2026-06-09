#ifndef __PAGE_H__
#define __PAGE_H__

#include "Types.h"

#define PAGE_FLAGS_P 0X00000001 // Present
#define PAGE_FLAGS_RW 0X00000002 // Read/Write
#define PAGE_FLAGS_US 0X00000004 // User/Supervisor
#define PAGE_FLAGS_PWT 0X00000008 // Page-Level Write-Through
#define PAGE_FLAGS_PCD 0X00000010 // Page-Level Cache Disable
#define PAGE_FLAGS_A 0X00000020 // Accessed
#define PAGE_FLAGS_D 0X00000040 // Dirty
#define PAGE_FLAGS_PS 0X00000080 // Page Size
#define PAGE_FLAGS_G 0X00000100 // Global
#define PAGE_FLAGS_PAT 0X00001000 // Page Attribute Table
#define PAGE_FLAGS_EXB 0X00002000 // Execute Disable Bit
#define PAGE_FLAGS_DEFAULT (PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define PAGE_TABLESIZE 0x1000 // 4KB
#define PAGE_MAXENTRYCOUNT 512

#define PAGE_DEFAULTSIZE 0x200000 // 2MB


#pragma pack(push, 1)

typedef struct kPageTableEntryStruct {
    DWORD dwAttributeAndLowerBaseAddress;
    DWORD dwUpperBaseAddressAndEXB;
} PML4ENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack(pop)

void kInitializePageTables(void);
void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
     DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);

#endif /*__PAGE_H__*/


