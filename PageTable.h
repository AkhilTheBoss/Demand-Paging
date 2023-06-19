#ifndef PAGETABLE_H
#define PAGETABLE_H

typedef struct PageTable_Entry
{
    int Dirtybit; // if 1 then the block's memory is modified and is not saved in the secondary storage else 0
    int Validbit; // (or resident bit) if 1 then entry is valid else the entry is not valid
    int PPN; // The physical page number to the corresponding VPN
} PageTable_Entry;

PageTable_Entry** PT_Initialization(PageTable_Entry** PageTable, unsigned int No_of_entries);
void PT_destroy(PageTable_Entry** PageTable, unsigned int No_of_entries);
int PT_check(PageTable_Entry** PageTable, int VPN, int* PhysicalPageNo);
int PT_update(PageTable_Entry** PageTable, int VPN, int PhysicalPageNo);

#endif