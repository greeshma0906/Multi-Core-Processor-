
.data
arr:  .word 64, 25, 12, 22, 11  # Array to be sorted
n:  .word 5                    # Size of array
.text
.globl main
main:
    lw x1, n  # Load n into a register
    li x2, 0          # i = 0
outer_loop:
    bge x2,x1 , done_outer  # Exit loop if i >= n

    # Initialize min_index to i
    mv x3, x2         # min_index = i

    # Loop over the remaining elements to find the minimum
    addi x4, x2, 1    # j = i + 1
inner_loop:
    bge x4, x1, done_inner  # Exit loop if j >= n

    # Load arr[j] into a register
    lw x5, arr(x4)

    # Load arr[min_index] into a register
    lw x6, arr(x3)

    # Compare arr[j] with arr[min_index]
    blt x5, x6, update_min  # If arr[j] < arr[min_index], update min_index

    # Increment j
    addi x4, x4, 1
    j inner_loop

update_min:
    # Update min_index to j
    mv x3, x4

    # Increment j
    addi x4, x4, 1
    j inner_loop

done_inner:
    # Swap arr[i] with arr[min_index]
    lw x7, arr(x2)   # Load arr[i] into a register
    lw x8, arr(x3)   # Load arr[min_index] into a register
    sw x8, arr(x2)   # Store arr[min_index] at arr[i]
    sw x7, arr(x3)   # Store arr[i] at arr[min_index]

    # Increment i
    addi x2, x2, 1
    j outer_loop

done_outer:
    li a7, 10  # syscall exit
    ecall
