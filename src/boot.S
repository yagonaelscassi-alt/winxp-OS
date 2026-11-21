/* Cabecera Multiboot 1 con pedido de framebuffer lineal 800x600x32 */
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set VIDEO,    1<<2
.set MAGIC,    0x1BADB002
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long 0          /* modo grafico */
.long 800
.long 600
.long 32

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp
    push %ebx        /* puntero a multiboot_info */
    push %eax        /* magic */
    call kernel_main
    cli
1:  hlt
    jmp 1b
