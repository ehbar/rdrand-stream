/*
 * rdrand-stream - generate an endless stream of random numbers from the Intel
 *                 RDRAND instruction
 *
 * Copyright (C) 2021,2026 Eric Barrett <eric@ericbarrett.com>.
 * Released under the MIT license.  See LICENSE.txt for details.
 */
#pragma once

#include <stdint.h>

uint32_t cpu_supports_rdrand(void);
