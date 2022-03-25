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

SPPLIB := $(shell realpath .)
SRC_FOLDER := $(SPPLIB)/src/
OBJ_FOLDER := $(SPPLIB)/obj/
FILES := spp wrappers_spp

TARGET := $(addprefix $(OBJ_FOLDER), $(addsuffix .o, $(FILES)))

all: $(OBJ_FOLDER) $(TARGET)

$(OBJ_FOLDER):
	@mkdir -p $(OBJ_FOLDER)

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.c Makefile
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -rf $(OBJ_FOLDER)*
