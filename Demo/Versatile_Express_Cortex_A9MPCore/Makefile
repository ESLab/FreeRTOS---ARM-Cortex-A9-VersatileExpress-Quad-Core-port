NAME = FreeRTOSDemo

CC = arm-eabi-gcc
LD = arm-eabi-gcc
OBJCOPY = arm-eabi-objcopy

DEFINES = -DPRINTF_FLOAT_SUPPORT

LIBS = -lm

SOURCE_DIR = ../..
BUILD_DIR = Build

C_OPTS =	-I"$(SOURCE_DIR)/Demo/Versatile_Express_Cortex_A9MPCore" \
			-I"$(SOURCE_DIR)/Demo/Common/include" \
			-I"$(SOURCE_DIR)/Source/include" \
			-I"$(SOURCE_DIR)/Source/portable/GCC/ARM_Cortex-A9" \
			-O3 \
			-Wall \
			-fmessage-length=0 \
			-mcpu=cortex-a9 \
			-g3 \
			-gdwarf-2 \
			-std=c99

#			-Werror \

C_FILES =	Source/croutine.c \
			Source/list.c \
			Source/queue.c \
			Source/tasks.c \
			Source/timers.c \
			Source/portable/GCC/ARM_Cortex-A9/port.c \
			Source/portable/MemMang/heap_2.c \
			Demo/Versatile_Express_Cortex_A9MPCore/main.c \
			Demo/Versatile_Express_Cortex_A9MPCore/pl011.c \
			Demo/Versatile_Express_Cortex_A9MPCore/pl031_rtc.c \
			Demo/Versatile_Express_Cortex_A9MPCore/printf-stdarg.c \
			Demo/Versatile_Express_Cortex_A9MPCore/serial.c \
			Demo/Versatile_Express_Cortex_A9MPCore/sp804_timer.c \
			Demo/Versatile_Express_Cortex_A9MPCore/uart.c \
			Demo/Common/Minimal/BlockQ.c \
			Demo/Common/Minimal/blocktim.c \
			Demo/Common/Minimal/comtest.c \
			Demo/Common/Minimal/countsem.c \
			Demo/Common/Minimal/dynamic.c \
			Demo/Common/Minimal/flop.c \
			Demo/Common/Minimal/GenQTest.c \
			Demo/Common/Minimal/integer.c \
			Demo/Common/Minimal/PollQ.c \
			Demo/Common/Minimal/QPeek.c \
			Demo/Common/Minimal/recmutex.c \
			Demo/Common/Minimal/semtest.c

S_FILES =	Demo/Versatile_Express_Cortex_A9MPCore/startup.S

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)

S_OBJS = $(S_FILES:%.S=$(BUILD_DIR)/%.o)

AUTODEPENDENCY_CFLAGS=-MMD -MF$(@:.o=.d) -MT$@

ALL_CFLAGS = $(C_OPTS) $(DEFINES) $(CFLAGS) $(AUTODEPENDENCY_CFLAGS)

ALL_LDFLAGS_BASE =	$(LD_FLAGS) \
					-nostartfiles \
					-mcpu=cortex-a9 \
					-g3 \
					-gdwarf-2

LINK_PATH = MultiCore/

ALL_LDFLAGS_0 =	$(ALL_LDFLAGS_BASE) \
				-Wl,-T,$(LINK_PATH)plain.0.ld

ALL_LDFLAGS_1 =	$(ALL_LDFLAGS_BASE) \
				-Wl,-T,$(LINK_PATH)plain.1.ld

ALL_LDFLAGS_2 =	$(ALL_LDFLAGS_BASE) \
				-Wl,-T,$(LINK_PATH)plain.2.ld

ALL_LDFLAGS_3 =	$(ALL_LDFLAGS_BASE) \
				-Wl,-T,$(LINK_PATH)plain.3.ld

AUTODEPENDENCY_CFLAGS=-MMD -MF$(@:.o=.d) -MT$@




.SUFFIXES: .o .c .bin

all: $(NAME).0.uimg $(NAME).1.uimg $(NAME).2.uimg $(NAME).3.uimg

clean:
	rm -rf $(BUILD_DIR) $(NAME).0.elf $(NAME).1.elf $(NAME).2.elf $(NAME).3.elf $(NAME).0.bin $(NAME).1.bin $(NAME).2.bin $(NAME).3.bin $(NAME).0.uimg $(NAME).1.uimg $(NAME).2.uimg $(NAME).3.uimg

qemu: $(NAME).uimg
	qemu-system-arm -M vexpress-a9 -s -S -serial stdio \
	-kernel $(NAME).uimg

qemu-run: $(NAME).uimg
	qemu-system-arm -M vexpress-a9 -serial stdio \
	-kernel $(NAME).uimg

$(NAME).0.uimg: $(NAME).0.bin
	mkimage -A arm -O linux -T kernel -C none -a 0x0010000 -e 0x0010000 \
	-d $< -n FreeRTOS.0 $@

$(NAME).1.uimg: $(NAME).1.bin
	mkimage -A arm -O linux -T kernel -C none -a 0x1010000 -e 0x1010000 \
	-d $< -n FreeRTOS.0 $@

$(NAME).2.uimg: $(NAME).2.bin
	mkimage -A arm -O linux -T kernel -C none -a 0x2010000 -e 0x2010000 \
	-d $< -n FreeRTOS.0 $@

$(NAME).3.uimg: $(NAME).3.bin
	mkimage -A arm -O linux -T kernel -C none -a 0x3010000 -e 0x3010000 \
	-d $< -n FreeRTOS.0 $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

$(NAME).0.elf: $(C_OBJS) $(S_OBJS)
	$(LD) $(ALL_LDFLAGS_0) -o $@ $^ $(LIBS)

$(NAME).1.elf: $(C_OBJS) $(S_OBJS)
	$(LD) $(ALL_LDFLAGS_1) -o $@ $^ $(LIBS)

$(NAME).2.elf: $(C_OBJS) $(S_OBJS)
	$(LD) $(ALL_LDFLAGS_2) -o $@ $^ $(LIBS)

$(NAME).3.elf: $(C_OBJS) $(S_OBJS)
	$(LD) $(ALL_LDFLAGS_3) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

-include $(C_OBJS:.o=.d)

