        lw      0       1       n           ; load n into r1
        lw      0       4       Faddr       ; load function address
        jalr    4       7                   ; call fibonacci
        halt
fib     lw      0       6       pos1        ; r6 = 1 for stack operations
        sw      5       7       Stack       ; save return address
        add     5       6       5           ; increment stack pointer
        sw      5       1       Stack       ; save n
        add     5       6       5           ; increment stack pointer
        beq     0       1       ret0        ; if n=0, return 0
        lw      0       4       pos1        ; load 1 into r4
        beq     1       4       ret1        ; if n=1, return 1
        lw      0       6       neg1        ; load -1
        add     1       6       1           ; n = n-1
        lw      0       4       Faddr       ; prepare for recursive call
        jalr    4       7                   ; call fibonacci(n-1)
        sw      5       3       Stack       ; save result of fib(n-1)
        add     5       6       5           ; increment stack pointer
        lw      5       1       Stack       ; restore original n
        lw      0       6       neg2        ; load -2
        add     1       6       1           ; n = n-2
        lw      0       4       Faddr       ; prepare for recursive call
        jalr    4       7                   ; call fibonacci(n-2)
        lw      0       6       neg1        ; prepare to pop stack
        add     5       6       5           ; decrement stack pointer
        lw      5       4       Stack       ; load fib(n-1) result
        add     3       4       3           ; add fib(n-1) + fib(n-2)
clean   lw      0       6       neg1        ; load -1
        add     5       6       5           ; decrement stack pointer
        lw      5       1       Stack       ; restore original n
        add     5       6       5           ; decrement stack pointer
        lw      5       7       Stack       ; restore return address
        jalr    7       4                   ; return
ret0    lw      0       3       zero        ; return 0
        beq     0       0       clean       ; goto cleanup
ret1    lw      0       3       pos1        ; return 1
        beq     0       0       clean       ; goto cleanup
n       .fill   7
Faddr   .fill   fib
pos1    .fill   1
neg1    .fill   -1
neg2    .fill   -2
zero    .fill   0