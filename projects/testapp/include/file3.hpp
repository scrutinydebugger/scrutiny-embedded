//    file3.hpp
//        Placeholder file for variable extraction test
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#include <cstdint>

union File3Union
{
    uint32_t u32_var;
    uint16_t u16_var;
    uint8_t u8_var;
};

union File3AnonymousBitfieldInUnion
{
    struct
    {
        uint8_t bit1 : 1;
        uint8_t bit2_4 : 3;
        uint8_t bit5_8 : 4;
    } bits;
    uint8_t val;
};

namespace FileNamespace
{
    class File3TestClass
    {
    public:
        enum class File3EnumInClass : uint32_t
        {
            AAA,
            BBB,
            CCC
        };

        File3EnumInClass m_file3testclass_inclassenum;
        struct
        {
            uint32_t field1;
            uint32_t field2;
            union
            {
                uint32_t field3_u32;
                struct
                {
                    uint16_t p0; // LSB
                    uint16_t p1;
                } field3_u16;
                struct
                {
                    uint8_t p0; // LSB
                    uint8_t p1;
                    uint8_t p2;
                    uint8_t p3;
                } field3_u8;
                struct
                {
                    File3EnumInClass p0 : 5;
                    File3EnumInClass p1 : 7;
                    File3EnumInClass p2 : 10;
                    File3EnumInClass p3 : 10;
                } field3_enum_bitfields;
            };
        } m_file3_complex_struct;
    };
}

void file3SetValues();