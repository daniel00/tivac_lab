# TM4C123 Examples Makefile
TARGET ?= main
MODE ?= debug
APP ?= 01_led_test

# ANSI Escape Code
GREEN  = \033[0;32m
CYAN   = \033[0;36m
YELLOW = \033[0;33m
RED    = \033[0;31m
RESET  = \033[0m

# 경로 설정
SRCDIR = $(APP)/source
BUILDDIR = $(APP)/build

# 출력 파일
BIN = $(BUILDDIR)/$(TARGET).bin
ELF = $(BUILDDIR)/$(TARGET).elf

# 소스 파일
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

# 컴파일러/링커
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# TM4C123 설정
CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -DTM4C123
LDSCRIPT = $(SRCDIR)/tm4c123.ld
LDFLAGS = -T $(LDSCRIPT) -nostdlib -Wl,--gc-sections
OCFLAGS = -O binary
FLASH_SIZE = 262144
RAM_SIZE   = 32768

# 빌드 모드
ifeq ($(MODE), debug)
	CFLAGS += -g -O0 -DDEBUG
	MODE_MSG = "DEBUG"
else
	CFLAGS += -Os -DNDEBUG
	MODE_MSG = "RELEASE"
endif

# 빌드 정보
BUILD_TIME  := $(shell date "+%Y-%m-%d %H:%M:%S")
GCC_VERSION := $(shell $(CC) -dumpversion)
SRC_COUNT   := $(words $(SRCS))
GIT_CHANGES := $(shell git diff --shortstat 2>/dev/null || echo "No changes")
GIT_FILES   := $(shell git diff --name-only 2>/dev/null | head -n 3 | tr '\n' ' ')

# ============ 메인 타겟 ============
all: printinfo $(BIN) check_size flash

build: printinfo $(BIN) check_size

flash: $(BIN)
	@echo ""
	@echo "$(YELLOW)Downloading to TM4C123 ... : $(BIN) $(RESET)"
	sudo lm4flash $(BIN)
	@echo "$(CYAN)OK$(RESET)"
	@echo ""

printinfo:
	@echo ""
	@echo "==============================================================================="
	@echo " [TM4C123] [$(MODE_MSG)] - $(APP)"
	@echo "==============================================================================="
	@echo "  Target Name  : $(TARGET).bin"
	@echo "  Build Mode   : $(MODE_MSG)"
	@echo "  GCC Version  : $(GCC_VERSION)"
	@echo "  Source Files : $(SRC_COUNT) files"
	@echo "  Build Time   : $(BUILD_TIME)"
	@echo "  Output Path  : $(BUILDDIR)"
	@echo "==============================================================================="

check_size: $(ELF)
	@echo ""
	@echo "$(YELLOW)>> Memory Usage Analysis: $(RESET)"
	@$(SIZE) $<
	@echo "==============================================================================="
	@$(eval TEXT := $(shell $(SIZE) $< | awk 'NR==2 {print $$1}'))
	@$(eval DATA := $(shell $(SIZE) $< | awk 'NR==2 {print $$2}'))
	@$(eval BSS  := $(shell $(SIZE) $< | awk 'NR==2 {print $$3}'))
	@$(eval FLASH_USED := $(shell echo $$(($(TEXT) + $(DATA)))))
	@$(eval RAM_USED   := $(shell echo $$(($(DATA) + $(BSS)))))
	@$(eval FLASH_FREE := $(shell echo $$(($(FLASH_SIZE) - $(FLASH_USED)))))
	@$(eval RAM_FREE   := $(shell echo $$(($(RAM_SIZE) - $(RAM_USED)))))
	@$(eval FLASH_PCT  := $(shell echo $$(($(FLASH_USED) * 100 / $(FLASH_SIZE)))))
	@$(eval RAM_PCT    := $(shell echo $$(($(RAM_USED) * 100 / $(RAM_SIZE)))))
	@echo "  Flash: $(FLASH_USED) / $(FLASH_SIZE) bytes ($(FLASH_PCT)%)"
	@echo "         Remaining: $(FLASH_FREE) bytes"
	@echo "  RAM:   $(RAM_USED) / $(RAM_SIZE) bytes ($(RAM_PCT)%)"
	@echo "         Remaining: $(RAM_FREE) bytes"
	@echo "==============================================================================="

$(BIN): $(ELF)
	@echo ""
	@echo "$(YELLOW)Creating Binary...: $@ $(RESET)"
	$(OC) $(OCFLAGS) $< $@
	@echo "$(CYAN)OK$(RESET)"

$(ELF): $(OBJS)
	@echo ""
	@echo "$(YELLOW)Linking...: $@$(RESET)"
	$(LD) $(LDFLAGS) $^ -o $@ 
	@echo "$(CYAN)OK$(RESET)"

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo ""
	@echo "$(YELLOW)Compile...: $< to $@ $(RESET)"
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(CYAN)OK$(RESET)"

clean: 
	@echo ""
	@echo "$(YELLOW)Clean build files... $(RESET)"
	@rm -rf $(BUILDDIR)
	@echo "$(CYAN)OK$(RESET)"
	@echo ""

# ============ 동적 타겟 처리 ============
# 폴더명으로 된 타겟이 들어오면 자동으로 APP으로 설정
.DEFAULT_GOAL := help

%: force
	@if [ -d "$@" ] && [ -f "$@/source/main.c" ]; then \
		$(MAKE) APP=$@ all; \
	else \
		echo "Error: Unknown target or example directory not found: $@"; \
	fi

force: ;

help:
	@echo "Usage: make <example_name>"
	@echo "Examples:"
	@ls -d */ 2>/dev/null | sed 's#/##' | awk '{print "  make " $$1}'

# 예제 디렉토리를 phony로 선언
.PHONY: all build flash printinfo check_size clean help force
