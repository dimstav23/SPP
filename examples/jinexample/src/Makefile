SPPLIBDIR = "../spplib/"

src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d)

CFLAGS = "-include ${SPPLIGDIR}/src/spp.h" 
LDFLAGS= -lm

mytest: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) mytest

.PHONY: cleandep
cleandep:
	rm -f $(dep)

