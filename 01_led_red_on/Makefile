#make TARGET=myapp 등으로 이름을 지정하지 않으면 기본값 main 적용
TARGET ?= main
MODE ?= debug

# ANSI Escape Code를 사용합니다.
GREEN  = \033[0;32m
CYAN   = \033[0;36m
YELLOW = \033[0;33m
RED    = \033[0;31m
RESET  = \033[0m


SRCDIR = source
BUILDDIR = build

#최종 출력 파일 bin, elf
BIN = $(BUILDDIR)/$(TARGET).bin
ELF = $(BUILDDIR)/$(TARGET).elf

#링크시 오브젝트 파일을 통째로 전달하기 *.c 파일로 부터 *.o 파일 리스트를 문자열 형태로 뽑아낸다.
#patsubst로 치환시 %로 처리함에 유의
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

#컴파일러,링커 설정
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OC = arm-none-eabi-objcopy


#컴파일러옵션
#공통
CFLAGS = -mcpu=cortex-m4 -mthumb -Wall

#debug, releash에 따라 다르게
ifeq ($(MODE), debug)
	CFLAGS += -g -O0 -DDEBUG
	MODE_MSG = "DEBUG MODE"
else
	CFLAGS += -Os -DNDEBUG
	MODE_MSG = "RELEASE MODE"
endif


#링커 옵션
LDSCRIPT = $(SRCDIR)/tm4c123.ld
LDFLAGS = -T $(LDSCRIPT)
OCFLAGS = -O binary


#빌드 정보를 출력
BUILD_TIME  := $(shell date "+%Y-%m-%d %H:%M:%S")
GCC_VERSION := $(shell $(CC) -dumpversion)
SRC_COUNT   := $(words $(SRCS))

#Git 정보 추출 (주요 변경점 요약)
# - 변경된 파일 개수와 간략한 목록을 가져옵니다.
GIT_BRANCH  := $(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "Not a Git Repo")
GIT_CHANGES := $(shell git diff --shortstat 2>/dev/null || echo "No changes")
# 수정된 파일 이름들 중 상위 3개만 추출 (너무 많아지는 것 방지)

GIT_FILES   := $(shell git diff --name-only 2>/dev/null | head -n 3 | tr '\n' ' ')


#최종적으로 bin파일을 만들거다
# 아래처럼 하면 printinfo->BIN만들고->check_size->down_ti 를 순서대로 진행한다.
all: printinfo $(BIN) check_size down_ti


#그냥 make만 치면 down 타겟으로 진행된다.
down_ti: printinfo $(BIN) check_size
	@echo ""
	@echo "$(YELLOW)Downloading ... : $(BIN) $(RESET)"
	sudo lm4flash $(BIN)
	@echo "$(CYAN)OK$(RESET)"
	@echo ""


down_st: printinfo $(BIN) check_size
	@echo ""
	@echo "$(YELLOW)Downloading ... : $(BIN) $(RESET)"
	sudo st-flash write $(BUILDDIR)/main.bin 0x8000000
	@echo "$(CYAN)OK$(RESET)"
	@echo ""






# [정보 출력] 다니엘님의 스타일 + Git 정보 추가
printinfo:
	@echo ""
	@echo "==============================================================================="
	@echo " [$(MODE)] PROCESS STARTED"
	@echo "==============================================================================="
	@echo "  Target Name  : $(TARGET).bin"
	@echo "  Build Mode   : $(MODE)"
	@echo "  GCC Version  : $(GCC_VERSION)"
	@echo "  Source Files : $(SRC_COUNT) files"
	@echo "  Linker Script: $(LDSCRIPT)"
	@echo "  Build Time   : $(BUILD_TIME)"
	@echo "  Output Path  : $(BUILDDIR)"
	@echo "  Git Changes  : $(GIT_CHANGES)"
	@echo "  Modified     : $(GIT_FILES)..."
	@echo "==============================================================================="


# 1. 칩 사양 설정 (TM4C123GH6PM 기준)
SIZE = arm-none-eabi-size
FLASH_SIZE = 262144  # 256 KB
RAM_SIZE   = 32768   # 32 KB

# ... 기존 설정들 ...

# [SIZE] 메모리 사용량 상세 분석
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


# # # [SIZE] 메모리 점유율 확인 (임베디드 필수)
# SIZE = arm-none-eabi-size
# check_size: $(ELF)
# 	@echo ""
# 	@echo ">> Memory Usage Summary:"
# 	@$(SIZE) $<
# 	@echo "==============================================================================="



#위 bin을 만들려면 , elf가 필요하다 == objcopy
$(BIN): $(ELF)
	@echo ""
	@echo "$(YELLOW)Creating Binary...: $@ $(RESET)"
	$(OC) $(OCFLAGS) $< $@
	@echo "$(CYAN)OK$(RESET)"

#위 elf를 만들려면 objs 파일들이 필요하다 == Linking
$(ELF): $(OBJS)
	@echo ""
	@echo "$(YELLOW)Linking...: $@$(RESET)"
	$(LD) $(LDFLAGS) $^ -o $@ 
	@echo "$(CYAN)OK$(RESET)"

#위 objs 파일을 만드는 방법이다 == compile 
#objs안에 들어있는 파일만 만든다. #즉 위에서 제외처리(filtered-out) 한 파일은 자동으로 포함되지 않는다
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo ""
	@echo "$(YELLOW)Compile...: $< to $@ $(RESET)"
	@echo "$(CYAN)🔍 Changes in $<:$(RESET)"
	@# --word-diff-regex를 써서 변경된 부분만 강조하고, 불필요한 헤더는 tail로 날립니다.
	@git diff --color=always -U0 $< | tail -n +5 | head -n 5 || true
	@echo "$(CYAN)----------------------$(RESET)"
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(CYAN)OK$(RESET)"

# $(BUILDDIR)/%.o: $(SRCDIR)/%.c
# 	@echo ""
# 	@echo "$(YELLOW)Compile...: $< to $@ $(RESET)"
# 	@echo "$(CYAN)🔍 Most Recent Changes in $< (by Time):$(RESET)"
# 	@# 1. git blame으로 각 줄의 수정시간을 가져옴
# 	@# 2. 시간 순서로 역순 정렬(sort -r)
# 	@# 3. 그 중 상위 5줄만 선택(head -n 5)
# 	@git blame --incremental $< | grep -E "^author-time " | cut -d' ' -f2 > .tmp_times || true
# 	@git blame $< | sort -k3,3r | head -n 5 | sed 's/^/  [RECENT] /' || true
# 	@echo "$(CYAN)----------------------$(RESET)"
# 	@mkdir -p $(BUILDDIR)
# 	$(CC) $(CFLAGS) -c $< -o $@
# 	@echo "$(CYAN)OK$(RESET)"

#중간에 생성된 *.o *.bin *.elf 등을 삭제한다.
clean: 
	@echo ""
	@echo "$(YELLOW)Clean build files... $(RESET)"
	@rm -rf $(BUILDDIR)
	@echo "$(CYAN)OK$(RESET)"
	@echo ""


debug:
	@echo ""
	@echo "$(SRCS)"
	@echo "$(OBJS)"
	@echo ""


# 아래는 그냥 매우 간단한 버전의 Makefile
# all:
# 	@echo "compile main.c ......"
# 	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c main.c -o main.o
# 	@echo "\ncompile startup.c ......"
# 	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c startup.c -o startup.o
# 	@echo "\nlinking ......"
# 	arm-none-eabi-ld -T tm4c123.ld startup.o main.o -o final.elf
# 	@echo "\nobjcopy ......"
# 	arm-none-eabi-objcopy -O binary final.elf final.bin
# 	@echo "\ndownloading binary ......"
# 	sudo lm4flash final.bin
#
# down:
# 	@echo "\ndownloading binary ......"
# 	sudo lm4flash final.bin
#
# down/%:
# 	@echo "\ndownloading binary ......"
# 	sudo lm4flash $*

