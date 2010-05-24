#include "dma/dma_api.h"
#include "dma/dma_message.h"

#define R2D_LCD_DMA_IDLE 	            0
#define R2D_LCD_DMA_CHANNEL_PARAM_SET	1
#define R2D_LCD_DMA_TRANSFER_COMPLETED 2
#define R2D_LCD_DMA_ERROR					3

#define R2D_LCD_DMA_UNINITILISED       0xff

typedef UINT8 T_R2D_LCD_DMA_STATE;



