  .global main
.text

main:
  # Translation of write (1, message, 14)
  mov $1, %rax        # indicates the 'write' syscall
  mov $1, %rdi        # where to write: file descriptor 1 (stdout)
  mov $message, %rsi  # address of the message
  mov $14, %rdx       # num of bytes to write
  syscall             # cause the write to happen

  # Translation of exit (0)
  mov $60, %rax       # indicates the 'exit' syscall
  mov $0, %rdi        # return code
  syscall             # exit the program

.data 
message:  .ascii "Hello, world!\n"



