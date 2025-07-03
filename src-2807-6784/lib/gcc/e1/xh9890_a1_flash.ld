/*
# MODULE:         scatter_map_gcc.txt
# DESCRIPTION:    ARM scatter-loading map file for xh98xx
# CREATION DATE:
#
# LICENSE:
#     This source code is copyright (C) 2014  Technologies
#     All rights reserved.
#
# SOURCE CONTROL:
#
# REVISION HISTORY:
*/

/* Define memory regions. */
MEMORY
{
    flash (rx)   : ORIGIN = 0x00403000, LENGTH = 512K
    ramtext (rx) : ORIGIN = 0x00200000, LENGTH = 38800
    ram (rw)     : ORIGIN = 0x20000000, LENGTH = 38800
}

/* Define the entry point of the output file. */
ENTRY(reset_handler)
_system_stack_size = 0x800;
_system_heap_size  = 0x300;

/* Define sections. */
SECTIONS
{
    /* Vector table has two body*/
    .vectors : {
        _vectors = .;
        KEEP(*(.vectors))
        . = ALIGN(4);
        _evectors = .;
    } > ramtext AT > flash
    _vectors_loadaddr = LOADADDR(.vectors);

    .ramtext : {
        . = ALIGN(4);
        _ramtext = .;
        *main.o (.text* .rodata*)
        *.o (.ramtext*)
        *sf.o(.text* .rodata*)
        *sf_sys.o(.text* .rodata*)
        *sf_base.o(.text* .rodata*)
        *co_delay.o(.text* .rodata*)
        . = ALIGN(4);
        _eramtext = .;
    } > ramtext AT > flash
    _ramtext_loadaddr = LOADADDR(.ramtext);

    .text : {
        *(.text*)    /* Program code */
        . = ALIGN(4);
        *(.rodata*)  /* Read-only data */
        . = ALIGN(4);
        _etext = .;
    } > flash

    _stack_vma = ORIGIN(ram) + ADDR(.ramtext) - ORIGIN(ramtext) + SIZEOF(.ramtext);
    .stack _stack_vma : {
        . = ALIGN(4);
        _stack = .;
        . = . + _system_stack_size;
        . = ALIGN(4);
        _estack = .;
    } > ram

    .heap : {
        . = ALIGN(4);
        __heap_base = .;
        co_heap_base = .;
        . = . + _system_heap_size;
        . = ALIGN(4);
        __heap_limit = .;
        co_heap_limit = .;
    } > ram

    .data : {
        _data = .;
        *(.data*)    /* Read-write initialized data */
        . = ALIGN(4);
        _edata = .;
    } > ram AT > flash
    _data_loadaddr = LOADADDR(.data);

    .bss : {
        *(.bss*)    /* Read-write zero initialized data */
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >ram

    . = ALIGN(4);
    end = .;
}
