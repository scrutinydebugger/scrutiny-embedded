//    file3.cpp
//        Placeholder file for variable extraction test
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "file3.hpp"

File3Union file3_union;
File3AnonymousBitfieldInUnion file3_anonbitfield_in_union;
FileNamespace::File3TestClass file3_test_class;

void file3SetValues()
{
    file3_union.u32_var = 0x12345678;
    file3_union.u16_var = 0xAABB;
    file3_union.u8_var = 0x99;
    // result should be 99AA3412 in memory (little endian)

    file3_anonbitfield_in_union.val = 0x55;
    file3_anonbitfield_in_union.bits.bit1 = 0;
    file3_anonbitfield_in_union.bits.bit5_8 = 7;
    // result should be 0x74

    file3_test_class.m_file3testclass_inclassenum = FileNamespace::File3TestClass::File3EnumInClass::BBB;

    file3_test_class.m_file3_complex_struct.field1 = 0x11223344;
    file3_test_class.m_file3_complex_struct.field2 = 0x55667788;
    file3_test_class.m_file3_complex_struct.field3.field3_u32 = 0x12345678;
    file3_test_class.m_file3_complex_struct.field3.field3_u16.p0 = 0xBCDE;
    file3_test_class.m_file3_complex_struct.field3.field3_u8.p3 = 0xAA;
    file3_test_class.m_file3_complex_struct.field3.field3_enum_bitfields.p0 = FileNamespace::File3TestClass::File3EnumInClass::CCC;
    // Should read field3 = AA34BCC2
}