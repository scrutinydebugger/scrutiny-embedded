// No ifdef guards on purpose

#include <stdint.h>
#include "scrutiny_setup.hpp"

typedef void (*scrutiny_c_user_command_callback_t)(
    const uint8_t subfunction,
    const uint8_t *request_data,
    const uint16_t request_data_length,
    uint8_t *response_data,
    uint16_t *response_data_length,
    const uint16_t response_max_data_length);

typedef enum
{
    SCRUTINY_C_ENDIANNESS_LITTLE,
    SCRUTINY_C_ENDIANNESS_BIG
} scrutiny_c_endianness_e;

typedef struct
{
    void *start;
    void *end;
} scrutiny_c_address_range_t;

typedef uint32_t scrutiny_c_timediff_t;

typedef enum
{
    SCRUTINY_C_VARIABLE_TYPE_TYPE_sint = 0 << 4,
    SCRUTINY_C_VARIABLE_TYPE_TYPE_uint = 1 << 4,
    SCRUTINY_C_VARIABLE_TYPE_TYPE_float = 2 << 4,
    SCRUTINY_C_VARIABLE_TYPE_TYPE_boolean = 3 << 4,
    SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat = 4 << 4,
    SCRUTINY_C_VARIABLE_TYPE_TYPE_undef = 0xF << 4
} scrutiny_c_variable_type_type_e;

/// @brief Represent a type size
typedef enum
{
    SCRUTINY_C_VARIABLE_TYPE_SIZE_8 = 0,
    SCRUTINY_C_VARIABLE_TYPE_SIZE_16 = 1,
    SCRUTINY_C_VARIABLE_TYPE_SIZE_32 = 2,
    SCRUTINY_C_VARIABLE_TYPE_SIZE_64 = 3,
    SCRUTINY_C_VARIABLE_TYPE_SIZE_128 = 4,
    SCRUTINY_C_VARIABLE_TYPE_SIZE_256 = 5,
    SCRUTINY_C_VARIABLE_TYPE_SIZE_undef = 0xF
} scrutiny_c_variable_type_size_e;

/// @brief  Represent a datatype. Must match the python core module enum
typedef enum
{
    SCRUTINY_C_VARIABLE_TYPE_sint8 = SCRUTINY_C_VARIABLE_TYPE_TYPE_sint | SCRUTINY_C_VARIABLE_TYPE_SIZE_8,       // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_sint16 = SCRUTINY_C_VARIABLE_TYPE_TYPE_sint | SCRUTINY_C_VARIABLE_TYPE_SIZE_16,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_sint32 = SCRUTINY_C_VARIABLE_TYPE_TYPE_sint | SCRUTINY_C_VARIABLE_TYPE_SIZE_32,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_uint8 = SCRUTINY_C_VARIABLE_TYPE_TYPE_uint | SCRUTINY_C_VARIABLE_TYPE_SIZE_8,       // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_uint16 = SCRUTINY_C_VARIABLE_TYPE_TYPE_uint | SCRUTINY_C_VARIABLE_TYPE_SIZE_16,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_uint32 = SCRUTINY_C_VARIABLE_TYPE_TYPE_uint | SCRUTINY_C_VARIABLE_TYPE_SIZE_32,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_float8 = SCRUTINY_C_VARIABLE_TYPE_TYPE_float | SCRUTINY_C_VARIABLE_TYPE_SIZE_8,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_float16 = SCRUTINY_C_VARIABLE_TYPE_TYPE_float | SCRUTINY_C_VARIABLE_TYPE_SIZE_16,   // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_float32 = SCRUTINY_C_VARIABLE_TYPE_TYPE_float | SCRUTINY_C_VARIABLE_TYPE_SIZE_32,   // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_cfloat8 = SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat | SCRUTINY_C_VARIABLE_TYPE_SIZE_8,   // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_cfloat16 = SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat | SCRUTINY_C_VARIABLE_TYPE_SIZE_16, // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_cfloat32 = SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat | SCRUTINY_C_VARIABLE_TYPE_SIZE_32, // cppcheck-suppress[badBitmaskCheck]
#ifdef __cplusplus
    SCRUTINY_C_VARIABLE_TYPE_boolean = SCRUTINY_C_VARIABLE_TYPE_TYPE_boolean | SCRUTINY_C_VARIABLE_TYPE_SIZE_8, // cppcheck-suppress[badBitmaskCheck]
#endif

#if SCRUTINY_SUPPORT_64BITS
    SCRUTINY_C_VARIABLE_TYPE_sint64 = SCRUTINY_C_VARIABLE_TYPE_TYPE_sint | SCRUTINY_C_VARIABLE_TYPE_SIZE_64,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_uint64 = SCRUTINY_C_VARIABLE_TYPE_TYPE_uint | SCRUTINY_C_VARIABLE_TYPE_SIZE_64,     // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_float64 = SCRUTINY_C_VARIABLE_TYPE_TYPE_float | SCRUTINY_C_VARIABLE_TYPE_SIZE_64,   // cppcheck-suppress[badBitmaskCheck]
    SCRUTINY_C_VARIABLE_TYPE_cfloat64 = SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat | SCRUTINY_C_VARIABLE_TYPE_SIZE_64, // cppcheck-suppress[badBitmaskCheck]
#endif
    SCRUTINY_C_VARIABLE_TYPE_unknown = 0xFF
} scrutiny_c_variable_type_e;

typedef union
{
    uint8_t uint8;
    uint16_t uint16;
    uint32_t uint32;

    int8_t sint8;
    int16_t sint16;
    int32_t sint32;

    float float32;
#ifdef __cplusplus
    bool boolean;
#endif
#if SCRUTINY_SUPPORT_64BITS
    int64_t sint64;
    uint64_t uint64;
    double float64;
#endif
} scrutiny_c_any_type_t;

/// @brief The fast version of AnyType
typedef union
{
    uint_fast8_t uint8;
    uint_fast16_t uint16;
    uint_fast32_t uint32;

    int_fast8_t sint8;
    int_fast16_t sint16;
    int_fast32_t sint32;

    float float32;
#ifdef __cplusplus
    bool boolean;
#endif
#if SCRUTINY_SUPPORT_64BITS
    uint_fast64_t uint64;
    int_fast64_t sint64;
    double float64;
#endif
} scrutiny_c_any_type_fast_t;

/// @brief  Represent a RuntimePublishedValue definition.
/// It is a data object with a type and a ID that can be read/written by the server
typedef struct
{
    uint16_t id;
    uint8_t type;
} scrutiny_c_runtime_published_value_t;

/// @brief Callback called on Runtime Published Value read
typedef int (*scrutiny_c_rpv_read_callback_t)(const scrutiny_c_runtime_published_value_t rpv, scrutiny_c_any_type_t *outval);
/// @brief Callback called on Runtime Published Value write
typedef int (*scrutiny_c_rpv_write_callback_t)(const scrutiny_c_runtime_published_value_t rpv, const scrutiny_c_any_type_t *inval);

#if SCRUTINY_ENABLE_DATALOGGING
typedef void (*scrutiny_c_datalogging_trigger_callback_t)();
#if SCRUTINY_DATALOGGING_BUFFER_32BITS
typedef uint32_t scrutiny_c_datalogging_buffer_size_t;
#else
typedef uint16_t scrutiny_c_datalogging_buffer_size_t;
#endif
#endif
