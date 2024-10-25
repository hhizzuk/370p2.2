        lw      0       1       Input     ; load input value
        lw      0       4       SubAdr    ; load function address
        jalr    4       7               ; call subroutine
        halt
Input   .fill   5
SubAdr  .fill   sub1              ; global reference to function in test1_1.as
