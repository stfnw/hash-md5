/*
 * SPDX-FileCopyrightText: 2025 Refactoring and modifications: stfnw. Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Refactored MD5 implementation from
 * https://datatracker.ietf.org/doc/html/rfc1321. Derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm. Self-contained single-file
 * implementation; with macros replaced with functions. */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct {
    u32 state[4];  /* state (ABCD) */
    u32 count[2];  /* number of bits, modulo 2^64 (lsb first) */
    u8 buffer[64]; /* input buffer */
} MD5_CTX;

u32 S11 = 7;
u32 S12 = 12;
u32 S13 = 17;
u32 S14 = 22;
u32 S21 = 5;
u32 S22 = 9;
u32 S23 = 14;
u32 S24 = 20;
u32 S31 = 4;
u32 S32 = 11;
u32 S33 = 16;
u32 S34 = 23;
u32 S41 = 6;
u32 S42 = 10;
u32 S43 = 15;
u32 S44 = 21;

static u8 PADDING[64] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

u32 F(u32 x, u32 y, u32 z) { return (x & y) | ((~x) & z); }
u32 G(u32 x, u32 y, u32 z) { return (x & z) | (y & (~z)); }
u32 H(u32 x, u32 y, u32 z) { return x ^ y ^ z; }
u32 I(u32 x, u32 y, u32 z) { return y ^ (x | (~z)); }

u32 rotate_left(u32 x, u32 n) { return (x << n) | (x >> (32 - n)); }

u32 FF(u32 a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) {
    u32 tmp1 = a + F(b, c, d) + x + ac;
    u32 tmp2 = rotate_left(tmp1, s);
    u32 tmp3 = tmp2 + b;
    return tmp3;
}

u32 GG(u32 a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) {
    u32 tmp1 = a + G(b, c, d) + x + ac;
    u32 tmp2 = rotate_left(tmp1, s);
    u32 tmp3 = tmp2 + b;
    return tmp3;
}

u32 HH(u32 a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) {
    u32 tmp1 = a + H(b, c, d) + x + ac;
    u32 tmp2 = rotate_left(tmp1, s);
    u32 tmp3 = tmp2 + b;
    return tmp3;
}

u32 II(u32 a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) {
    u32 tmp1 = a + I(b, c, d) + x + ac;
    u32 tmp2 = rotate_left(tmp1, s);
    u32 tmp3 = tmp2 + b;
    return tmp3;
}

/* MD5 initialization. Begins an MD5 operation, writing a new context. */
void MD5Init(MD5_CTX *context /* context */) {
    context->count[0] = context->count[1] = 0;
    /* Load magic initialization constants. */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

/* Encodes input (u32) into output (u8). Assumes len is
 * a multiple of 4. */
static void Encode(u8 *output, u32 *input, u32 len) {
    for (u32 i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (u8)(input[i] & 0xff);
        output[j + 1] = (u8)((input[i] >> 8) & 0xff);
        output[j + 2] = (u8)((input[i] >> 16) & 0xff);
        output[j + 3] = (u8)((input[i] >> 24) & 0xff);
    }
}

/* Decodes input (u8) into output (u32). Assumes len is
 *  a multiple of 4. */
static void Decode(u32 *output, u8 *input, u32 len) {
    for (u32 i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = ((u32)input[j]) | (((u32)input[j + 1]) << 8) |
                    (((u32)input[j + 2]) << 16) | (((u32)input[j + 3]) << 24);
    }
}

/* MD5 basic transformation. Transforms state based on block. */
static void MD5Transform(u32 state[4], u8 block[64]) {
    u32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    Decode(x, block, 64);

    /* Round 1 */
    a = FF(a, b, c, d, x[0], S11, 0xd76aa478);  /* 1 */
    d = FF(d, a, b, c, x[1], S12, 0xe8c7b756);  /* 2 */
    c = FF(c, d, a, b, x[2], S13, 0x242070db);  /* 3 */
    b = FF(b, c, d, a, x[3], S14, 0xc1bdceee);  /* 4 */
    a = FF(a, b, c, d, x[4], S11, 0xf57c0faf);  /* 5 */
    d = FF(d, a, b, c, x[5], S12, 0x4787c62a);  /* 6 */
    c = FF(c, d, a, b, x[6], S13, 0xa8304613);  /* 7 */
    b = FF(b, c, d, a, x[7], S14, 0xfd469501);  /* 8 */
    a = FF(a, b, c, d, x[8], S11, 0x698098d8);  /* 9 */
    d = FF(d, a, b, c, x[9], S12, 0x8b44f7af);  /* 10 */
    c = FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    b = FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    a = FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    d = FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    c = FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    b = FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    a = GG(a, b, c, d, x[1], S21, 0xf61e2562);  /* 17 */
    d = GG(d, a, b, c, x[6], S22, 0xc040b340);  /* 18 */
    c = GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    b = GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);  /* 20 */
    a = GG(a, b, c, d, x[5], S21, 0xd62f105d);  /* 21 */
    d = GG(d, a, b, c, x[10], S22, 0x2441453);  /* 22 */
    c = GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    b = GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);  /* 24 */
    a = GG(a, b, c, d, x[9], S21, 0x21e1cde6);  /* 25 */
    d = GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    c = GG(c, d, a, b, x[3], S23, 0xf4d50d87);  /* 27 */
    b = GG(b, c, d, a, x[8], S24, 0x455a14ed);  /* 28 */
    a = GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    d = GG(d, a, b, c, x[2], S22, 0xfcefa3f8);  /* 30 */
    c = GG(c, d, a, b, x[7], S23, 0x676f02d9);  /* 31 */
    b = GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    a = HH(a, b, c, d, x[5], S31, 0xfffa3942);  /* 33 */
    d = HH(d, a, b, c, x[8], S32, 0x8771f681);  /* 34 */
    c = HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    b = HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    a = HH(a, b, c, d, x[1], S31, 0xa4beea44);  /* 37 */
    d = HH(d, a, b, c, x[4], S32, 0x4bdecfa9);  /* 38 */
    c = HH(c, d, a, b, x[7], S33, 0xf6bb4b60);  /* 39 */
    b = HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    a = HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    d = HH(d, a, b, c, x[0], S32, 0xeaa127fa);  /* 42 */
    c = HH(c, d, a, b, x[3], S33, 0xd4ef3085);  /* 43 */
    b = HH(b, c, d, a, x[6], S34, 0x4881d05);   /* 44 */
    a = HH(a, b, c, d, x[9], S31, 0xd9d4d039);  /* 45 */
    d = HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    c = HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    b = HH(b, c, d, a, x[2], S34, 0xc4ac5665);  /* 48 */

    /* Round 4 */
    a = II(a, b, c, d, x[0], S41, 0xf4292244);  /* 49 */
    d = II(d, a, b, c, x[7], S42, 0x432aff97);  /* 50 */
    c = II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    b = II(b, c, d, a, x[5], S44, 0xfc93a039);  /* 52 */
    a = II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    d = II(d, a, b, c, x[3], S42, 0x8f0ccc92);  /* 54 */
    c = II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    b = II(b, c, d, a, x[1], S44, 0x85845dd1);  /* 56 */
    a = II(a, b, c, d, x[8], S41, 0x6fa87e4f);  /* 57 */
    d = II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    c = II(c, d, a, b, x[6], S43, 0xa3014314);  /* 59 */
    b = II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    a = II(a, b, c, d, x[4], S41, 0xf7537e82);  /* 61 */
    d = II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    c = II(c, d, a, b, x[2], S43, 0x2ad7d2bb);  /* 63 */
    b = II(b, c, d, a, x[9], S44, 0xeb86d391);  /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /* Zeroize sensitive information. */
    memset((u8 *)x, 0, sizeof(x));
}

/* MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context. */
void MD5Update(MD5_CTX *context /* context */, u8 *input /* input block */,
               u32 inputLen /* length of input block */) {
    /* Compute number of bytes mod 64 */
    u32 index = (u32)((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((context->count[0] += ((u32)inputLen << 3)) < ((u32)inputLen << 3))
        context->count[1]++;
    context->count[1] += ((u32)inputLen >> 29);

    u32 partLen = 64 - index;

    /* Transform as many times as possible. */
    u32 i;
    if (inputLen >= partLen) {
        memcpy((u8 *)&context->buffer[index], (u8 *)input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64) {
            MD5Transform(context->state, &input[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    /* Buffer remaining input */
    memcpy((u8 *)&context->buffer[index], (u8 *)&input[i], inputLen - i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context. */
void MD5Final(u8 digest[16] /* message digest */,
              MD5_CTX *context /* context */) {
    /* Save number of bits */
    u8 bits[8];
    Encode(bits, context->count, 8);

    /* Pad out to 56 mod 64. */
    u32 index = (u32)((context->count[0] >> 3) & 0x3f);
    u32 padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, PADDING, padLen);

    /* Append length (before padding) */
    MD5Update(context, bits, 8);

    /* Store state in digest */
    Encode(digest, context->state, 16);

    /* Zeroize sensitive information. */
    memset((u8 *)context, 0, sizeof(*context));
}

/* Prints a message digest in hexadecimal. */
static void MD5Print(u8 digest[16]) {
    for (u8 i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
}

static void MDFile(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "rb")) == NULL) {
        printf("%s can't be opened\n", filename);
    } else {
        MD5_CTX context;
        MD5Init(&context);

        u8 buffer[1024];
        i32 len;
        while ((len = fread(buffer, 1, 1024, file))) {
            MD5Update(&context, buffer, len);
        }

        u8 digest[16];
        MD5Final(digest, &context);

        fclose(file);

        printf("MD5 (%s) = ", filename);
        MD5Print(digest);
        printf("\n");
    }
}

i32 main(i32 argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s infile\n", argv[0]);
        return 1;
    }

    MDFile(argv[1]);

    return 0;
}
