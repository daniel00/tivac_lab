

#make TARGET=myapp 등으로 이름을 지정하지 않으면 기본값 main 적용
TARGET ?= main
MODE ?= debug

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


#그냥 make만 치면 down 타겟으로 진행된다.
down: printinfo $(BIN)
	@echo ""
	@echo "Downloading ... : $(BIN)"
	sudo lm4flash $(BIN)

#make all을 치면 bin까지만 만들고 다운로드는 하지 않는다.
#최종적으로 bin파일을 만들거다
all: printinfo $(BIN)

#빌드 정보를 출력
printinfo: 
	@echo "==============================================="
	@echo "Building Target: $(BIN)"
	@echo "Build Mode     : $(MODE_MSG)"
	@echo "==============================================="

#위 bin을 만들려면 , elf가 필요하다 == objcopy
$(BIN): $(ELF)
	@echo ""
	@echo "Creating Binary...: $@"
	$(OC) $(OCFLAGS) $< $@

#위 elf를 만들려면 objs 파일들이 필요하다 == Linking
$(ELF): $(OBJS)
	@echo ""
	@echo "Linking...: $@"
	$(LD) $(LDFLAGS) $^ -o $@ 

#위 objs 파일을 만드는 방법이다 == compile 
#objs안에 들어있는 파일만 만든다. #즉 위에서 제외처리(filtered-out) 한 파일은 자동으로 포함되지 않는다
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo ""
	@echo "Compile...: $< to $@"
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@


#중간에 생성된 *.o *.bin *.elf 등을 삭제한다.
clean: 
	@echo ""
	@echo "Clean build files..."
	@rm -rf $(BUILDDIR)


debug:
	@echo ""
	@echo "$(SRCS)"
	@echo "$(OBJS)"



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
