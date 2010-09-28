DEFINE  = strdup=p_strdup
DEFINE += nsframebuffer
#DEFINE += fprintf=emo_fprintf
#DEFINE += NETSURF_HOMEPAGE="\"file:///home.html\""
#DEFINE += NETSURF_HOMEPAGE="\"file:///emo/index.html\""
#DEFINE += NETSURF_HOMEPAGE="\"http://72.20.1.6\""
#DEFINE += NETSURF_HOMEPAGE="\"http://www.cmw.me\""
DEFINE += NETSURF_HOMEPAGE="\"http://m.yahoo.com\""
DEFINE += NETSURF_FB_RESPATH="\"\""
DEFINE += FB_USE_FREETYPE
DEFINE += WITH_PNG
DEFINE += WITH_GIF
DEFINE += WITH_BMP
#DEFINE += messages_get\(a\)=emo_messages_get\(a,__FUNCTION__,__LINE__\)
#DEFINE += WITH_JPEG

INCLUDE  = $(SRCDIR)/netsurf/emoport
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(SRCDIR)/netsurf
INCLUDE += $(SRCDIR)/netdepends/libcss/include
INCLUDE += $(SRCDIR)/netdepends/libnsgif/include
INCLUDE += $(SRCDIR)/netdepends/libnsbmp/include
INCLUDE += $(SRCDIR)/netdepends/libjpeg
INCLUDE += $(SRCDIR)/netdepends/libwapcaplet/include
INCLUDE += $(SRCDIR)/netdepends/libxml2/include
INCLUDE += $(SRCDIR)/netdepends/curl-7.21.1/include
INCLUDE += $(SRCDIR)/netdepends/hubbub-0.0.2/include
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include
INCLUDE += $(SRCDIR)/netdepends/libnsfb/include
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
INCLUDE += $(SRCDIR)/netdepends/curl-7.21.1/emoport
INCLUDE += $(M23DIR)/g23m/nexgen/coreip
INCLUDE += $(SRCDIR)/netsurf/image
INCLUDE += $(SRCDIR)/

S_PORT := ceilf.c strcmp.c gettimeofday.c strtof.c iconv.c locale.c getcwd.c realpath.c stat.c peek_file.c
#S_CONTENT := content.c fetch.c hlcache.c llcache.c urldb.c fetchers/fetch_curl.c fetchers/fetch_data.c
S_CONTENT := content.c dirlist.c fetch.c hlcache.c llcache.c urldb.c fetchers/fetch_curl.c fetchers/fetch_data.c fetchers/fetch_file.c
S_CSS := css.c dump.c internal.c select.c utils.c
S_RENDER := box.c box_construct.c box_normalise.c favicon.c font.c form.c html.c html_interaction.c html_redraw.c hubbub_binding.c imagemap.c layout.c list.c table.c textplain.c
S_UTILS := base64.c filename.c hashtable.c http.c locale.c messages.c talloc.c url.c utf8.c utils.c useragent.c findresource.c
S_DESKTOP := knockout.c options.c plot_style.c print.c search.c searchweb.c scroll.c textarea.c tree.c version.c
S_COMMON := $(addprefix emoport/,$(S_PORT)) $(addprefix content/,$(S_CONTENT)) $(addprefix css/,$(S_CSS)) $(addprefix render/,$(S_RENDER)) $(addprefix utils/,$(S_UTILS)) $(addprefix desktop/,$(S_DESKTOP))

# S_IMAGE are sources related to image management
S_IMAGE := bmp.c gif.c ico.c jpeg.c mng.c nssprite.c svg.c rsvg.c png.c
S_IMAGE := $(addprefix image/,$(S_IMAGE))

# S_PDF are sources of the pdf plotter + the ones for paged-printing
S_PDF := pdf_plotters.c font_haru.c
S_PDF := $(addprefix desktop/save_pdf/,$(S_PDF))

# S_BROWSER are sources related to full browsers but are common
# between RISC OS, GTK, BeOS and AmigaOS builds
S_BROWSER := browser.c download.c frames.c history_core.c netsurf.c	\
	save_complete.c save_text.c selection.c textinput.c
S_BROWSER := $(addprefix desktop/,$(S_BROWSER))

# S_FRAMEBUFFER are sources purely for the framebuffer build
S_FRAMEBUFFER := gui.c framebuffer.c tree.c history.c hotlist.c 	\
        save.c schedule.c thumbnail.c misc.c bitmap.c filetype.c	\
	login.c	findfile.c localhistory.c

S_FRAMEBUFFER_FBTK := fbtk.c event.c fill.c user.c window.c bitmap.c 	\
	scroll.c osk.c text.c

S_FRAMEBUFFER += font_freetype.c

S_IMAGES := caret_image.c hand_image.c history_image.c history_image_g.c left_arrow.c left_arrow_g.c menu_image.c pointer_image.c progress_image.c reload.c reload_g.c right_arrow.c right_arrow_g.c scrolld.c scrolll.c scrollr.c scrollu.c stop_image.c stop_image_g.c throbber0.c throbber1.c throbber2.c throbber3.c throbber4.c throbber5.c throbber6.c throbber7.c throbber8.c

S_FRAMEBUFFER := $(addprefix framebuffer/,$(S_FRAMEBUFFER)) $(addprefix framebuffer/fbtk/,$(S_FRAMEBUFFER_FBTK)) $(addprefix build-Linux-framebuffer/,$(S_IMAGES))

SOURCES := $(S_COMMON) $(S_IMAGE) $(S_BROWSER) $(S_FRAMEBUFFER) 

SRC  = $(patsubst %.c,%.o,$(SOURCES))
OBJ  = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libnetsurf.a

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

