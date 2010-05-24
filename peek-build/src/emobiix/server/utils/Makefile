TARGET = utils.a

INCLUDE += ./include

OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET) $^; $(RANLIB) $@

clean:
	$(RM) $(TARGET) *.o *.d

include $(BASEDIR)/Makefile.rules
