# Demand-Paging

I implemented the demand paging process in C. I considered VirtualAddress.txt as the input file that contains 1000 virtual addresses. The goal of the project 
is to access the byte/data requested by each virtual address. There are series of steps to be followed to access the data requested: <br />
i) Extract the virtual page number and the offset from the virtual address: <br />
&emsp; Convert the 32-bit virtual address to 16-bit by masking the upper 16 bits. Since there are 256 page table entries, the virtual page number is of 8 bits (as 2^8 = 256). Hence, there are 8 bits for the offset meaning the number of bytes per page = 256 bytes (as 2^8 = 256). <br />
ii) Check the TLB: <br />
&emsp; Check the TLB to see if the virtual address has a translated physical address. If there is no entry of the virtual address, it is called a TLB Miss 
and the page map is checked. On the other hand, if there is an entry present for the virtual address, the transalted physical address is taken and the 
RAM will be accessed 
