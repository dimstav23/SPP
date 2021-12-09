TARGET   = mytest 
SPPLIBDIR = ./spplib
SPPLIBOBJ = $(SPPLIBDIR)/obj/spp_hookobj.o
SRCDIR = ./src
OBJDIR = ./obj
PMDKSRCDIR = ../../pmdk/src

CC = clang
LLC = llc 
LLVMLINK = llvm-link
OPT = opt
OPTLEVEL = -O1
CFLAGS = -include $(SPPLIBDIR)/src/spp.h -emit-llvm -I$(PMDKSRCDIR)/include/
SPP_PASS = ../../pass/spp.so
LDFLAGS= -L$(PMDKSRCDIR)/debug/ -DTAG_BITS=15 -lpmem -lpmemobj -lm

SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HOOKED   = ./obj/hooked.o  
LLVMLINKED   = ./obj/llvmlinked.o  
ASFILE = ./obj/asfile.s

$(TARGET): $(ASFILE)
	$(CC) -o $@ $(LDFLAGS) $(ASFILE)
	@echo "-- Linking complete"
	@echo "-- Executable: $(TARGET)"

$(ASFILE): $(HOOKED)
	$(LLC) $(HOOKED) -o $@ 
	@echo "-- LLCing "$<" complete"

$(HOOKED): $(LLVMLINKED)
	$(OPT) -load $(SPP_PASS) -spp $(LLVMLINKED) -o $@
	@echo "-- OPTing "$(LLVMLINKED)" complete"

$(LLVMLINKED): $(OBJECTS)
	$(LLVMLINK) -o $@ $(OBJECTS) $(SPPLIBOBJ)
	@echo "-- LLVM-Linking complete "

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "-- Compiling "$<" complete"

.PHONY: clean
clean:
	rm -f $(OBJECTS) mytest


