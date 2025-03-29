Refactored MD5 implementation from https://datatracker.ietf.org/doc/html/rfc1321.
Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm.

# Usage

Build with:

```
$ ./build.sh
+ CFLAGS='-Wall -Wextra -g'
+ gcc -Wall -Wextra -g -c md5c.c
+ gcc -Wall -Wextra -g -c mddriver.c
+ gcc -Wall -Wextra -g -o mddriver md5c.o mddriver.o
```

Commandline parameters (from mddriver.c):

```c
/* Main driver.
 *
 * Arguments (may be any combination):
 *   -sstring - digests string
 *   -t       - runs time trial
 *   -x       - runs test script
 *   filename - digests file
 *   (none)   - digests standard input */
```

# Example usage

```
$ printf 'Hello World' | ./mddriver
b10a8db164e0754105b7a99be72e3fe5

$ printf 'Hello World' | md5sum
b10a8db164e0754105b7a99be72e3fe5  -
```
