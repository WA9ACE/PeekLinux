#include "emobiix_font.h"
#include "Debug.h"
#include "p_malloc.h"

static void *freetype_do_alloc(FT_Memory memory, long size) 
{ 
	return malloc(size); 
}

static void freetype_do_free(FT_Memory memory, void *block) 
{ 
	return free(block); 
}

static void *freetype_do_realloc(FT_Memory memory, long cur_size, 
		long new_size, void *block) 
{
	void *pNew = freetype_do_alloc(memory, new_size);
	memset(pNew, 0, new_size);
	if (block != NULL) {
		memcpy(pNew, block, cur_size);
		freetype_do_free(memory, block);
	}
	return pNew;
}

extern "C" FT_Library* init_freetype()
{
	static FT_MemoryRec_ memory = { 
		NULL,
		(FT_Alloc_Func)freetype_do_alloc,
		(FT_Free_Func)freetype_do_free,
		(FT_Realloc_Func)freetype_do_realloc
	};

	static FT_Library ft;
	int	error = FT_New_Library(&memory, &ft);
	if (error != 0)
		emo_printf("EMOBIIX: FT_New_Library() = %d", error);

  FT_Add_Default_Modules(ft);
	return &ft;
}

