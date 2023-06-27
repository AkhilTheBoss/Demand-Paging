# Demand-Paging

I implemented the demand paging process in C. I considered VirtualAddress.txt as the input file that contains 1000 virtual addresses. The goal of the project 
is to access the byte/data requested by each virtual address. There are series of steps to be followed to access the data requested: <br />
i) Extract the virtual page number and the offset from the virtual address: <br />
&emsp; Convert the 32-bit virtual address to 16-bit by masking the upper 16 bits. Since there are 256 page table entries, the virtual page number is of 8 bits (as 2^8 = 256). Hence, there are 8 bits for the offset meaning the number of bytes per page = 256 bytes (as 2^8 = 256). <br />
ii) Check the TLB(Translation Lookaside Buffer): <br />
&emsp; Check the TLB to see if the virtual address has a translated physical address. If there is no entry of the virtual address, it is called a TLB Miss and the page map is checked. On the other hand, if there is an entry present for the virtual address, it is a TLB hit and the translated physical address is taken and the RAM will be accessed. <br />
iii) Check the Page Map: <br />
&emsp; If there was a TLB miss, the page map will be checked to see if there is a mapping for the given virtual address. If it is the page map hit the data is accessed from the RAM with that physical address else the page fault handler is called. <br />
iv) Page Fault Handler: <br />
&emsp; When the page fault handler is called, the secondary storage is accessed and the respective page from the secondary storage to the RAM. The data is then accessed from the RAM and the page map and the TLB are updated. <br />
