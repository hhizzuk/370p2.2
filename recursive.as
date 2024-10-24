        lw      0       1       n           
        lw      0       4       Faddr       
        jalr    4       7                   
        halt
fib     lw      0       6       pos1        
        sw      5       7       Stack       
        add     5       6       5           
        sw      5       1       Stack       
        add     5       6       5           
        lw      0       2       neg1        
        add     1       2       4           
        beq     0       4       ret1        
        beq     0       1       ret0        
        add     1       2       1           
        lw      0       4       Faddr       
        jalr    4       7                   
        sw      5       3       Stack       
        add     5       6       5           
        lw      0       2       neg2        
        lw      5       1       Stack       
        add     1       2       1           
        lw      0       4       Faddr       
        jalr    4       7                   
        lw      0       6       neg1        
        add     5       6       5           
        lw      5       4       Stack       
        add     3       4       3           
clean   lw      0       6       neg1        
        add     5       6       5           
        lw      5       1       Stack       
        add     5       6       5           
        lw      5       7       Stack       
        jalr    7       4                   
ret0    lw      0       3       zero        
        beq     0       0       clean       
ret1    lw      0       3       pos1        
        beq     0       0       clean       
n       .fill   7
Faddr   .fill   fib
pos1    .fill   1
neg1    .fill   -1
neg2    .fill   -2
zero    .fill   0