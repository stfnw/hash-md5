/* GLOBAL.H - RSAREF types and constants */

#include <stdint.h>

/* PROTOTYPES should be set to one if and only if the compiler supports
 * function argument prototyping.
 * The following makes PROTOTYPES default to 0 if it has not already
 * been defined with C compiler flags. */
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* POINTER defines a generic pointer type */
typedef u8 *POINTER;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
 * If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
 * returns an empty list. */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif
