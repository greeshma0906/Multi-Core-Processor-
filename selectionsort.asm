# Initialize variables
.data
arr:  .word 64, 25, 12, 22, 11  # Array to be sorted
n:    .word 5                    # Size of the array

.text
.globl main

main:
    # Load n into a register
    lw a0, n

    # Loop over the array
    li t0, 0          # i = 0
outer_loop:
    bge t0, a0, done_outer  # Exit loop if i >= n

    # Initialize min_index to i
    mv t1, t0         # min_index = i

    # Loop over the remaining elements to find the minimum
    addi t2, t0, 1    # j = i + 1
inner_loop:
    bge t2, a0, done_inner  # Exit loop if j >= n

    # Load arr[j] into a register
    lw t3, arr(t2)

    # Load arr[min_index] into a register
    lw t4, arr(t1)

    # Compare arr[j] with arr[min_index]
    blt t3, t4, update_min  # If arr[j] < arr[min_index], update min_index

    # Increment j
    addi t2, t2, 1
    j inner_loop

update_min:
    # Update min_index to j
    mv t1, t2

    # Increment j
    addi t2, t2, 1
    j inner_loop

done_inner:
    # Swap arr[i] with arr[min_index]
    lw t5, arr(t0)   # Load arr[i] into a register
    lw t6, arr(t1)   # Load arr[min_index] into a register
    sw t6, arr(t0)   # Store arr[min_index] at arr[i]
    sw t5, arr(t1)   # Store arr[i] at arr[min_index]

    # Increment i
    addi t0, t0, 1
    j outer_loop

done_outer:
    # Print sorted array (omitted for simplicity)

    # Exit program
    li a7, 10  # syscall exit
    ecall
