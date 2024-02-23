.data
array: .word 14 2 0 9 10 31 28 11 5 60
.text
.globl main
main:
li x0,0            
li x1,0           
li x2,0             
li x3,10            
li x4,10             
li x9,0
addi x3,x3,-1
outerloop:
li x2,0
addi x4,x4,-1      
addi x7,x0,0        
innerloop:
lw x5,0(x7)        
addi x7,x7,1        
lw x6,0(x7)         
addi x2,x2,1        
slt x8,x5,x6        
bne x8,x9,leave
swap:
sw x5,0(x7)
sw x6,-1(x7)
lw x6,0(x7)
leave:
bne x2,x4,innerloop     
addi x1,x1,1           
bne x1,x3,outerloop     
