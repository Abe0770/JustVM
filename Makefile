flags := -O2 -std=c2x
ldflags := # -lbu (uncomment if you need it)

PWD := $(shell pwd)

UTILS_DIR := $(PWD)/utils
OBJ_DIR := $(PWD)/obj

UTILS_OBJ := $(UTILS_DIR)/utils.o
JUSTVM_OBJ := justvm.o

.PHONY: all clean

all: justvm

$(UTILS_OBJ): $(UTILS_DIR)/utils.c $(UTILS_DIR)/utils.h
	(cd $(UTILS_DIR) && make)

justvm: $(JUSTVM_OBJ) $(UTILS_OBJ)
	cc $(flags) $^ -o $@ $(ldflags)

justvm.o: justvm.c justvm.h
	cc $(flags) -c $<

clean:
	rm -f *.o justvm $(UTILS_DIR)/*.o
