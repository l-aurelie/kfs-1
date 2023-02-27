# requires GNU make >4.2.0

## Vars ##
# files
NAME			:= kfs.bin
IMG_NAME	:= $(NAME:.bin=.iso)
SYM_NAME	:= $(NAME:.bin=.sym)

# iso
IMG_BUILD_DIR := .iso

# toolchain, hard-coded but we provide containers!
ARCH		:=	i686
PREFIX	:= $(ARCH)-elf
CC			:= $(PREFIX)-gcc
AS			:= $(PREFIX)-as
HAS_CC	:= $(shell $(CC) --version 2>/dev/null)

CFLAGS := \
 -g -O2 \
 -fno-builtin \
 -fno-exceptions \
 -fno-stack-protector \
 -nostdlib \
 -nodefaultlibs \
 -ffreestanding

# grub
TEST_GRUB		:= $(shell grub-file -u && xorriso -version 2>/dev/null && test -d /usr/lib/grub/i386-pc)
ifneq ($(.SHELLSTATUS),1)
HAS_GRUB		:= true
endif

# docker
DOCKER_BIN		?= docker
HAS_DOCKER		:= $(shell $(DOCKER_BIN) ps 2>/dev/null)
DOCKER_CC			:= ghcr.io/l-aurelie/i686-cc:latest
HAS_DOCKER_CC = $(shell $(DOCKER_BIN) image inspect $(DOCKER_CC) 2>/dev/null) 

# gbb
GDB_PORT	:= 4666

# sources
SRC_PATH		:= src/
SRC_ASM			= $(shell find $(SRC_PATH) -name "*.s")
SRC					= $(shell find $(SRC_PATH) -name "*.c")
HEADERS			= $(shell find $(SRC_PATH) -name "*.h")
OBJ					= $(SRC_ASM:.s=.o) $(SRC:.c=.o)
LINKER_FILE	= src/linker.ld

# helpers
ROOT_DIR			:= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

## Rulez ##
.PHONY: all use_docker run gdb clean fclean re

all:
	@$(MAKE) $(NAME)
	@$(MAKE) $(IMG_NAME)

use_docker:
ifdef HAS_CC
	@echo "[INFO] Running $(MAKECMDGOALS) in docker"
else ifdef HAS_DOCKER_CC
	@echo "[INFO] Re-running in $(DOCKER_CC)"
	$(DOCKER_BIN) run --rm -v $(ROOT_DIR):/build $(DOCKER_CC) $(MAKE) $(MAKECMDGOALS)
else ifdef HAS_DOCKER
	@echo "[INFO] $(CC) not found, pulling $(DOCKER_CC)"
	$(DOCKER_BIN) pull --platform linux/amd64 $(DOCKER_CC)
	$(MAKE) $(MAKECMDGOALS)
else
	@$(error "[ERROR] Couldn't find $(CC) or use docker")
endif

$(NAME): $(OBJ)
ifdef HAS_CC
	$(CC) $(CFLAGS) -T $(LINKER_FILE) -o $(NAME) $(OBJ) -lgcc
else
	$(MAKE) use_docker $(MAKECMDGOALS)
endif

$(IMG_NAME): $(NAME)
ifdef HAS_GRUB
	mkdir -p $(IMG_BUILD_DIR)/boot/grub
	cp $(NAME) $(IMG_BUILD_DIR)/boot
	echo "menuentry \"$(NAME:.bin=)\" { multiboot /boot/$(NAME) }" > $(IMG_BUILD_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(IMG_NAME) $(IMG_BUILD_DIR)
else
	@echo "[INFO] grub or xoriso not found, using docker"
	$(MAKE) use_docker $(MAKECMDGOALS)
endif

 %.o: %.c $(HEADERS)
ifdef HAS_CC
	$(CC) $(CFLAGS) -c $< -o $@
else
	$(MAKE) use_docker $(MAKECMDGOALS)
endif

 %.o: %.s
ifdef HAS_CC
	$(AS) $< -o $@
else
	$(MAKE) use_docker $(MAKECMDGOALS)
endif

## virtual rules ##

run:
	$(MAKE) $(IMG_NAME)
	@echo "[INFO] Qemu is running a gdb server at $(GDB_PORT)"
	qemu-system-i386 -boot d -cdrom $(IMG_NAME) \
		-m 64M \
		-display curses \
		-gdb tcp:localhost:$(GDB_PORT) # ncurses interface and gdb server

gdb:
	$(MAKE) $(NAME)
	gdb -ex "target remote localhost:$(GDB_PORT)" -ex "symbol-file $(NAME)"

clean:
	find src/ -name "*.o" -delete

fclean: clean
	$(RM) -r $(NAME) $(IMG_NAME) $(MAP_NAME) $(IMG_BUILD_DIR)

re:
	$(MAKE) fclean
	$(MAKE) all
