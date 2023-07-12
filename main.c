#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
// #include <sys/mman.h>
// #include <fcntl.h>
#include "TLB.h"
#include "PageTable.h"

// 0xFFFF -> 0000 0000 0000 0000 1111 1111 1111 1111
// 0xFF -> 0000 0000 1111 1111
// 0xFF00 -> 1111 1111 0000 0000
#define BITMASK 0xFFFF // Mask the leftmost 16 bits, only keep the rightmost 16 bits as our program deals with 16 bits
#define OFFSET_MASK 0xFF // Mask the virtual page number but not the offset bits
#define PN_MASK 0xFF00 // Mask the offset but not the virtual page number bits
#define SHIFT 8 // Shift 8 bits to extract the virtual page number
#define PT_SIZE 256 // There are 2^8 page table entries 
#define TLB_SIZE 16 // There are 16 TLB entries
#define PAGE_SIZE 256 // There are 2^8 bytes per page
#define FRAME_SIZE 256 // FRAME_SIZE = PAGE_SIZE
#define NO_FRAMES 256 // Number of frames = 2^8 = 256 frames in RAM
#define RAM_SIZE 65536 // The total RAM size is 256 frames * 256 bytes/frame = 65536 bytes 
#define SECONDARY_STORAGE_SIZE 65536 // BACKING_STORE is 65536 bytes

// This function prints the binary verion of the number
void printBinary(unsigned int num) { 
    int i;
    unsigned int size = sizeof(num) * 4; // Size of the unsigned integer in bits if it was an integer: sizeof(num) * 8

    for (i = size - 1; i >= 0; i--) {
        int bit = (num >> i) & 1; // Extract the bit at position i
        printf("%d", bit);
    }
    printf("\n");
}

// This function extracts the offset from the virtual address
int extractOffset(int virtualAddress){
    return virtualAddress & OFFSET_MASK;
}

// This function extracts the virtual page number from the virtual address
int extractVPN(int virtualAddress){
    return (virtualAddress & PN_MASK) >> SHIFT;
}

// This function convertes '32' bit addresses to '16' bit addresses
int extractAddress(int virtualAddress){
    return virtualAddress & BITMASK;
}

void Dram_Initialization(int*** Memory){
    *Memory = malloc(NO_FRAMES * sizeof(int*));
    for (unsigned int i = 0; i < NO_FRAMES; ++i){
        (*Memory)[i] = malloc(FRAME_SIZE * sizeof(int));
    }
    for (unsigned int i = 0; i < NO_FRAMES; ++i){
        for (int j = 0; j < FRAME_SIZE; ++j){
            (*Memory)[i][j] = 0;
        }
    }
    // return Memory;
}

int Dram_Store(int*** Memory, char* buffer, int *FrameNumber){
    ++*FrameNumber;
    // printf("BUFFER: %d\n", buffer[7]);
    if (*FrameNumber > 255){
        printf("RAM is full - time for a frame replacement\n");
        return -1;
    }
    for (int i = 0; i < FRAME_SIZE; ++i){
        (*Memory)[*FrameNumber][i] = buffer[i];
        // printf("BUFFER: %d\n", Memory[*FrameNumber][7]);
    }
    // printf("BUFFER: %d\n", (*Memory)[*FrameNumber][7]);
    return 0;
}

void Dram_free(int** Memory){
    for (unsigned int i = 0; i < NO_FRAMES; ++i){
        free(Memory[i]);
    }
    free(Memory);
}

int main(int argc, char* argv[]){
    char VA[10];
    FILE *fp = NULL;
    FILE* secondaryStorage = NULL;
    TLB_Detail T = {};
    int offset;
    int VPN;
    PageTable_Entry** PageTable = NULL;
    int** memory;
    int FrameNumber = -1;
    int virtualAddress;
    int TLB_Hit = 0;
    int PT_Hit = 0;
    int VM_Hit = 0;
    int PageFault_Rate = 0;
    int Total_addr = 1000;
    clock_t start, end;
    double TLB_CpuTime = 0;
    double PT_CpuTime = 0;
    double VM_CpuTime = 0;
    double CpuTime = 0;
    char HIT[25] = "";
 
    fp = fopen("VirtualAddresses.txt" , "r");
    if(fp == NULL) {
        perror("Error opening file");
        return -1;
    }
    if (feof(fp)){
        perror("End of file");
        return -2;
    }
    char buffer[PAGE_SIZE+1]; // +1 for null character

    // int fd = open(secondaryStorage, O_RDONLY);
    // Page = mmap(NULL, SECONDARY_STORAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    secondaryStorage = fopen("BACKING_STORE.bin", "rb");
    FILE* outfile = fopen( "output.txt" , "w");
    // fread(buffer, 1, 256, secondaryStorage);
    // fseek(secondaryStorage, 0, SEEK_END);
    // printf("RetVal: %ld\n", ftell(secondaryStorage));

    TLB_Initialization(&T, TLB_SIZE);
    PageTable = PT_Initialization(PageTable, PT_SIZE);
    Dram_Initialization(&memory);

    while (fgets(VA, 10, fp) != NULL){
        int PhysicalPageNo;

        // Start the timer
        start = clock();

        virtualAddress = extractAddress(atoi(VA));
        offset = extractOffset(virtualAddress);
        VPN = extractVPN(virtualAddress);
        int return_status = TLB_check(T, TLB_SIZE, VPN, &PhysicalPageNo);
        if (return_status == 0){
            // printf("TLB Hit\n");
            ++TLB_Hit;
            strcpy(HIT, "TLB");
        } 
        else{
            // printf("TLB Miss\n");
            // page map check
            return_status = PT_check(PageTable, VPN, &PhysicalPageNo);
            if (return_status == 0){
                strcpy(HIT, "Page Map");
                ++PT_Hit;
                // printf("Page Map Hit\n");
            } 
            else{
                // printf("Page Map Miss\n");

                // printf("\033[31mThis is the page fault handler\033[0m\n");
                // CALL PAGE FAULT HANDLER
                ++PageFault_Rate;
                fseek(secondaryStorage, VPN * FRAME_SIZE, SEEK_SET);
                fread(buffer, 1, FRAME_SIZE, secondaryStorage);
                return_status = Dram_Store(&memory, buffer, &FrameNumber);
                if (return_status == -1){
                    return -3;
                }
                strcpy(HIT, "Secondary Storage");
                ++VM_Hit;
                // printf("Page is succesfully transferred from disk to RAM\n");
                PhysicalPageNo = FrameNumber;
                // UPDATE PAGE MAP
                return_status = PT_update(PageTable, VPN, PhysicalPageNo);
                if (return_status == -1){
                    return -4;
                }
                // printf("Updated the Page Map\n");
                // UPDATE TLB
                return_status = TLB_update(&T, TLB_SIZE, VPN, PhysicalPageNo);
                // if (return_status == -1){
                //     return -4;
                // }
                // if (return_status == -1){
                //     printf("Unsuccessful in updating the TLB\n");
                // }
                // else {
                //     printf("Updated the TLB\n");
                // }
            }
            // if there is no entry on the page map activate the page fault handler
            // Take the memory from the disk and place it a frame of the ram, update page map and TLB
        }
        int Value = memory[FrameNumber][offset];
        // Stop the timer
        end = clock();
        CpuTime = ((double) (end - start)) / CLOCKS_PER_SEC;
        if (strcmp(HIT, "TLB") == 0){
            TLB_CpuTime += CpuTime;
        }
        if (strcmp(HIT, "Page Map") == 0){
            PT_CpuTime += CpuTime;
        }
        if (strcmp(HIT, "Secondary Storage") == 0){
            VM_CpuTime += CpuTime;
        }
        // char output[80];
        // sprintf(output, "Virtual address: %d\t\tPhysical address: %d\t\tValue: %d\n", VPN, PhysicalPageNo, Value);
        // fwrite(output , 1 , strlen(output) , fp);
        // fclose(outfile);
        fprintf(outfile, "Virtual address: %d\t\tPhysical address: %d\t\tValue: %d\t\tAccessed From: %s\n\n", virtualAddress, (PhysicalPageNo << SHIFT) | offset, Value, HIT);
    }
    fclose(fp);
    fclose(secondaryStorage);
    FILE* results = fopen( "results.txt" , "w");
    fprintf(results, "TLB Hit: %f%%\nPage Fault Rate: %f%%\nTLB Latency: %f micro secs\nPage Map Latency: %f micro secs\nSecondary Storage Latency: %f micro secs\n", ((float)TLB_Hit/Total_addr)*100, ((float)PageFault_Rate/Total_addr)*100, TLB_CpuTime * 1000000, PT_CpuTime * 1000000, VM_CpuTime* 1000000);

    // int err = munmap(Page, SECONDARY_STORAGE_SIZE);
    // if (err != 0){
    //     printf("Unmapping failed\n");
    //     return -3;
    // }
    
    T = TLB_destroy(T, TLB_SIZE);
    PT_destroy(PageTable, PT_SIZE);
    Dram_free(memory);
    
    return 0;
}
