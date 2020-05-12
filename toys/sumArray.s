     .global  main
main:
    leaq    array,  %r8
    movq    $0,     %rax    # result
    movq    $0,     %r9     # i

loop:
    cmpq    size, %r9     # i ? size
    jge     done
    addq    (%r8, %r9, 8), %rax
    incq    %r9
    jmp     loop

done:
    ret        

    .data
size:   .quad 4
array:  .quad 10
        .quad 20
        .quad 30
        .quad 40

       
