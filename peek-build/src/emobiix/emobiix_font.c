#include "emobiix_font.h"
#ifndef SIMULATOR
#include "Debug.h"
#include "balapi.h"
#include "bal_def.h"
#endif
#include "p_malloc.h"

static void *freetype_do_alloc(FT_Memory memory, long size) 
{ 
	return p_malloc(size); 
}

static void freetype_do_free(FT_Memory memory, void *block) 
{ 
	p_free(block); 
}

static void *freetype_do_realloc(FT_Memory memory, long cur_size, 
		long new_size, void *block) 
{
	return p_realloc(block, new_size);
}

FT_Library* init_freetype()
{
	static struct FT_MemoryRec_ memory = { 
		NULL,
		(FT_Alloc_Func)freetype_do_alloc,
		(FT_Free_Func)freetype_do_free,
		(FT_Realloc_Func)freetype_do_realloc
	};

	static FT_Library ft;
	int	error = FT_New_Library(&memory, &ft);
	if (error != 0)
		emo_printf("Font: New_Library() = %d", error);

    FT_Add_Default_Modules(ft);
	return &ft;
}

