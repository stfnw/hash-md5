Refactored MD5 implementation from https://datatracker.ietf.org/doc/html/rfc1321.
Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm.

# Usage (standalone implementation)

Build with:

```
$ ./build.sh
+ CFLAGS='-Wall -Wextra -g'
+ gcc -Wall -Wextra -g -c md5c.c
+ gcc -Wall -Wextra -g -c mddriver.c
+ gcc -Wall -Wextra -g -o mddriver md5c.o mddriver.o
+ gcc -Wall -Wextra -g -o standalone-md5 standalone-md5.c
```

Usage:

```
./standalone-md5 infile
```

# Example usage (original code)

```
$ printf 'Hello World' > test
$ ./standalone-md5 test
MD5 (test) = b10a8db164e0754105b7a99be72e3fe5

$ printf 'Hello World' | md5sum
b10a8db164e0754105b7a99be72e3fe5  -
```

# Usage (original code)

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

# Example usage (original code)

```
$ printf 'Hello World' | ./mddriver
b10a8db164e0754105b7a99be72e3fe5

$ printf 'Hello World' | md5sum
b10a8db164e0754105b7a99be72e3fe5  -
```
