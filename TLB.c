#include "TLB.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static int headIndex = 0;
int TLB_Initialization(TLB_Detail *T, unsigned int No_of_entries){
    int columns = 2; // There are 2 columns in the TLB: VPN and PPN
    T->TLB = malloc(No_of_entries * sizeof (TLB_Entry*));
    for (unsigned int i = 0; i < No_of_entries; ++i){
        T->TLB[i] = malloc(columns * sizeof (TLB_Entry));
    }
    for (unsigned int i = 0; i < No_of_entries; ++i){
        T->TLB[i]->Validbit = 0;
    }
    T->TLB_Empty = true;
    //TLB_Entry**
    // printf("HI\n");
    
    return 0;
}

TLB_Detail TLB_destroy(TLB_Detail T, unsigned int No_of_entries){
    // int columns = 2;
    for (unsigned int i = 0; i < No_of_entries; ++i){
        free(T.TLB[i]);
    }
    free(T.TLB);
    return T;
}

int TLB_check(TLB_Detail T, unsigned int No_of_entries, int VPN, int* PhysicalPageNo){
    for (int i = 0; i < No_of_entries; ++i){
        if (T.TLB[i]->Validbit == 0){
            continue;
        }
        if (T.TLB[i]->VPN == VPN){
            *PhysicalPageNo = T.TLB[i]->PPN;
            return 0;
        }
    }
    return -1;
}

// First in - first out algorithm performed for the TLB if the TLB is full
void TLB_Fifo(TLB_Detail **T,  unsigned int No_of_entries, int VPN, int PhysicalPageNo){
    (*T)->TLB[headIndex]->VPN = VPN;
    (*T)->TLB[headIndex]->PPN = PhysicalPageNo;

    if (headIndex == No_of_entries - 1){
        headIndex = 0;
    }
    else {
        ++headIndex;
    }
}

int TLB_update(TLB_Detail *T, unsigned int No_of_entries, int VPN, int PhysicalPageNo){
    for (int i = 0; i < No_of_entries; ++i){
        if (T->TLB[i]->Validbit == 1){
            continue;
        }
        T->TLB[i]->Validbit = 1;
        T->TLB[i]->VPN = VPN;
        T->TLB[i]->PPN = PhysicalPageNo;
        return 0;
    }
    TLB_Fifo(&T, No_of_entries, VPN, PhysicalPageNo);
    return -1;
}

