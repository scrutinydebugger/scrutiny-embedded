#include "file4.hpp"

File4NamespaceB::File4ClassB file4classB;
File4NamespaceB::File4ClassB file4classB_array[3];
File4NamespaceA1::File4NamespaceA2::File4ClassA3 file4classA3_array[2][2];

void file4SetValues()
{
    file4classB.some_bool = true;
    file4classB.array_of_A2[2][0].xxx = 0xdeadbeef;
    file4classB.array_of_A2[2][0].yyy[0][0] = 0x10001111;
    file4classB.array_of_A2[2][0].yyy[0][1] = 0x20002222;
    file4classB.array_of_A2[2][0].yyy[0][2] = 0x30003333;
    file4classB.array_of_A2[2][0].yyy[1][2] = 0x40004444;
    file4classB.array_of_A2[2][0].zzz[0][0][0] = 0x5566;
    file4classB.array_of_A2[2][0].zzz[1][2][3] = 0x6789;
    file4classB.array_of_A2[2][0].A2enum = File4NamespaceA1::File4EnumA::YYY;

    file4classB_array[0].some_bool = true;
    file4classB_array[0].array_of_A2[2][0].xxx = 0xdeadbeef + 1;
    file4classB_array[0].array_of_A2[2][0].yyy[0][0] = 0x10001111 + 1;
    file4classB_array[0].array_of_A2[2][0].yyy[0][1] = 0x20002222 + 1;
    file4classB_array[0].array_of_A2[2][0].yyy[0][2] = 0x30003333 + 1;
    file4classB_array[0].array_of_A2[2][0].yyy[1][2] = 0x40004444 + 1;
    file4classB_array[0].array_of_A2[2][0].zzz[0][0][0] = 0x5566 + 1;
    file4classB_array[0].array_of_A2[2][0].zzz[1][2][3] = 0x6789 + 1;
    file4classB_array[0].array_of_A2[2][0].A2enum = File4NamespaceA1::File4EnumA::XXX;

    file4classB_array[1].some_bool = true;
    file4classB_array[1].array_of_A2[2][0].xxx = 0xdeadbeef + 2;
    file4classB_array[1].array_of_A2[2][0].yyy[0][0] = 0x10001111 + 2;
    file4classB_array[1].array_of_A2[2][0].yyy[0][1] = 0x20002222 + 2;
    file4classB_array[1].array_of_A2[2][0].yyy[0][2] = 0x30003333 + 2;
    file4classB_array[1].array_of_A2[2][0].yyy[1][2] = 0x40004444 + 2;
    file4classB_array[1].array_of_A2[2][0].zzz[0][0][0] = 0x5566 + 2;
    file4classB_array[1].array_of_A2[2][0].zzz[1][2][3] = 0x6789 + 2;
    file4classB_array[1].array_of_A2[2][0].A2enum = File4NamespaceA1::File4EnumA::XXX;

    file4classB_array[2].some_bool = false;
    file4classB_array[2].array_of_A2[2][0].xxx = 0xdeadbeef + 3;
    file4classB_array[2].array_of_A2[2][0].yyy[0][0] = 0x10001111 + 3;
    file4classB_array[2].array_of_A2[2][0].yyy[0][1] = 0x20002222 + 3;
    file4classB_array[2].array_of_A2[2][0].yyy[0][2] = 0x30003333 + 3;
    file4classB_array[2].array_of_A2[2][0].yyy[1][2] = 0x40004444 + 3;
    file4classB_array[2].array_of_A2[2][0].zzz[0][0][0] = 0x5566 + 3;
    file4classB_array[2].array_of_A2[2][0].zzz[1][2][3] = 0x6789 + 3;
    file4classB_array[2].array_of_A2[2][0].A2enum = File4NamespaceA1::File4EnumA::YYY;

    file4classA3_array[0][0].the_union.u32 = 0x12345678;
    file4classA3_array[0][1].the_union.u32 = 0xAABBCCDD;
    file4classA3_array[1][0].the_union.u32 = 0x11223344;
    file4classA3_array[1][1].the_union.u32 = 0x55667788;
}