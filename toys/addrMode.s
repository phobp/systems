  .global main
  .text

main:
  pushq %rbx

  mov   $message, %rdi
  xor   %rax, %rax
  call  printf
  
#  movq $35, %rax 

  movq $0, %rbx
  movq arr(, %rbx, 8) , %rax

  popq %rbx
  ret

  .data

message: .asciz "Hello World!\n"
arr:     .quad 10, 20, 30, 40 , 50 , 60


