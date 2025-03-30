#!/bin/bash

set -ex

CFLAGS="-Wall -Wextra -g"

gcc $CFLAGS -c md5c.c
gcc $CFLAGS -c mddriver.c
gcc $CFLAGS -o mddriver md5c.o mddriver.o

gcc $CFLAGS -o standalone-md5 standalone-md5.c
