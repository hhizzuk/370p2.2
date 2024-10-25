        lw      0       5       StackInit    ; initialize stack pointer
        lw      0       4       RecFunc      ; load recursive function
        lw      0       1       Count        ; load counter
        jalr    4       7                    ; call recursive function
        halt
StackInit    .fill   Stack
RecFunc      .fill   recur
Count        .fill   3