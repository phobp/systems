	.file	"asm.c"
	.text
	.globl	mult
	.type	mult, @function
mult:
.LFB0:
	.cfi_startproc
	movl	%edi, %eax
	imull	%esi, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	mult, .-mult
	.globl	mult2
	.type	mult2, @function
mult2:
.LFB1:
	.cfi_startproc
#APP
# 7 "asm.c" 1
	mov %esi, %eax
# 0 "" 2
# 8 "asm.c" 1
	imull %edi, %eax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE1:
	.size	mult2, .-mult2
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	movl	$4, %esi
	movl	$3, %edi
	call	mult
	rep ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
