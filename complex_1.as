recur   lw      0       6       Pos1         ; for stack operations
        sw      5       7       Stack        ; save return address
        add     5       6       5            ; increment stack pointer
        sw      5       1       Stack        ; save counter
        add     5       6       5            ; increment stack pointer
        beq     1       0       done         ; base case
        lw      0       2       Neg1         
        add     1       2       1            ; decrement counter
        jalr    4       7                    ; recursive call
done    lw      0       2       Neg1         
        add     5       2       5            ; restore stack
        lw      5       1       Stack        ; restore counter
        add     5       2       5            ; restore stack
        lw      5       7       Stack        ; restore return address
        jalr    7       4                    ; return
Pos1    .fill   1
Neg1    .fill   -1