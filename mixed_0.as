        lw      0       1       DataStart
        lw      0       2       DataEnd
loop    add     1       3       3            ; sum data
        beq     1       2       done
        lw      0       4       Inc
        add     1       4       1
        beq     0       0       loop
done    halt
Inc     .fill   1
DataStart    .fill   data1
DataEnd      .fill   dataEnd