/*
 * rdrand-stream - generate an endless stream of random numbers from the Intel
 *                 RDRAND instruction
 *
 * Copyright (C) 2021 Eric Barrett <eric@ericbarrett.com>.
 * Released under the MIT license.  See LICENSE.txt for details.
 */

#if !defined(__x86_64__) && !defined(__x86_64)
#  error This source file uses instructions specific to the x86_64 architecture.
#endif

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "cpuid.h"
#include "rdrand-fill.h"

// # of bytes collected before stdout write
const size_t BUF_SIZE = 256*256;  // 64 KiB

// retry count to pass to rdrand_fill()
const uint32_t RDRAND_RETRY_COUNT = 3;

// Condition flag for main loop
int do_exit = 0;


void handler_sigint(int sig) {
    do_exit = 1;
}


void set_signals() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler_sigint;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL)) {  // catch a SIGINT, save a terminal
        perror("sigaction()");
        exit(EXIT_FAILURE);
    }
}


/* write_buf() writes the given buffer with the given size to stdout (fd = 1).
 * Returns 0 if successful, non-zero if failure occurred.  Will retry short
 * writes.
 */
int write_buf(void *buf, size_t buf_size) {
    size_t wr_so_far = 0;
    while (wr_so_far < buf_size) {
        ssize_t wr_just_now = write(1, (char *)buf + wr_so_far, buf_size - wr_so_far);
        if (wr_just_now >= 0) {
            wr_so_far += wr_just_now;
            assert(wr_so_far <= buf_size);
        } else {
            if (errno != EPIPE) {  // EPIPE = stdout closed; not an error here
                perror("write()");
            }
            return 1;
        }
    }
    return 0;
}


int main(int argc__unused, char *argv__unused[]) {
    assert(BUF_SIZE % 8 == 0);
    if (!cpu_supports_rdrand()) {
        fputs("ERROR: The current CPU does not support the RDRAND instruction.\n", stderr);
        return EXIT_FAILURE;
    }
    set_signals();
                                           
    void *buf = malloc(BUF_SIZE);
    if (!buf) {
        perror("ERROR: malloc()");
        return EXIT_FAILURE;
    }

    while (!do_exit) {
#ifndef NDEBUG
        // zero buffer to make failure to fill more obvious in output stream
        // (in testing, this had a negligible performance impact)
        memset(buf, 0, BUF_SIZE);
#endif
        int ret = rdrand_fill(buf, BUF_SIZE, RDRAND_RETRY_COUNT);
        if (!ret) {
            fputs("ERROR: rdrand_fill failed\n", stderr);
            return EXIT_FAILURE;
        }

        // print buf to stdout as raw stream
        do_exit = write_buf(buf, BUF_SIZE);
    }
    free(buf);

    return EXIT_SUCCESS;
}
