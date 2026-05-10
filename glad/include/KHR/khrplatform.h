/*
 * KHR/khrplatform.h — Khronos 平台类型定义
 * 从 Khronos 官方仓库提取，用于 GLAD
 */
#ifndef __khrplatform_h_
#define __khrplatform_h_

#include <stdint.h>
#include <stddef.h>

typedef int32_t  khronos_int32_t;
typedef uint32_t khronos_uint32_t;
typedef int64_t  khronos_int64_t;
typedef uint64_t khronos_uint64_t;
typedef float    khronos_float_t;
typedef signed   char khronos_int8_t;
typedef unsigned char khronos_uint8_t;
typedef int16_t  khronos_int16_t;
typedef uint16_t khronos_uint16_t;
typedef intptr_t khronos_intptr_t;
typedef uintptr_t khronos_uintptr_t;
typedef ptrdiff_t khronos_ssize_t;
typedef size_t   khronos_usize_t;

#define KHRONOS_MAX_ENUM 0x7FFFFFFF
typedef enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1,
    KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = KHRONOS_MAX_ENUM
} khronos_boolean_enum_t;

#endif /* __khrplatform_h_ */
