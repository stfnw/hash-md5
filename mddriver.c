/* MDDRIVER.C - test driver for MD2, MD4 and MD5 */

/* Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All
 * rights reserved.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software. */

#include "global.h"
#include "md5.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

/* Length of test block, number of test blocks. */
#define TEST_BLOCK_LEN 10000
#define TEST_BLOCK_COUNT 10000

static void MDString(u8 *);
static void MDTimeTrial(void);
static void MDTestSuite(void);
static void MDFile(char *);
static void MDFilter(void);
static void MDPrint(u8[16]);

#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final

/* Main driver.
 *
 * Arguments (may be any combination):
 *   -sstring - digests string
 *   -t       - runs time trial
 *   -x       - runs test script
 *   filename - digests file
 *   (none)   - digests standard input */
i32 main(i32 argc, char *argv[]) {
    if (argc > 1) {
        for (i32 i = 1; i < argc; i++)
            if (argv[i][0] == '-' && argv[i][1] == 's') {
                MDString((u8 *)argv[i] + 2);
            } else if (strcmp(argv[i], "-t") == 0) {
                MDTimeTrial();
            } else if (strcmp(argv[i], "-x") == 0) {
                MDTestSuite();
            } else {
                MDFile(argv[i]);
            }
    } else {
        MDFilter();
    }

    return (0);
}

/* Digests a string and prints the result. */
static void MDString(u8 *string) {
    u32 len = strlen((char *)string);

    MD_CTX context;
    MDInit(&context);
    MDUpdate(&context, string, len);

    u8 digest[16];
    MDFinal(digest, &context);

    printf("MD5 (\"%s\") = ", string);
    MDPrint(digest);
    printf("\n");
}

/* Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte
 *  blocks. */
static void MDTimeTrial() {
    printf("MD5 time trial. Digesting %d %d-byte blocks ...", TEST_BLOCK_LEN,
           TEST_BLOCK_COUNT);

    /* Initialize block */
    u8 block[TEST_BLOCK_LEN];
    for (u32 i = 0; i < TEST_BLOCK_LEN; i++) {
        block[i] = (u8)(i & 0xff);
    }

    /* Start timer */
    time_t startTime;
    time(&startTime);

    /* Digest blocks */
    MD_CTX context;
    MDInit(&context);
    for (u32 i = 0; i < TEST_BLOCK_COUNT; i++) {
        MDUpdate(&context, block, TEST_BLOCK_LEN);
    }
    u8 digest[16];
    MDFinal(digest, &context);

    /* Stop timer */
    time_t endTime;
    time(&endTime);

    printf(" done\n");
    printf("Digest = ");
    MDPrint(digest);
    printf("\nTime = %ld seconds\n", (long)(endTime - startTime));
    printf("Speed = %ld bytes/second\n", (long)TEST_BLOCK_LEN *
                                             (long)TEST_BLOCK_COUNT /
                                             (endTime - startTime));
}

/* Digests a reference suite of strings and prints the results. */
static void MDTestSuite() {
    printf("MD5 test suite:\n");

    MDString((u8 *)"");
    MDString((u8 *)"a");
    MDString((u8 *)"abc");
    MDString((u8 *)"message digest");
    MDString((u8 *)"abcdefghijklmnopqrstuvwxyz");
    MDString(
        (u8 *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    MDString((u8 *)"1234567890123456789012345678901234567890\
1234567890123456789012345678901234567890");
}

/* Digests a file and prints the result. */
static void MDFile(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "rb")) == NULL) {
        printf("%s can't be opened\n", filename);
    }

    else {
        MD_CTX context;
        MDInit(&context);

        u8 buffer[1024];
        i32 len;
        while ((len = fread(buffer, 1, 1024, file))) {
            MDUpdate(&context, buffer, len);
        }

        u8 digest[16];
        MDFinal(digest, &context);

        fclose(file);

        printf("MD5 (%s) = ", filename);
        MDPrint(digest);
        printf("\n");
    }
}

/* Digests the standard input and prints the result. */
static void MDFilter() {
    MD_CTX context;
    MDInit(&context);

    u8 buffer[16];
    i32 len;
    while ((len = fread(buffer, 1, 16, stdin))) {
        MDUpdate(&context, buffer, len);
    }

    u8 digest[16];
    MDFinal(digest, &context);

    MDPrint(digest);
    printf("\n");
}

/* Prints a message digest in hexadecimal. */
static void MDPrint(u8 digest[16]) {
    for (u8 i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
}
