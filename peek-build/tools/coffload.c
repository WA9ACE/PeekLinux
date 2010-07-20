#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "armcoff.h"

#define COFF_DIAG(...) fprintf(stderr, __VA_ARGS__)

typedef struct {
    uint8_t *data;
    uint32_t size;
} coff_ctx;

#define SC_NONE   0
#define SC_TEXT   1
#define SC_RODATA 2
#define SC_DATA   3
#define SC_BSS    4

typedef struct {
    coff_sechdr hdr;
    int clss;
    uint32_t rma;
} coff_sec;

uint32_t coff_get_extern(char *str)
{
    return 0;
}

void coff_reader(void *data, int offset, int size, void *buf)
{
    memcpy(buf, ((uint8_t *)data)+offset, size);
}

char *coff_getstr(char *strtab, uint8_t *name)
{
    char *ptr;
    uint32_t off;

    if(name[0]) {
	ptr = malloc(9);
	ptr[8] = 0;
	memcpy(ptr, name, 8);
	return ptr;
    }

    memcpy(&off, name+4, 4);
    ptr = malloc(strlen(strtab + off)+1);
    strcpy(ptr, strtab+off);
    return ptr;
}

void *coff_load(void *data)
{
    coff_ctx *c = calloc(1, sizeof(coff_load));
    coff_filehdr filehdr;
    coff_sec *secs;
    coff_rel rel;
    coff_sym *syms;
    char *strtab;
    int sptr, sec, len, i, errf=0, errl=0;
    uint32_t csize[4]={0,0,0,0};
    uint32_t cbase[4]={0,0,0,0};
    uint32_t ref, off, vma_base, src, dst, tmp;
    char *name;

    coff_reader(data, 0, sizeof_coff_filehdr, &filehdr);
    if(filehdr.target != TRG_TMS470) {
	COFF_DIAG("COFF file not for TMS470 (target = 0x%04x)\n", filehdr.target);
	free(c);
	return NULL;
    }
    if(filehdr.flags & F_RELFLG) {
	COFF_DIAG("COFF file has no relocation records\n");
	free(c);
	return NULL;
    }

    sptr = filehdr.symfp + filehdr.nsym*sizeof_coff_sym;
    coff_reader(data, sptr, 4, &len);
    if(len) {
	strtab = malloc(len);
	coff_reader(data, sptr, len, strtab);
    } else
	strtab = NULL;

    secs = calloc(sizeof(coff_sec), filehdr.nsec);

    sptr = sizeof_coff_filehdr + filehdr.opthdrsz;
    for(sec=0; sec<filehdr.nsec; sec++) {
	coff_reader(data, sptr+sec*sizeof_coff_sechdr, sizeof_coff_sechdr, &(secs[sec].hdr));

	if(secs[sec].hdr.flags & STYP_TEXT)
	    secs[sec].clss = SC_TEXT;
	else if((secs[sec].hdr.flags & STYP_DATA) && (secs[sec].hdr.flags & STYP_RO))
	    secs[sec].clss = SC_RODATA;
	else if(secs[sec].hdr.flags & STYP_DATA)
	    secs[sec].clss = SC_DATA;
	else if(secs[sec].hdr.flags & STYP_BSS)
	    secs[sec].clss = SC_BSS;

	if(secs[sec].clss) {
	    secs[sec].rma = csize[secs[sec].clss-1];
	    csize[secs[sec].clss-1] = (csize[secs[sec].clss-1] + secs[sec].hdr.size + 15) & ~15;
	}
    }

    for(i=1; i<4; i++)
	cbase[i] = cbase[i-1] + csize[i-1];
    c->size = cbase[3] + csize[3];
    c->data = malloc(c->size);
    vma_base = 0x50000;
//    vma_base = (uint32_t)(c->data);

    /* Load sections */
    for(sec=0; sec<filehdr.nsec; sec++)
	if(secs[sec].clss) {
	    secs[sec].rma += cbase[secs[sec].clss-1];
	    if(secs[sec].clss!=SC_BSS && secs[sec].hdr.size)
		coff_reader(data, secs[sec].hdr.fp, secs[sec].hdr.size, c->data+secs[sec].rma);
	    else if(secs[sec].hdr.size)
		memset(c->data+secs[sec].rma, 0, secs[sec].hdr.size);
	}

    syms = malloc(sizeof(coff_sym)*filehdr.nsym);
    /* Load Symbol table */
    for(i=0; i<filehdr.nsym; i++) {
	coff_reader(data, filehdr.symfp+i*sizeof_coff_sym, sizeof_coff_sym, syms+i);
	syms[i].type = 0;
	i += syms[i].auxent;
    }

    for(sec=0; sec<filehdr.nsec; sec++)
	if(secs[sec].clss)
	    for(i=0; i<secs[sec].hdr.nrel; i++) {
		coff_reader(data, secs[sec].hdr.relfp+sizeof_coff_rel*i, sizeof_coff_rel, &rel);
		if(rel.sym != 0xFFFF) {
		    if(!syms[rel.sym].type) {
			if(syms[rel.sym].sect) {
			    syms[rel.sym].val += vma_base + secs[syms[rel.sym].sect-1].rma - secs[syms[rel.sym].sect-1].hdr.virt;
			} else {
			    name = coff_getstr(strtab, syms[rel.sym].name);
			    syms[rel.sym].val = coff_get_extern(name);
			    if(!syms[rel.sym].val) {
				COFF_DIAG("Missing reference: '%s'\n", name);
				errf ++;
			    }
			    free(name);
			}
			syms[rel.sym].type = 1;
		    }
		    ref = syms[rel.sym].val;
		    name = coff_getstr(strtab, syms[rel.sym].name);
		} else {
		    ref = vma_base + secs[sec].rma - secs[sec].hdr.virt;
		    name = strdup("*local*");
		}
		off = rel.virt + secs[sec].rma - secs[sec].hdr.virt;
		switch(rel.type) {
		case R_RELLONG:
		    if(off & 3) {
			COFF_DIAG("R_RELLONG encountered at unaligned offset 0x%X\n", off);
			errl ++;
		    }
		    *(uint32_t *)(c->data+off) += ref;
		    break;
		case R_PCR23H:
		    if(off & 1) {
			COFF_DIAG("R_PCR23H encountered at unaligned offset 0x%X\n", off);
			errl ++;
		    }
		    tmp = (((uint16_t *)(c->data+off))[1] & 0x07FF) |
		          ((((uint16_t *)(c->data+off))[0] & 0x07FF) << 11);
		    src = (ref - (vma_base + secs[sec].rma - secs[sec].hdr.virt)) >> 1;
		    dst = (tmp + src) & 0x3FFFFF;
		    ((uint16_t *)(c->data+off))[1] = ((((uint16_t *)(c->data+off))[1]) & 0xF800) | (dst & 0x07FF);
		    ((uint16_t *)(c->data+off))[0] = ((((uint16_t *)(c->data+off))[0]) & 0xF800) | ((dst>>11) & 0x07FF);
		    break;
		default:
		    COFF_DIAG("Invalid relocation type: %d\n", rel.type);
		    errl ++;
		}
	    }

    for(i=0; i<filehdr.nsym; i++) {
	if(syms[i].clss==C_EXT || syms[i].clss==C_UEXT) {
	    name = coff_getstr(strtab, syms[i].name);
	    if(!syms[i].type) {
		if(syms[i].sect) {
		    syms[i].val += vma_base + secs[syms[i].sect-1].rma - secs[syms[i].sect-1].hdr.virt;
		    syms[i].type = 1;
		} else {
		    COFF_DIAG("Symbol '%s' declared for export but not defined\n");
		    free(name);
		}
	    }
	    if(syms[i].type) {
		printf("%3d %s %08x\n", syms[i].clss, name, syms[i].val);
	    }
	}
	i += syms[i].auxent;
    }

    free(secs);
    free(syms);
    free(strtab);

    if(errf)
	COFF_DIAG("Object has %d unresolved references.\n", errf);
    if(errl)
	COFF_DIAG("Object has %d undefined relocations.\n", errl);
    if(errf || errl) {
	free(c);
	return NULL;
    }

    return c;
}

int main(int argc, const char* argv[])
{
    void *buf;
    int len;
    FILE *fp;

    fp = fopen(argv[1], "r");
    if(!fp) {
	printf("Supply coff image\n");
	return -1;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf = malloc(len);
    fread(buf, 1, len, fp);
    fclose(fp);

    coff_load(buf);

    return 0;
}
