        lw      0       5       Start   ; initialize stack pointer
        lw      0       4       Func    ; load function address
        jalr    4       7               ; call function
        halt
Start   .fill   Stack                   ; Stack reference
Func    .fill   func   