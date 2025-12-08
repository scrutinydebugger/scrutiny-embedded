//    file4.hpp
//        Placeholder file for variable extraction test
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include <stdint.h>

namespace File4NamespaceA1
{
    enum class File4EnumA : uint16_t
    {
        XXX = 123,
        YYY = 456
    };

    namespace File4NamespaceA2
    {
        class File4ClassA1
        {
          public:
            uint32_t xxx;
            int32_t yyy[2][3];
        };

        class File4ClassA2 : public File4ClassA1
        {
          public:
            uint16_t zzz[4][5][6];
            File4EnumA A2enum;
        };

        class File4ClassA3
        {
          public:
            union
            {
                uint32_t u32;
                uint8_t u8[4];
            } the_union;
        };
    } // namespace File4NamespaceA2
} // namespace File4NamespaceA1

namespace File4NamespaceB
{
    class File4ClassB
    {
      public:
        bool some_bool;
        File4NamespaceA1::File4NamespaceA2::File4ClassA2 array_of_A2[3][2];
    };
} // namespace File4NamespaceB

extern File4NamespaceB::File4ClassB file4classB;
extern File4NamespaceB::File4ClassB file4classB_array[3];
extern File4NamespaceA1::File4NamespaceA2::File4ClassA3 file4classA3_array[2][2];

void file4SetValues();