SOURCES = spp.cpp
LIB_NAME = spp.so

LDFLAGS = $(shell llvm-config-12 --ldflags)
CXXFLAGS = -ggdb3 -Wall -fno-rtti -fPIC -shared $(shell llvm-config-12 --cxxflags)

all: $(LIB_NAME)

$(LIB_NAME): $(SOURCES)
	g++ $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(LIB_NAME)
