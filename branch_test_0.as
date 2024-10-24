lw 0 1 target   # Load target address from label 'target'
beq 0 1 done    # Branch to 'done' if 0 == 1
halt            # Stop program
target .fill 10
