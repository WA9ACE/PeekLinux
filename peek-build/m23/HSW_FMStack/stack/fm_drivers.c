#include "fm_drivers.h"
#include "fms.h"
#include "fms_api.h"
#include "btl_config.h"

#if 0

TIFM_U16 FmFreqToIndex(TIFM_U32 freq)
{
	TIFM_U32 base_freq;
	TIFM_U16 index;

	if(FMC(band) == FM_EUROPE_US_BAND)
	{
		base_freq = BASE_FREQ_US_EUROPE;
	}
	else
	{
		base_freq = BASE_FREQ_JAPAN;
	}

	index = (TIFM_U16)(((TIFM_U32)(freq - base_freq)) / 50);

	return index;
}

TIFM_U32 FmIndexToFreq(TIFM_U16 index)
{
	TIFM_U32 base_freq;
	TIFM_U32 freq;

	if(FMC(band) == FM_EUROPE_US_BAND)
	{
		base_freq = BASE_FREQ_US_EUROPE;
	}
	else
	{
		base_freq = BASE_FREQ_JAPAN;
	}

	freq = (index * 50) + base_freq;

	return freq;
}

void fmDriverInit(void)
{
	FMC(fmDriver).gen_int_mask = MASK_MAL | MASK_STIC;
}

/* Find the next index in steps of 100KHz */
TIFM_U16 findNextIndex(TIFM_U8 dir, TIFM_U16 index)
{
	int new_index;
	TIFM_U32 last_index;
	TIFM_U8 offset;

	last_index = FMC(band) ? LAST_INDEX_JAPAN : LAST_INDEX_US_EUROPE;
	/* Check the offset in order to be aligned to the 100KHz steps */
	offset = (TIFM_U8)(index % 2);

	
	if(dir == FM_DIR_UP)
	{
		new_index = index + 2;
	}
	else
	{
		new_index = index - 2;
	}

	/* Add or subtract offset (0/1) in order to stay aligned to the 100KHz steps */
	if(new_index < 0)
	{
		new_index = last_index - offset;
	}
	else if((TIFM_U32)new_index > last_index)
	{
		new_index = 0 + offset;
	}
	return (TIFM_U16)new_index;
}


#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/


