CC := $(shell which clang)
CFLAGS := -O2
BMI1_SUPP := $(shell lscpu | grep bmi1)
BMI2_SUPP := $(shell lscpu | grep bmi2)

ifneq ($(BMI1_SUPP),)
CFLAGS += -mbmi
endif

ifneq ($(BMI2_SUPP),)
CFLAGS += -mbmi2
endif

ifneq ($(TAG_BITS),)
CFLAGS += -DTAG_BITS=$(TAG_BITS)
endif

ifeq ($(HW), OFF)
CFLAGS += -DHW_OFF
endif

ifneq ($(FAIL_OBL_COMP),)
CFLAGS += -DFAIL_OBL_COMP
endif

ifneq ($(MOCK_SPP_FUNCS),)
CFLAGS += -DMOCK_SPP_FUNCS
endif

ifneq ($(SPP_STATS),)
CFLAGS += -DSPP_STATS
# to enable stats printing at exit
	ifneq ($(PRINT_STATS),)
	CFLAGS += -DPRINT_STATS
	endif
endif


SPPLIB := $(shell realpath .)
SRC_FOLDER := $(SPPLIB)/src/
OBJ_FOLDER := $(SPPLIB)/obj/
PMDK_INC := $(SPPLIB)/../pmdk/src/include
FILES := spp wrappers_spp

CFLAGS += -I$(PMDK_INC)

TARGET := $(addprefix $(OBJ_FOLDER), $(addsuffix .o, $(FILES)))

all: $(OBJ_FOLDER) $(TARGET)

$(OBJ_FOLDER):
	@mkdir -p $(OBJ_FOLDER)

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.c Makefile
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -rf $(OBJ_FOLDER)*
