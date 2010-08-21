DEFINE = HAVE_CONFIG_H

INCLUDE  = $(SRCDIR)/netdepends/libxml2/include
INCLUDE += $(SRCDIR)/zlib

SRC  = SAX.c
SRC += entities.c
SRC += encoding.c
SRC += error.c
SRC += parserInternals.c
SRC += parser.c
SRC += tree.c
SRC += hash.c
SRC += list.c
SRC += xmlIO.c
SRC += xmlmemory.c
SRC += uri.c
SRC += valid.c
SRC += xlink.c
SRC += HTMLparser.c
SRC += HTMLtree.c
SRC += debugXML.c
SRC += xpath.c
SRC += xpointer.c
SRC += xinclude.c
#SRC += nanohttp.c
#SRC += nanoftp.c
#SRC += DOCBparser.c
SRC += catalog.c
SRC += globals.c
SRC += threads.c
SRC += c14n.c
SRC += xmlstring.c
SRC += xmlregexp.c
SRC += xmlschemas.c
SRC += xmlschemastypes.c
SRC += xmlunicode.c
SRC += xmlreader.c
SRC += relaxng.c
SRC += dict.c
SRC += SAX2.c
SRC += xmlwriter.c
SRC += legacy.c
SRC += chvalid.c
SRC += pattern.c
SRC += xmlsave.c
#SRC += xmlmodule.c
SRC += schematron.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libxml2.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules
