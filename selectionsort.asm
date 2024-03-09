.data
arr: .word 4 2 3 1 5 6
.text
addi x19 x0 0
addi x18 x0 0
addi x7 x0 0
addi x2 x0 0 
addi x3 x0 0 
addi x12 x0 6
addi x11 x0 0
loop1:
bge x2 x12 exit
addi x3 x2 0  
addi x20 x19 1
addi x21 x19 0
addi x4 x2 1
loop2:
bge x4 x12 loope
lw x8 0(x20)
lw x9 0(x21)
blt x8 x9 min_ind
addi x20 x20 1
addi x4 x4 1
j loop2
loope:    
bne x3 x2 swap 
addi x19 x19 1
addi x2 x2 1
j loop1  
min_ind:
addi x3 x4 0
addi x21 x20 0
addi x20 x20 1
addi x4 x4 1
j loop2  
swap:
lw x30 0(x21)
lw x31 0(x19)
sw x30 0(x19)
sw x31 0(x21) 
addi x19 x19 1
addi x2 x2 1
j loop1 
exit:
