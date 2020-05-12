.section .text
.global main

main:
  # printf("Enter two numbers --> ");
  mov $prompt1, %rdi
  xor %rax, %rax        # Must zero %rax before calling printf
  call printf

  # scanf ("%d %d", &i , &j);
  mov $format1, %rdi
  mov $i, %rsi          # $i is the memory adddress of the data 
  mov $j, %rdx
  xor %rax, %rax        # Must zero %rax before calling scanf
  call scanf

  # printf ("i=%d ,  j=%d\n", i , j);
  mov $format2, %rdi
  mov i, %rsi           # %rsi = Mem[i] 
  mov j, %rdx           # %rdx = Mem[j]
  xor %rax, %rax        # Must zero %rax before calling printf
  call printf

  ret
#---------------------------------------------
.section .data

# The value of each symbol is its memory address
i:  .quad 0
j:  .quad 0
prompt1:  .asciz "Enter two numbers --> "
format1:  .asciz "%d %d"
format2:  .asciz "You entered i=%d , j=%d\n"


