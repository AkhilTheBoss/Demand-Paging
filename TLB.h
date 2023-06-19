#ifndef TLB_H
#define TLB_H

#include <stdbool.h>

typedef struct TLB_Entry{
    int Validbit; // (or resident bit) if 1 then entry is valid else the entry is not valid
    int VPN; // Virtual Page Number
    int PPN; // Physical Page Number
    char* h;
} TLB_Entry;

typedef struct TLB_Detail{
    int TLB_Miss_Count;
    int TLB_Hit_Count;
    int TotalCount;
    bool TLB_Empty; // If True then empty else it is not empty
    TLB_Entry **TLB;
} TLB_Detail;

int TLB_Initialization(TLB_Detail *T, unsigned int No_of_entries);
TLB_Detail TLB_destroy(TLB_Detail T, unsigned int No_of_entries);
int TLB_check(TLB_Detail T, unsigned int No_of_entries, int VPN, int* PhysicalPageNo);
int TLB_update(TLB_Detail *T, unsigned int No_of_entries, int VPN, int PhysicalPageNo);
void TLB_Fifo(TLB_Detail **T,  unsigned int No_of_entries, int VPN, int PhysicalPageNo);

#endif