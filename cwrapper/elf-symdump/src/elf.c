//    elf.c
//        ELF parsing functions
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "elf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

uint8_t const ELF_MAGIC[4] = { 0x7F, 'E', 'L', 'F' };

#define ELF_HEADER_P1_SIZE 0x18
#define ELF_HEADER_32BITS_SIZE 0x34
#define ELF_HEADER_64BITS_SIZE 0x40
#define ELF_HEADER_MAX_SIZE MAX(ELF_HEADER_64BITS_SIZE, ELF_HEADER_32BITS_SIZE)
#define ELF_SYMBOL_32BITS_SIZE 16
#define ELF_SYMBOL_64BITS_SIZE 24

#define ELF_ASSERT(cond, msg)                                                                                                                        \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (!(cond))                                                                                                                                 \
        {                                                                                                                                            \
            fprintf(stderr, "%s\n", msg);                                                                                                            \
            fflush(stderr);                                                                                                                          \
            return ELF_FAIL;                                                                                                                         \
        }                                                                                                                                            \
                                                                                                                                                     \
    } while (0)

ELFStatus elf_read_section_header(ELFHeader const *const elfheader, uint8_t const *const data, ELFSectionHeader *const header)
{
    if (elfheader->e_class == ELF_CLASS_32BITS)
    {
        header->sh_name = read_32(elfheader->e_endianness, &data[0]);
        header->sh_type = read_32(elfheader->e_endianness, &data[0x4]);
        header->sh_flags = read_32(elfheader->e_endianness, &data[0x8]);
        header->sh_addr = read_32(elfheader->e_endianness, &data[0xC]);
        header->sh_offset = read_32(elfheader->e_endianness, &data[0x10]);
        header->sh_size = read_32(elfheader->e_endianness, &data[0x14]);
        header->sh_link = read_32(elfheader->e_endianness, &data[0x18]);
        header->sh_info = read_32(elfheader->e_endianness, &data[0x1C]);
        header->sh_addralign = read_32(elfheader->e_endianness, &data[0x20]);
        header->sh_entsize = read_32(elfheader->e_endianness, &data[0x24]);
    }
    else if (elfheader->e_class == ELF_CLASS_64BITS)
    {
        header->sh_name = read_32(elfheader->e_endianness, &data[0]);
        header->sh_type = read_32(elfheader->e_endianness, &data[0x4]);
        header->sh_flags = read_64(elfheader->e_endianness, &data[0x8]);
        header->sh_addr = read_64(elfheader->e_endianness, &data[0x10]);
        header->sh_offset = read_64(elfheader->e_endianness, &data[0x18]);
        header->sh_size = read_64(elfheader->e_endianness, &data[0x20]);
        header->sh_link = read_32(elfheader->e_endianness, &data[0x28]);
        header->sh_info = read_32(elfheader->e_endianness, &data[0x2C]);
        header->sh_addralign = read_64(elfheader->e_endianness, &data[0x30]);
        header->sh_entsize = read_64(elfheader->e_endianness, &data[0x38]);
    }
    else
    {
        return ELF_FAIL;
    }
    return ELF_OK;
}

ELFStatus elf_read_symbol(ELFHeader const *const elfheader, uint8_t const *const buf, ELFSymbol *const symbol)
{
    if (elfheader->e_class == ELF_CLASS_32BITS)
    {
        symbol->st_name = read_32(elfheader->e_endianness, &buf[0]);
        symbol->st_value = read_32(elfheader->e_endianness, &buf[4]);
        symbol->st_size = read_32(elfheader->e_endianness, &buf[8]);
        symbol->st_info = read_8(elfheader->e_endianness, &buf[12]);
        symbol->st_other = read_8(elfheader->e_endianness, &buf[13]);
        symbol->st_shndx = read_16(elfheader->e_endianness, &buf[14]);
    }
    else if (elfheader->e_class == ELF_CLASS_64BITS)
    {
        symbol->st_name = read_32(elfheader->e_endianness, &buf[0]);
        symbol->st_info = read_8(elfheader->e_endianness, &buf[4]);
        symbol->st_other = read_8(elfheader->e_endianness, &buf[5]);
        symbol->st_shndx = read_16(elfheader->e_endianness, &buf[6]);
        symbol->st_value = read_64(elfheader->e_endianness, &buf[8]);
        symbol->st_size = read_64(elfheader->e_endianness, &buf[16]);
    }
    else
    {
        return ELF_FAIL;
    }
    return ELF_OK;
}

uint8_t read_8(ELFEndianess const endianness, uint8_t const buf[])
{
    (void)endianness;
    return buf[0];
}

uint16_t read_16(ELFEndianess const endianness, uint8_t const buf[])
{
    if (endianness == ELF_ENDIAN_LITTLE)
    {
        return (((uint16_t)buf[1]) << 8) | (((uint16_t)buf[0]) << 0);
    }
    else if (endianness == ELF_ENDIAN_BIG)
    {
        return (((uint16_t)buf[0]) << 8) | (((uint16_t)buf[1]) << 0);
    }
    return 0;
}

uint32_t read_32(ELFEndianess const endianness, uint8_t const buf[])
{
    if (endianness == ELF_ENDIAN_LITTLE)
    {
        return (((uint32_t)buf[3]) << 24) | (((uint32_t)buf[2]) << 16) | (((uint32_t)buf[1]) << 8) | (((uint32_t)buf[0]) << 0);
    }
    else if (endianness == ELF_ENDIAN_BIG)
    {
        return (((uint32_t)buf[0]) << 24) | (((uint32_t)buf[1]) << 16) | (((uint32_t)buf[2]) << 8) | (((uint32_t)buf[3]) << 0);
    }
    return 0;
}

uint64_t read_64(ELFEndianess const endianness, uint8_t const buf[])
{
    if (endianness == ELF_ENDIAN_LITTLE)
    {
        return (((uint64_t)buf[7]) << 56) | (((uint64_t)buf[6]) << 48) | (((uint64_t)buf[5]) << 40) | (((uint64_t)buf[4]) << 32) |
               (((uint64_t)buf[3]) << 24) | (((uint64_t)buf[2]) << 16) | (((uint64_t)buf[1]) << 8) | (((uint64_t)buf[0]) << 0);
    }
    else if (endianness == ELF_ENDIAN_BIG)
    {
        return (((uint64_t)buf[0]) << 56) | (((uint64_t)buf[1]) << 48) | (((uint64_t)buf[2]) << 40) | (((uint64_t)buf[3]) << 32) |
               (((uint64_t)buf[4]) << 24) | (((uint64_t)buf[5]) << 16) | (((uint64_t)buf[6]) << 8) | (((uint64_t)buf[7]) << 0);
    }
    return 0;
}

ELFStatus elf_read_str(
    FILE *const fptr,
    ELFSectionHeader const *const strtab_header,
    uint64_t const strtab_offset,
    char *const buf,
    size_t const size)
{
    if (strtab_offset >= strtab_header->sh_size)
    {
        return ELF_FAIL;
    }

    if (fseek(fptr, strtab_header->sh_offset + strtab_offset, SEEK_SET) != 0)
    {
        return ELF_FAIL;
    }

    uint64_t max_size = size;
    uint64_t const dist_to_end = strtab_header->sh_size - strtab_offset;
    if (dist_to_end < max_size)
    {
        max_size = dist_to_end;
    }

    size_t nread = 0;
    while (nread < max_size)
    {
        if (fread(&buf[nread], 1, 1, fptr) != 1)
        {
            return ELF_FAIL;
        }
        if (buf[nread] == '\0')
        {
            return ELF_OK;
        }
        nread++;
    }

    return ELF_FAIL;
}

ELFStatus elf_read_header(FILE *const fptr, ELFHeader *const elf_header)
{
    int nread;
    uint8_t buf[ELF_HEADER_MAX_SIZE];
    nread = fread(buf, 1, ELF_HEADER_P1_SIZE, fptr);
    ELF_ASSERT(nread == ELF_HEADER_P1_SIZE, "Failed to read header");

    memcpy(&elf_header->e_magic, &buf[0], 4);
    memcpy(&elf_header->e_class, &buf[0x4], 1);
    memcpy(&elf_header->e_endianness, &buf[0x5], 1);
    memcpy(&elf_header->e_type, &buf[0x10], 1);

    ELF_ASSERT(memcmp(elf_header->e_magic, ELF_MAGIC, sizeof(ELF_MAGIC)) == 0, "Invalid header magic");

    ELF_ASSERT(elf_header->e_endianness == ELF_ENDIAN_LITTLE || elf_header->e_endianness == ELF_ENDIAN_BIG, "Invalid endianness");
    ELF_ASSERT(elf_header->e_class == ELF_CLASS_32BITS || elf_header->e_class == ELF_CLASS_64BITS, "Invalid class");

    ELF_ASSERT(fseek(fptr, 0, SEEK_SET) == 0, "Failed to seek in header");

    if (elf_header->e_class == ELF_CLASS_32BITS)
    {
        nread = fread(buf, 1, ELF_HEADER_32BITS_SIZE, fptr);
        ELF_ASSERT(nread == ELF_HEADER_32BITS_SIZE, "Invalid header");
        elf_header->e_phoff = read_32(elf_header->e_endianness, &buf[0x1C]);
        elf_header->e_shoff = read_32(elf_header->e_endianness, &buf[0x20]);
        elf_header->e_phentsize = read_32(elf_header->e_endianness, &buf[0x2A]);
        elf_header->e_phnum = read_16(elf_header->e_endianness, &buf[0x2C]);
        elf_header->e_shentsize = read_16(elf_header->e_endianness, &buf[0x2E]);
        elf_header->e_shnum = read_16(elf_header->e_endianness, &buf[0x30]);
        elf_header->e_shstrndx = read_16(elf_header->e_endianness, &buf[0x32]);
    }
    else if (elf_header->e_class == ELF_CLASS_64BITS)
    {
        nread = fread(buf, 1, ELF_HEADER_64BITS_SIZE, fptr);
        ELF_ASSERT(nread == ELF_HEADER_64BITS_SIZE, "Invalid header");
        elf_header->e_phoff = read_64(elf_header->e_endianness, &buf[0x20]);
        elf_header->e_shoff = read_64(elf_header->e_endianness, &buf[0x28]);
        elf_header->e_phentsize = read_32(elf_header->e_endianness, &buf[0x36]);
        elf_header->e_phnum = read_16(elf_header->e_endianness, &buf[0x38]);
        elf_header->e_shentsize = read_16(elf_header->e_endianness, &buf[0x3A]);
        elf_header->e_shnum = read_16(elf_header->e_endianness, &buf[0x3C]);
        elf_header->e_shstrndx = read_16(elf_header->e_endianness, &buf[0x3E]);
    }
    else
    {
        return ELF_FAIL;
    }
    return ELF_OK;
}

size_t elf_symbol_size(ELFHeader const *const elf_header)
{
    return (elf_header->e_class == ELF_CLASS_32BITS) ? ELF_SYMBOL_32BITS_SIZE : ELF_SYMBOL_64BITS_SIZE;
}

ELFStatus elf_read_symtab(ELFHeader const *const elf_header, ELFSectionHeader const *const sh, ELFSymbolTable *const symtab)

{
    uint32_t const symbol_size = elf_symbol_size(elf_header);
    ELF_ASSERT((sh->sh_size % symbol_size) == 0, "Invalid symbol table size");
    symtab->offset = sh->sh_offset;
    symtab->size = sh->sh_size;
    ELF_ASSERT(sh->sh_link < elf_header->e_shnum, "Invalid strtab index");
    symtab->strtab_index = sh->sh_link;

    return ELF_OK;
}