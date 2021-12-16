TARGET   = mytest 

# CHECK THE PATH! AND MODIFY ME! from here  ##################

SPPLIBDIR = ./spplib
SPPLIBOBJ = $(SPPLIBDIR)/obj/spp_hookobj.o
SRCDIR = ./src
OBJDIR = ./obj
PMDKSRCDIR = ../../pmdk/src
SPP_PASS = ../../pass/spp.so

CC = clang
LLC = llc 
LLVMLINK = llvm-link
OPT = opt

# MODIFY ME! up to here  ##################

#SPP_PASS = /home/poolmoo/tools/clangllvm/miu.2021.llvm.12.0/build/lib/LLVMSPP.so 

#CC = /home/poolmoo/.local/clangllvm.12.0.0/bin/clang
#LLC = /home/poolmoo/.local/clangllvm.12.0.0/bin/llc
#OPT = /home/poolmoo/.local/clangllvm.12.0.0/bin/opt
#LLVMLINK = /home/poolmoo/.local/clangllvm.12.0.0/bin/llvm-link

OPTLEVEL = -O1
CFLAGS = -include $(SPPLIBDIR)/src/spp.h -emit-llvm -I$(PMDKSRCDIR)/include/
LDFLAGS= -L$(PMDKSRCDIR)/debug/ -DTAG_BITS=15 -lpmem -lpmemobj -lm

SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HOOKED   = ./obj/hooked.o  
LLVMLINKED   = ./obj/llvmlinked.o  
ASFILE = ./obj/asfile.s

$(TARGET): $(HOOKED)
	$(CC) -x ir -o $@ $(LDFLAGS) $(HOOKED)
	@echo "-- Linking complete"
	@echo "-- Executable: $(TARGET)"

#$(ASFILE): $(HOOKED)
#	$(LLC) $(HOOKED) -o $@ 
#	@echo "-- LLCing "$<" complete"

$(HOOKED): $(LLVMLINKED)
	$(OPT) -load $(SPP_PASS) -spp $(LLVMLINKED) -o $@
	@echo "-- OPTing "$(LLVMLINKED)" complete"

$(LLVMLINKED): $(OBJECTS)
	$(LLVMLINK) -o $@ $(OBJECTS) $(SPPLIBOBJ)
	@echo "-- LLVM-Linking complete "

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "-- Compiling "$<" complete"

.PHONY: clean
clean:
	rm -f $(OBJECTS) mytest


