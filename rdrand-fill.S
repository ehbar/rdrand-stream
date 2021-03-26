/*
 * rdrand-stream - generate an endless stream of random numbers from the Intel
 *                 RDRAND instruction
 *
 * Copyright (C) 2021 Eric Barrett <eric@ericbarrett.com>.
 * Released under the MIT license.  See LICENSE.txt for details.
 */

/* How to use this function in a C program; should work similarly for other
 * languages:
 *
 *   #include "rdrand-fill.h"
 *   ...
 *   const uint32_t retries = 3;
 *   const size_t buf_size = 8192;  // must be multiple of 8
 *   ...
 *   buf = malloc(buf_size);
 *   int ret = rdrand_fill_asm(buf, buf_size, retries);
 *   if (!ret) handle_error();
 */

/* _rdrand_fill
 *
 * Arguments
 *   0 [rdi]: buffer
 *   1 [rsi]: buffer size
 *   2 [edx]: retries
 *
 * Register variables
 *   rax: rdrand output
 *   ecx: retry count
 *   r8: current buffer offset
 */
.global _rdrand_fill
_rdrand_fill:
        endbr64

        movq        $0,%r8              // init buffer offset

        // If the buffer size is not a multiple of 8 bytes, it is an error
        movq        %rsi,%rax
        and         $0b111,%rax
        test        %rax,%rax
        jz          rf__enter

        // ...it wasn't.
        movq        $0,%rax              // return failure
        jmp         rf__ret

rf__enter:
        mov         $0,%ecx              // retries = 0
rf__try:
        lfence                          // spectre?
        rdrand      %rax
        jc          rf__ok               // rdrand sets carry flag on success

        // rdrand failed, increment retry count & loop if under threshold
        inc         %ecx
        cmpl        %edx,%ecx
        jl          rf__try
        movq        $0,%rax              // return failure
        jmp         rf__ret

rf__ok:
        // store our 64 random bits in the buffer
        movq        %rax,(%rdi,%r8)
        addq        $8,%r8
        cmp         %rsi,%r8             // have we reached the end?
        jl          rf__enter
        movq        $1,%rax              // return success

rf__ret:
        retq
