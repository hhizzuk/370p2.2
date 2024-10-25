        lw      0       1       local1  ; Local label
        lw      0       4       Next    ; Global label
        jalr    4       7
local1  .fill   42
Next    .fill   next