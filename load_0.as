        lw      0       1       Start
        lw      0       2       End
        lw      0       3       Func
loop    jalr    3       7
        lw      0       4       One
        add     1       4       1
        beq     1       2       done
        beq     0       0       loop
done    halt
Start   .fill   0
End     .fill   5
One     .fill   1
Func    .fill   storeVal