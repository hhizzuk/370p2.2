func    sw      5       7       Stack   ; save return address
        add     5       6       5       ; increment stack pointer
        lw      5       7       Stack   ; restore return address
        jalr    7       4               ; return
