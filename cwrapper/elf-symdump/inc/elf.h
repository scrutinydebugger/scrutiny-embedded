//    elf.h
//        ELF structure and constants definitions
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern uint8_t const ELF_MAGIC[4];

typedef enum
{
    ELF_ENDIAN_LITTLE = 1,
    ELF_ENDIAN_BIG = 2
} ELFEndianess;

typedef enum
{
    ELF_CLASS_32BITS = 1,
    ELF_CLASS_64BITS = 2
} ELFClass;

typedef enum
{
    ELF_OK = 0,
    ELF_FAIL = 1
} ELFStatus;

#define ET_NONE 0        /* No file type */
#define ET_REL 1         /* Relocatable file */
#define ET_EXEC 2        /* Executable file */
#define ET_DYN 3         /* Shared object file */
#define ET_CORE 4        /* Core file */
#define ET_NUM 5         /* Number of defined types */
#define ET_LOOS 0xfe00   /* OS-specific range start */
#define ET_HIOS 0xfeff   /* OS-specific range end */
#define ET_LOPROC 0xff00 /* Processor-specific range start */
#define ET_HIPROC 0xffff /* Processor-specific range end */

#define SHT_NULL 0                    /* Section header table entry unused */
#define SHT_PROGBITS 1                /* Program data */
#define SHT_SYMTAB 2                  /* Symbol table */
#define SHT_STRTAB 3                  /* String table */
#define SHT_RELA 4                    /* Relocation entries with addends */
#define SHT_HASH 5                    /* Symbol hash table */
#define SHT_DYNAMIC 6                 /* Dynamic linking information */
#define SHT_NOTE 7                    /* Notes */
#define SHT_NOBITS 8                  /* Program space with no data (bss) */
#define SHT_REL 9                     /* Relocation entries, no addends */
#define SHT_SHLIB 10                  /* Reserved */
#define SHT_DYNSYM 11                 /* Dynamic linker symbol table */
#define SHT_INIT_ARRAY 14             /* Array of constructors */
#define SHT_FINI_ARRAY 15             /* Array of destructors */
#define SHT_PREINIT_ARRAY 16          /* Array of pre-constructors */
#define SHT_GROUP 17                  /* Section group */
#define SHT_SYMTAB_SHNDX 18           /* Extended section indeces */
#define SHT_NUM 19                    /* Number of defined types.  */
#define SHT_LOOS 0x60000000           /* Start OS-specific.  */
#define SHT_GNU_ATTRIBUTES 0x6ffffff5 /* Object attributes.  */
#define SHT_GNU_HASH 0x6ffffff6       /* GNU-style hash table.  */
#define SHT_GNU_LIBLIST 0x6ffffff7    /* Prelink library list */
#define SHT_CHECKSUM 0x6ffffff8       /* Checksum for DSO content.  */
#define SHT_LOSUNW 0x6ffffffa         /* Sun-specific low bound.  */
#define SHT_SUNW_move 0x6ffffffa
#define SHT_SUNW_COMDAT 0x6ffffffb
#define SHT_SUNW_syminfo 0x6ffffffc
#define SHT_GNU_verdef 0x6ffffffd  /* Version definition section.  */
#define SHT_GNU_verneed 0x6ffffffe /* Version needs section.  */
#define SHT_GNU_versym 0x6fffffff  /* Version symbol table.  */
#define SHT_HISUNW 0x6fffffff      /* Sun-specific high bound.  */
#define SHT_HIOS 0x6fffffff        /* End OS-specific type */
#define SHT_LOPROC 0x70000000      /* Start of processor-specific */
#define SHT_HIPROC 0x7fffffff      /* End of processor-specific */
#define SHT_LOUSER 0x80000000      /* Start of application-specific */
#define SHT_HIUSER 0x8fffffff      /* End of application-specific */

#define STT_NOTYPE 0     /* Symbol type is unspecified */
#define STT_OBJECT 1     /* Symbol is a data object */
#define STT_FUNC 2       /* Symbol is a code object */
#define STT_SECTION 3    /* Symbol associated with a section */
#define STT_FILE 4       /* Symbol's name is file name */
#define STT_COMMON 5     /* Symbol is a common data object */
#define STT_TLS 6        /* Symbol is thread-local data object*/
#define STT_NUM 7        /* Number of defined types.  */
#define STT_LOOS 10      /* Start of OS-specific */
#define STT_GNU_IFUNC 10 /* Symbol is indirect code object */
#define STT_HIOS 12      /* End of OS-specific */
#define STT_LOPROC 13    /* Start of processor-specific */
#define STT_HIPROC 15    /* End of processor-specific */

#define STB_LOCAL 0       /* Local symbol */
#define STB_GLOBAL 1      /* Global symbol */
#define STB_WEAK 2        /* Weak symbol */
#define STB_NUM 3         /* Number of defined types.  */
#define STB_LOOS 10       /* Start of OS-specific */
#define STB_GNU_UNIQUE 10 /* Unique symbol.  */
#define STB_HIOS 12       /* End of OS-specific */
#define STB_LOPROC 13     /* Start of processor-specific */
#define STB_HIPROC 15     /* End of processor-specific */

#define ELF_ST_BIND(val) (((unsigned char)(val)) >> 4)
#define ELF_ST_TYPE(val) ((val)&0xf)
#define ELF_ST_INFO(bind, type) (((bind) << 4) + ((type)&0xf))

typedef struct
{
    uint8_t e_magic[4];
    uint8_t e_class;
    uint8_t e_endianness;
    uint16_t e_type;

    uint64_t e_phoff;
    uint64_t e_shoff;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;

} ELFHeader;

typedef struct
{
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;

} ELFSectionHeader;

typedef struct
{
    uint32_t st_name;
    uint64_t st_value;
    uint64_t st_size;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
} ELFSymbol;

typedef struct
{
    uint64_t offset;
    uint64_t size;
    uint16_t strtab_index;
} ELFSymbolTable;

ELFStatus elf_read_section_header(
    ELFHeader const *const elfheader,
    uint8_t const *const data,
    ELFSectionHeader *const header);

ELFStatus elf_read_symbol(ELFHeader const *const elfheader, uint8_t const *const buf, ELFSymbol *const symbol);
ELFStatus elf_read_str(
    FILE *const fptr,
    ELFSectionHeader const *const strtab_header,
    uint64_t const strtab_offset,
    char *const buf,
    size_t const size);
ELFStatus elf_read_header(FILE *const fptr, ELFHeader *const elf_header);
ELFStatus elf_read_symtab(
    ELFHeader const *const elf_header,
    ELFSectionHeader const *const sh,
    ELFSymbolTable *const symtab);
size_t elf_symbol_size(ELFHeader const *const elf_header);

uint8_t read_8(ELFEndianess const endianness, uint8_t const buf[]);
uint16_t read_16(ELFEndianess const endianness, uint8_t const buf[]);
uint32_t read_32(ELFEndianess const endianness, uint8_t const buf[]);
uint64_t read_64(ELFEndianess const endianness, uint8_t const buf[]);

#endif