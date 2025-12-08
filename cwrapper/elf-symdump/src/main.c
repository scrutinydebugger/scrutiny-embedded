//    main.c
//        A simple tool that read a .elf and finds the value of a symbol if available. Invoked
//        like: 
//         scrutoiny-elf-symdump -i <input> -o <output>.h SYMBOL1 SYMBOL2
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "check.h"
#include "cmd.h"
#include "elf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ELFHeader elf_header;
static uint8_t fbuf[1024];

typedef struct
{
    char name[256];
    ELFSymbol symbol;
} SymbolData;

int main(int argc, char *argv[])
{

    CmdOptions args;
    parse_cmd(argc, argv, &args);

    SymbolData *wanted_symbol_data = malloc(args.symbol_count * sizeof(SymbolData));
    CHECK(wanted_symbol_data != NULL, "Cannot allocate memory");

    FILE *ifile = NULL;
#define FSEEK_ELFFILE(offset) CHECK(fseek(ifile, (offset), SEEK_SET) == 0, "Failed to seek in ELF file")
    ifile = fopen(args.ifile, "rb");
    if (ifile == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", args.ifile);
        return EXIT_FAILURE;
    }
    CHECK(ifile != NULL, "Cannot open file");
    CHECK(elf_read_header(ifile, &elf_header) == ELF_OK, "Failed to read ELF header");

    CHECK(sizeof(fbuf) >= elf_header.e_phentsize, "e_phentsize too big");
    CHECK(sizeof(fbuf) >= elf_header.e_shentsize, "e_shentsize too big");

    uint32_t symtab_count = 0;
    ELFSectionHeader *section_headers = malloc(elf_header.e_shnum * sizeof(ELFSectionHeader));
    CHECK(section_headers != NULL, "Failed to allocate memory for section headers");
    FSEEK_ELFFILE(elf_header.e_shoff);
    size_t nread = 0;
    for (uint32_t i = 0; i < elf_header.e_shnum; i++)
    {
        nread = fread(fbuf, 1, elf_header.e_shentsize, ifile);
        CHECK(nread == elf_header.e_shentsize, "Cannot read Section header");
        CHECK(
            elf_read_section_header(&elf_header, fbuf, &section_headers[i]) == ELF_OK,
            "Failed to read section header");

        if (section_headers[i].sh_type == SHT_SYMTAB)
        {
            symtab_count++;
        }
    }

    CHECK(symtab_count > 0, "No symbol table found");
    CHECK(symtab_count < 4096, "Too many symbol table found"); // Avoid consuming too much memory if file is garbage

    ELFSymbolTable *symtabs = malloc(symtab_count * sizeof(ELFSymbolTable));
    CHECK(symtabs != NULL, "Cannot allocate memory for symbol table");

    uint32_t symtab_index = 0;
    FSEEK_ELFFILE(elf_header.e_shoff);
    for (uint32_t i = 0; i < elf_header.e_shnum; i++)
    {
        ELFSectionHeader sh;
        nread = fread(fbuf, 1, elf_header.e_shentsize, ifile);
        CHECK(nread == elf_header.e_shentsize, "Cannot read Section header");
        elf_read_section_header(&elf_header, fbuf, &sh);

        if (sh.sh_type == SHT_SYMTAB)
        {
            CHECK(elf_read_symtab(&elf_header, &sh, &symtabs[symtab_index]) == ELF_OK, "Failed to read Symbol table");
            symtab_index++;
        }
    }

    size_t const symbol_size = elf_symbol_size(&elf_header);
    for (int k = 0; k < args.symbol_count; k++)
    {
        int found = 0;
        uint32_t symbol_count = 0;
        for (uint32_t i = 0; i < symtab_count; i++)
        {
            symbol_count = symtabs[i].size / symbol_size;
            for (uint32_t j = 0; j < symbol_count; j++)
            {
                ELFSymbol symbol;
                size_t fileoffset = symtabs[i].offset + j * symbol_size;
                FSEEK_ELFFILE(fileoffset);
                nread = fread(fbuf, 1, symbol_size, ifile);
                CHECK(nread == symbol_size, "Failed to read symbol");
                CHECK(elf_read_symbol(&elf_header, fbuf, &symbol) == ELF_OK, "Failed to read symbol");
                ELFSectionHeader *strtab = &section_headers[symtabs[i].strtab_index];
                ELFStatus rc = elf_read_str(
                    ifile,
                    strtab,
                    symbol.st_name,
                    wanted_symbol_data[k].name,
                    sizeof(wanted_symbol_data[k].name));
                CHECK(rc == ELF_OK, "Cannot read symbol name");
                if (strlen(wanted_symbol_data[k].name) > 0)
                {
                    if (strncmp(wanted_symbol_data[k].name, args.symbols[k], sizeof(wanted_symbol_data[k].name)) == 0)
                    {
                        if (ELF_ST_TYPE(symbol.st_info) != STT_OBJECT)
                        {
                            fprintf(stderr, "Symbol %s is not a defined variable\n", args.symbols[k]);
                            return EXIT_FAILURE;
                        }

                        if (symbol.st_shndx == 0)
                        {
                            fprintf(stderr, "Symbol %s has an undefined section\n", args.symbols[k]);
                            return EXIT_FAILURE;
                        }

                        wanted_symbol_data[k].symbol = symbol; // Copy
                        found = 1;
                    }
                }

                if (found)
                {
                    break;
                }
            }
            if (found)
            {
                break;
            }
        }
        if (!found)
        {
            fprintf(stderr, "Could not find symbol %s\n", args.symbols[k]);
            return EXIT_FAILURE;
        }
    }

    FILE *ofile = NULL;
    if (args.ofile != NULL)
    {
        ofile = fopen(args.ofile, "w");
        CHECK(ofile != NULL, "Cannot open output file");

        fprintf(ofile, "#ifndef _SCRUTINY_C_WRAPPER_CPP_CONSTANTS_H_\n");
        fprintf(ofile, "#define _SCRUTINY_C_WRAPPER_CPP_CONSTANTS_H_\n\n");
    }

    for (int k = 0; k < args.symbol_count; k++)
    {
        SymbolData const *const wanted_symbol = &wanted_symbol_data[k];
        ELFSectionHeader const *const section = &section_headers[wanted_symbol->symbol.st_shndx];

        uint64_t const value_section_offset = wanted_symbol->symbol.st_value - section->sh_addr;
        FSEEK_ELFFILE(section->sh_offset + value_section_offset);
        CHECK(
            fread(fbuf, 1, wanted_symbol->symbol.st_size, ifile) == wanted_symbol->symbol.st_size,
            "Failed to read symbol value");
        uint64_t symbol_val = 0;
        switch (wanted_symbol->symbol.st_size)
        {
        case 1:
            symbol_val = read_8(elf_header.e_endianness, fbuf);
            break;
        case 2:
            symbol_val = read_16(elf_header.e_endianness, fbuf);
            break;
        case 4:
            symbol_val = read_32(elf_header.e_endianness, fbuf);
            break;
        case 8:
            symbol_val = read_64(elf_header.e_endianness, fbuf);
            break;
        default:
            fprintf(stderr, "Unsupported symbol size 0x%lx for %s\n", wanted_symbol->symbol.st_size, args.symbols[k]);
            return EXIT_FAILURE;
        }

        if (ofile)
        {
            fprintf(ofile, "#define %s 0x%lx\n", wanted_symbol->name, symbol_val);
        }
        else
        {
            fprintf(stdout, "%s=0x%lx\n", wanted_symbol->name, symbol_val);
        }
    }

    if (ofile != NULL)
    {
        fprintf(ofile, "\n#endif\n");
        fclose(ofile);
    }
    fclose(ifile);
    return EXIT_SUCCESS;
}