Fibonacci
=========

> In mathematics, the Fibonacci numbers are the numbers in the
> following integer sequence, called the Fibonacci sequence, and
> characterized by the fact that every number after the first two is
> the sum of the two preceding ones.
> [Wikipedia](https://en.wikipedia.org/wiki/Fibonacci_number)

Start of the Fibonacci sequence:

1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, ...

## Pseudocode
```c
int fibonacci(int n) {
    int prev = 0;
    int next = 1;

    for (int i = 1; i < n; i++) {
        int sum = prev + next;

        prev = next;
        next = sum;
    }

    return next;
}
```

## Logical flow
The program flow is almost identical to that of the pseudocode. It
starts with initializing the variables `prev` and `next`. After this
we set up a iterative loop starting at one. For each iteration we sum
our two numbers and store the value. If having iterated as many times
as first declared, jump forward, if not jump back to start of loop.
