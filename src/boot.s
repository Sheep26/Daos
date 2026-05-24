.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set VIDEO,    1<<2

.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set MAGIC,    0xE85250D6
.set ARCH,     0

.section .multiboot,"a",@progbits
.align 8

.long MAGIC
.long ARCH
.long header_end - header_start
.long -(MAGIC + ARCH + (header_end - header_start))

header_start:

/* --- Tag: framebuffer request --- */
.align 8
.short 5              /* MULTIBOOT_TAG_TYPE_FRAMEBUFFER_REQUEST */
.short 0              /* flags */
.long 20              /* size */
.long 640             /* width */
.long 480             /* height */
.long 32              /* depth */

/* --- End tag (required) --- */
.align 8
.short 0              /* type = end */
.short 0              /* flags */
.long 8               /* size */

header_end:

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp

	push %ebx
    push %eax

	call kernel_main

	cli
1:	hlt
	jmp 1b

.size _start, . - _start
