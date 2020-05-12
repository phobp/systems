  .global main
  .text
main:
  pushq %rbx

  movq $1, %rbx
  incb arr(, %rbx, 8)
  movq arr(, %rbx, 8) , %r10
  incq %rbx
  addq arr(, %rbx, 8) , %r10
  decq %rbx
  movq %r10,arr(, %rbx, 8) 
  salq $4, arr(, %rbx, 8)
  sarq $1, arr(, %rbx, 8)
  movq arr(, %rbx, 8) , %rax

  popq %rbx
  ret

  .data
arr: 
  .quad 0x30
  .quad 0x2
  .quad 0x5
  .quad 0x6
  .quad 0xe
  .quad 0xf


