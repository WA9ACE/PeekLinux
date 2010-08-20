DEFINE  = strdup=p_strdup
DEFINE  += NETSURF_HOMEPAGE="\"www.emobiix.com\""
DEFINE  += NETSURF_FB_RESPATH="\"netsurf\""

INCLUDE  = $(SRCDIR)/netsurf
INCLUDE += $(SRCDIR)/netdepends/libcss/include
INCLUDE += $(SRCDIR)/netdepends/libwapcaplet/include
INCLUDE += $(SRCDIR)/netdepends/libxml2/include
INCLUDE += $(SRCDIR)/netdepends/curl-7.21.1/include
INCLUDE += $(SRCDIR)/netdepends/hubbub-0.0.2/include
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include
INCLUDE += $(SRCDIR)/netdepends/libnsfb/include
INCLUDE += $(SRCDIR)/zlib
INCLUDE += $(SRCDIR)/freetype2/include
INCLUDE += $(SRCDIR)/regex
INCLUDE += $(SRCDIR)/libiconv/include

S_CONTENT := content.c fetch.c hlcache.c llcache.c urldb.c fetchers/fetch_curl.c fetchers/fetch_data.c
S_CSS := css.c dump.c internal.c select.c utils.c
S_RENDER := box.c box_construct.c box_normalise.c directory.c favicon.c font.c form.c html.c html_interaction.c html_redraw.c hubbub_binding.c imagemap.c layout.c list.c table.c textplain.c
S_UTILS := base64.c filename.c hashtable.c http.c locale.c messages.c talloc.c url.c utf8.c utils.c useragent.c findresource.c
S_DESKTOP := knockout.c options.c plot_style.c print.c search.c searchweb.c scroll.c textarea.c tree.c version.c
S_COMMON := $(addprefix content/,$(S_CONTENT)) $(addprefix css/,$(S_CSS)) $(addprefix render/,$(S_RENDER)) $(addprefix utils/,$(S_UTILS)) $(addprefix desktop/,$(S_DESKTOP))

# S_IMAGE are sources related to image management
S_IMAGE := bmp.c gif.c ico.c jpeg.c mng.c nssprite.c png.c svg.c rsvg.c
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

S_FRAMEBUFFER_FBTK := fbtk.c event.c fill.c bitmap.c user.c window.c 	\
	text.c scroll.c osk.c 

S_FRAMEBUFFER += font_freetype.c

S_FRAMEBUFFER := $(addprefix framebuffer/,$(S_FRAMEBUFFER)) $(addprefix framebuffer/fbtk/,$(S_FRAMEBUFFER_FBTK))

SOURCES := $(S_COMMON) $(S_IMAGE) $(S_BROWSER) $(S_FRAMEBUFFER) $(S_IMAGES)

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

