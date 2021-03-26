/*
 * rdrand-stream - generate an endless stream of random numbers from the Intel
 *                 RDRAND instruction
 *
 * Copyright (C) 2021 Eric Barrett <eric@ericbarrett.com>.
 * Released under the MIT license.  See LICENSE.txt for details.
 */

#include "cpuid.h"

#if !defined(__x86_64__) && !defined(__x86_64)
#  error This source file uses instructions specific to the x86_64 architecture.
#endif

// See: https://handwiki.org/wiki/CPUID
static const uint32_t ECX_RDRND_FEATURE_BIT = 1 << 30;

/* cpu_supports_rdrand() tests the rdrnd feature flag from the x86 CPUID
 * instruction.  It returns non-zero if the bit is set, indicating processor
 * support.
 */ 
uint32_t cpu_supports_rdrand(void) {
    uint32_t ecx_feature_flags;

    asm volatile(
        "  mov $1,%%eax\n"
        "  cpuid\n"
        : "=c" (ecx_feature_flags)     // outputs
        :                              // inputs (none)
        : "%eax","%ebx","%edx"         // clobbers
    );

    return ecx_feature_flags & ECX_RDRND_FEATURE_BIT;
}
