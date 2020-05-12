	.file	"Carrays.c"
	.text
	.comm	nums,80,32
	.comm	grades,240,32
	.section	.rodata
.LC0:
	.string	"nums[%2d]= %ld\n"
.LC2:
	.string	"grades[%2d][%2d]= %4.2f  "
.LC3:
	.string	""
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$0, -8(%rbp)
	jmp	.L2
.L3:
	movl	-8(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	nums(%rip), %rax
	movq	(%rdx,%rax), %rdx
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -8(%rbp)
.L2:
	cmpl	$9, -8(%rbp)
	jle	.L3
	movl	$0, -8(%rbp)
	jmp	.L4
.L7:
	movl	$0, -4(%rbp)
	jmp	.L5
.L6:
	cvtsi2sd	-8(%rbp), %xmm1
	cvtsi2sd	-4(%rbp), %xmm0
	movsd	.LC1(%rip), %xmm2
	divsd	%xmm2, %xmm0
	addsd	%xmm1, %xmm0
	movl	-4(%rbp), %eax
	movslq	%eax, %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,8), %rdx
	leaq	grades(%rip), %rax
	movsd	%xmm0, (%rdx,%rax)
	movl	-4(%rbp), %eax
	movslq	%eax, %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,8), %rdx
	leaq	grades(%rip), %rax
	movq	(%rdx,%rax), %rcx
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movq	%rcx, -24(%rbp)
	movsd	-24(%rbp), %xmm0
	movl	%eax, %esi
	leaq	.LC2(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	addl	$1, -4(%rbp)
.L5:
	cmpl	$5, -4(%rbp)
	jle	.L6
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
	addl	$1, -8(%rbp)
.L4:
	cmpl	$4, -8(%rbp)
	jle	.L7
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC1:
	.long	0
	.long	1079574528
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
