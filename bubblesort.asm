.data 
arr: .word 41 44 59 52 98 3 8 19 57 25 
.text
main:
li x2 0
li x3 10
addi x4 x3 -1
li x5 0
li x11 0
loop:
lw x8 0(x5)
lw x9 1(x5)
bgt x8 x9 swap
addi x5 x5 1
sub x11 x3 x2
addi x11 x11 -1
blt x5 x11 loop
addi x2 x2 1
addi x12 x3 -1
blt x2 x12 loop2
exit
swap:
sw x8 1(x5)
sw x9 0(x5)
addi x5 x5 1
sub x11 x3 x2
addi x11 x11 -1
blt x5 x11 loop
addi x2 x2 1
addi x12 x3 -1
blt x2 x12 loop2
exit
loop2:
li x5 0
j loop
