#include "file5.hpp"
#include <stdint.h>

namespace File5NamespaceA
{
    struct File5StructA
    {
        int32_t i32;
        int32_t *i32_ptr;
        uint16_t u16_array[5];
    };

    struct File5StructB
    {
        uint32_t u32;
        File5StructA *structA_ptr;
    };

    int64_t file5i64;
    int32_t file5i32;
    int16_t file5i16;
    int8_t file5i8;

    uint64_t file5u64;
    uint32_t file5u32;
    uint16_t file5u16;
    uint8_t file5u8;

    File5StructA file5_structA;
    File5StructB file5_structB;

    File5StructB file5_structb_array[2];

} // namespace File5NamespaceA

namespace File5NamespaceB
{

    int64_t *file5i64_ptr{ nullptr };
    int32_t *file5i32_ptr{ nullptr };
    int16_t *file5i16_ptr{ nullptr };
    int8_t *file5i8_ptr{ nullptr };

    uint64_t *file5u64_ptr{ nullptr };
    uint32_t *file5u32_ptr{ nullptr };
    uint16_t *file5u16_ptr{ nullptr };
    uint8_t *file5u8_ptr{ nullptr };

    File5NamespaceA::File5StructA *file5_structA_ptr{ nullptr };
    File5NamespaceA::File5StructB *file5_structB_ptr{ nullptr };
    File5NamespaceA::File5StructB *file5_structB_array_ptr{ nullptr };

} // namespace File5NamespaceB

void file5SetValues()
{
    File5NamespaceA::file5i64 = -0x123456789abcdef;
    File5NamespaceA::file5i32 = -0x77553311;
    File5NamespaceA::file5i16 = -0x2A3C;
    File5NamespaceA::file5i8 = -0x35;
    File5NamespaceA::file5u64 = 0x98765432123456;
    File5NamespaceA::file5u32 = 0xaabbccdd;
    File5NamespaceA::file5u16 = 0xFDCE;
    File5NamespaceA::file5u8 = 0xBD;

    File5NamespaceB::file5i64_ptr = &File5NamespaceA::file5i64;
    File5NamespaceB::file5i32_ptr = &File5NamespaceA::file5i32;
    File5NamespaceB::file5i16_ptr = &File5NamespaceA::file5i16;
    File5NamespaceB::file5i8_ptr = &File5NamespaceA::file5i8;

    File5NamespaceB::file5u64_ptr = &File5NamespaceA::file5u64;
    File5NamespaceB::file5u32_ptr = &File5NamespaceA::file5u32;
    File5NamespaceB::file5u16_ptr = &File5NamespaceA::file5u16;
    File5NamespaceB::file5u8_ptr = &File5NamespaceA::file5u8;

    File5NamespaceB::file5_structA_ptr = &File5NamespaceA::file5_structA;
    File5NamespaceB::file5_structB_ptr = &File5NamespaceA::file5_structB;
    File5NamespaceB::file5_structB_array_ptr = File5NamespaceA::file5_structb_array;

    File5NamespaceA::file5_structA.i32 = 31415926;
    File5NamespaceA::file5_structA.i32_ptr = &File5NamespaceA::file5i32;
    File5NamespaceA::file5_structA.u16_array[0] = 0x121;
    File5NamespaceA::file5_structA.u16_array[1] = 0x122;
    File5NamespaceA::file5_structA.u16_array[2] = 0x123;
    File5NamespaceA::file5_structA.u16_array[3] = 0x124;
    File5NamespaceA::file5_structA.u16_array[4] = 0x125;

    File5NamespaceA::file5_structB.u32 = 0x97461346;
    File5NamespaceA::file5_structB.structA_ptr = &File5NamespaceA::file5_structA;

    File5NamespaceA::file5_structb_array[0].u32 = 0x1195735;
    File5NamespaceA::file5_structb_array[0].structA_ptr = &File5NamespaceA::file5_structA;

    File5NamespaceA::file5_structb_array[1].u32 = 0x1284629;
    File5NamespaceA::file5_structb_array[1].structA_ptr = &File5NamespaceA::file5_structA;
}