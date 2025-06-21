//    file2.c
//        Placeholder file for variable extraction test
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "file2.h"

char file2GlobalChar;
int file2GlobalInt;
short file2GlobalShort;
long file2GlobalLong;
unsigned char file2GlobalUnsignedChar;
unsigned int file2GlobalUnsignedInt;
unsigned short file2GlobalUnsignedShort;
unsigned long file2GlobalUnsignedLong;
float file2GlobalFloat;
double file2GlobalDouble;
int file2GlobalArray1Int5[5];
float file2GlobalArray2x2Float[2][2];

static char file2StaticChar;
static int file2StaticInt;
static short file2StaticShort;
static long file2StaticLong;
static unsigned char file2StaticUnsignedChar;
static unsigned int file2StaticUnsignedInt;
static unsigned short file2StaticUnsignedShort;
static unsigned long file2StaticUnsignedLong;
static float file2StaticFloat;
static double file2StaticDouble;

enum EnumA
{
    eVal1,
    eVal2,
    eVal3 = 100,
    eVal4
};

struct File2StructWithEnumA
{
    int a;
    enum EnumA instance_enumA;
    int b;
};

enum EnumA instance_enumA;
static enum EnumA staticInstance_enumA;

struct File2StructWithEnumA instance_structWithEnumA;
static volatile struct File2StructWithEnumA static_instance_structWithEnumA;

void file2SetValues()
{
    file2GlobalChar = 20;
    file2GlobalInt = 2000;
    file2GlobalShort = 998;
    file2GlobalLong = 555555;
    file2GlobalUnsignedChar = 254u;
    file2GlobalUnsignedInt = 123456u;
    file2GlobalUnsignedShort = 12345u;
    file2GlobalUnsignedLong = 1234567u;
    file2GlobalFloat = 0.1f;
    file2GlobalDouble = 0.11111111111111;

    file2StaticChar = -66;
    file2StaticInt = -8745;
    file2StaticShort = -9876;
    file2StaticLong = -12345678;
    file2StaticUnsignedChar = 12u;
    file2StaticUnsignedInt = 34u;
    file2StaticUnsignedShort = 56u;
    file2StaticUnsignedLong = 78u;
    file2StaticFloat = 2.22222f;
    file2StaticDouble = 3.3333;

    instance_enumA = eVal4;
    staticInstance_enumA = eVal1;

    instance_structWithEnumA.a = 123;
    instance_structWithEnumA.b = 456;
    instance_structWithEnumA.instance_enumA = eVal2;
    static_instance_structWithEnumA.a = 135;
    static_instance_structWithEnumA.b = 246;
    static_instance_structWithEnumA.instance_enumA = eVal3;

    file2GlobalArray1Int5[0] = 1111;
    file2GlobalArray1Int5[1] = 2222;
    file2GlobalArray1Int5[2] = 3333;
    file2GlobalArray1Int5[3] = 4444;
    file2GlobalArray1Int5[4] = 5555;

    file2GlobalArray2x2Float[0][0] = 1.1f;
    file2GlobalArray2x2Float[0][1] = 2.2f;
    file2GlobalArray2x2Float[1][0] = 3.3f;
    file2GlobalArray2x2Float[1][1] = 4.4f;
}

void file2func1(int x)
{
    (void)x;
    static double file2func1Var = 963258741.123;
    file2func1Var += (double)x;
    (void)file2func1Var;
}
