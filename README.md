# rdrand-stream

`rdrand-stream` is a standalone demonstration of the Intel [RDRAND instruction](https://en.wikipedia.org/wiki/RDRAND), available on most modern x86-64 processors.  When run, the program outputs an endless stream of random bits&mdash;assuming your chip's implementation isn't [broken](https://arstechnica.com/gadgets/2019/10/how-a-months-old-amd-microcode-bug-destroyed-my-weekend/).

The generator is implemented as the function `_rdrand_fill` in [rdrand-fill.S](rdrand-fill.S).  A C header providing the function `rdrand_fill(...)` is in [rdrand-fill.h](rdrand-fill.h).  [main.c](main.c) serves as the program body as well as a demo of the aforementioned function.

## Compiling

Just run `make`.  If it works you will have an `rdrand-stream` binary in the same directory.  You may have to change the compiler by modifying the CC= line in [Makefile](Makefile). (What's [autoconf](https://www.gnu.org/software/autoconf/autoconf.html)?)

If you get symbol errors during linking, you may also need to change the name of the `_rdrand_fill` function to meet your operating system's x86-64 C calling conventions.  (Make sure you update the `asm()` directive in the header file, as well.)  Tested on clang 11/MacOS 10.15 and gcc 9.3/Linux Mint (Ubuntu).

Obviously, an x86-64 compilation target is required.

## Performance

Though the `RDRAND` instruction is capable of generating gigabits per second on faster CPUs, this program peaks at between 100-800 megabits in my testing.  This is no doubt due to the [single-threaded nature of the process](https://stackoverflow.com/questions/10484164/what-is-the-latency-and-throughput-of-the-rdrand-instruction-on-ivy-bridge) combined with blocking writes to `stdout` between buffer fills.

Interestingly, I found speed varies on separate solo execution runs by as much as 7%.  The speed seems "locked in" at the start, and doesn't change until I end and restart the process.  This behavior occurs on both the CPUs I have for testing, an i7-1060NG7 (Ice Lake, hyperthreaded) and i7-i9700K (Coffee Lake, non-hyperthreaded).  Forcing buffer alignment to as coarse as 8k didn't change the behavior, and it was my only idea, so I'm not sure what's going on.

I also found that running multiple simultaneous instances will increase throughput on *individual* processes by a few percent, up to a point, whereafter additional copies rapidly slowed everything down.  In other words, peformance of n processes scaled by 𝜶n, where 𝜶 was between 1.01 and 1.05 and n < roughly the # of virtual CPUs.  This effect was much stronger on the i7-1060NG7, which is hyperthreaded, but still showed up on the i7-i9700K.  The behavior replicated in both MacOS and Linux.

# Important Disclaimer

This is an un-peer-reviewed personal project.  I am not a mathemetician, cryptographer, or expert assembly programmer.  I tried to write this as close to correctly as possible, but make no claims or guarantees as to its correctness, stability, security, or suitability for any purpose.  I did not consult any other implementation while writing `rdrand-stream`, for better or worse.  If you need professional `RDRAND` code, check out [librdrand](https://github.com/jtulak/RdRand).

# License and Copyright

Copyright © 2021 Eric Barrett <eric@ericbarrett.com>

Released under the MIT license; see [LICENSE.txt](LICENSE.txt).
