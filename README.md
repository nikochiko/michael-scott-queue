# Michael-Scott queue

Implements the [Michael-Scott queue](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf)
in C with the `compare_and_swap` instruction. In C, this instruction
is exposed as the [`__sync_bool_compare_and_swap()`](https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html)
function.

The [cas.c](cas.c) file contains an implementation of the Michael-Scott
queue using this function. The file also contains some preliminary stress
tests for the queue, where we create a large number of threads each
of which enqueues one value to the queue. We then dequeue the
exact same number of items and make sure the count of enqueued
items is the same as the count of dequeued items.

There is also another file [non_cas_queue.c](non_cas_queue.c) which
contains an analogous implementation of queues as a singly-linked
list but without the concurrency safety brought by the compare_and_swap
instruction. This has some tests and the probability of this program
segfaulting goes up significantly with the number of threads we
enqueue items from. This is to see/show the difference between using
a concurrent data structure and a non-concurrent one.
