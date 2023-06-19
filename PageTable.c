#include "PageTable.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


PageTable_Entry** PT_Initialization(PageTable_Entry** PageTable, unsigned int No_of_entries){
    int columns = 3; // There are 3 columns in the pagetable: dirty bit, valid bit, PPN
    PageTable = malloc(No_of_entries * sizeof (PageTable_Entry*));
    for (unsigned int i = 0; i < No_of_entries; ++i){
        PageTable[i] = malloc(columns * sizeof (PageTable_Entry));
    }
    for (unsigned int i = 0; i < No_of_entries; ++i){
        for (int j = 0; j < No_of_entries; ++j){
            PageTable[i]->Validbit = 0;
        }
    }
    return PageTable;
}

void PT_destroy(PageTable_Entry** PageTable, unsigned int No_of_entries){
    // int columns = 3;
    for (unsigned int i = 0; i < No_of_entries; ++i){
        free(PageTable[i]);
    }
    free(PageTable);
}

int PT_check(PageTable_Entry** PageTable, int VPN, int* PhysicalPageNo){
    if (PageTable[VPN]->Validbit == 0){
        return -1;
    }
    *PhysicalPageNo = PageTable[VPN]->PPN;
    return 0;
}

int PT_update(PageTable_Entry** PageTable, int VPN, int PhysicalPageNo){
    if (PageTable[VPN]->Validbit == 1){
        printf("Page map entry taken\n");
        return -1;
    }
    PageTable[VPN]->Validbit = 1;
    PageTable[VPN]->PPN = PhysicalPageNo;
    return 0;
}
