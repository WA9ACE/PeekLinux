#ifndef ARMCOFF_H
#define ARMCOFF_H

#include <stdint.h>

#define F_RELFLG 	0x0001 //Relocation information was stripped from the file.
#define F_EXEC 		0x0002 //The file is relocatable (it contains no unresolved external references).
#define F_LNNO 		0x0004 //Line numbers were stripped from the file.
#define F_LSYMS 	0x0008 //Local symbols were stripped from the file.
#define F_LITTLE 	0x0100 //The file has little-endian byte ordering (least significant byte first).
#define F_BIG 		0x0200 //The file has big-endian byte ordering (most significant byte first).
#define F_SYMMERGE 	0x1000 //Duplicate symbols were removed.

#define TRG_TMS470      0x0097

typedef struct {
    uint16_t version;
    uint16_t nsec;
    uint32_t timestamp;
    uint32_t symfp;
    uint32_t nsym;
    uint16_t opthdrsz;
    uint16_t flags;
    uint16_t target;
} coff_filehdr;
#define sizeof_coff_filehdr 22

#define OPT_MAGIC	0x0108

typedef struct {
    uint16_t magic;
    uint16_t version;
    uint32_t text_size;
    uint32_t data_size;
    uint32_t bss_size;
    uint32_t entry;
    uint32_t text_addr;
    uint32_t data_addr;
} coff_opthdr;
#define sizeof_coff_opthdr 28

#define STYP_REG 	0x00000000 //Regular section (allocated, relocated, loaded)
#define STYP_DSECT 	0x00000001 //Dummy section (relocated, not allocated, not loaded)
#define STYP_NOLOAD	0x00000002 //Noload section (allocated, relocated, not loaded)
#define STYP_COPY 	0x00000010 //Copy section (relocated, loaded, but not allocated; relocation and line number entries are processed normally)
#define STYP_TEXT 	0x00000020 //Section contains executable code
#define STYP_DATA 	0x00000040 //Section contains initialized data
#define STYP_BSS 	0x00000080 //Section contains uninitialized data
#define STYP_RO         0x00000200 //Section is read-only
#define STYP_BLOCK 	0x00001000 //Alignment used as a blocking factor
#define STYP_PASS 	0x00002000 //Section should pass through unchanged
#define STYP_CLINK 	0x00004000 //Section requires conditional linking

typedef struct {
    uint8_t name[8];	/* if [0]==0, [4-7] is ptr in string table */
    uint32_t phys;
    uint32_t virt;
    uint32_t size;
    uint32_t fp;
    uint32_t relfp;
    uint32_t lnumfp;
    uint32_t nrel;
    uint32_t nlnum;
    uint32_t flags;
    uint16_t _res;
    uint16_t page;
} coff_sechdr;
#define sizeof_coff_sechdr 48

#define R_RELLONG 	0x0011 //32-bit direct reference to symbol's address
#define R_PCR23H 	0x0016 //23-bit PC-relative reference to a symbol's address, in halfwords (divided by 2)
#define R_PCR24W 	0x0017 //24-bit PC-relative reference to a symbol's address, in words (divided by 4)

typedef struct {
    uint32_t virt;
    uint16_t sym;
    uint16_t _res;
    uint16_t _res2;
    uint16_t type;
} coff_rel;
#define sizeof_coff_rel 12

typedef struct {
    uint32_t addr_or_sym;
    uint16_t lnum;
} coff_lnum;
#define sizeof_coff_lnum 6

#define C_NULL 		0   //No storage class
#define C_EXT 		2   //External definition
#define C_STAT 		3   //Static
#define C_EXTREF 	5   //External reference
#define C_LABEL 	6   //Label
#define C_ULABEL 	7   //Undefined label
#define C_USTATIC 	14  //Undefined static
#define C_UEXT 		19  //Tentative external definition
#define C_STATLAB 	20  //Static load time label
#define C_EXTLAB 	21  //External load time label
#define C_VARARG 	27  //Last declared parameter of a function with variable number of arguments
#define C_LINE 		104 //Used only by utility programs

typedef struct {
    uint8_t name[8];
    uint32_t val;
    uint16_t sect;
    uint16_t type;
    uint8_t clss;
    uint8_t auxent;
} coff_sym;
#define sizeof_coff_sym 18

typedef struct {
    uint32_t sectlen;
    uint16_t nrel;
    uint16_t nlnum;
    uint8_t _res[10];
} coff_auxent;
#define sizeof_coff_auxent 18

#endif
