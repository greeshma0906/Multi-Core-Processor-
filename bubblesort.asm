.data
arr: .word 5 1 3 
.text
main:
addi x2 x0 6
addi x12 x0 7
add x4 x12 x0
addi x5 x0 9
addi x5 x5 -1
loop:
lw x6 0(x2)
lw x7 1(x2)
bgt x6 x7 swap 
addi x2 x2 1
blt x2 x5 loop
j loop1
swap:
sw x6 1(x2)  
sw x7 0(x2) 
addi x2 x2 1
blt x2 x5 loop 
j loop1
loop1:
addi x2 x2 1
blt x2 x4 loop1
exit
loop1
addi x2 x0 6
j loop
