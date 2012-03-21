DEFINE += 

INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(SRCDIR)/zlib
INCLUDE += $(SRCDIR)/freetype2/include
INCLUDE += $(SRCDIR)/regex
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/csw-system/drv_app/ffs
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/csw-system/drv_app/ffs/reliance
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/chipsetsw/Services
INCLUDE += $(M23DIR)/CS_ThirdParty/Datalight/Reliance/include
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/sdl/include
INCLUDE += $(SRCDIR)/sdl/src
INCLUDE += $(SRCDIR)/sdl/src/video
INCLUDE += $(M23DIR)/g23m/nexgen/coreip
INCLUDE += $(SRCDIR)/

S_VIDEO := SDL_RLEaccel.c  SDL_blit_0.c  SDL_blit_A.c  SDL_bmp.c     SDL_gamma.c   SDL_stretch.c  SDL_video.c  SDL_yuv_mmx.c \
	SDL_blit.c      SDL_blit_1.c  SDL_blit_N.c  SDL_cursor.c  SDL_pixels.c  SDL_surface.c  SDL_yuv.c    SDL_yuv_sw.c
S_MAIN := src/SDL.c src/SDL_error.c  src/SDL_fatal.c

SOURCES := $(S_MAIN) $(addprefix src/video/,$(S_VIDEO))

SRC  = $(patsubst %.c,%.o,$(SOURCES))
OBJ  = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libsdl.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

clean:
	$(RM) $(TARGET) 
	$(RM) $(OBJ) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules

