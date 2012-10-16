NAME = VersatileLoader

CC = arm-eabi-gcc
LD = arm-eabi-gcc
OBJCOPY = arm-eabi-objcopy

DEFINES = 

LIBS = 

SOURCE_DIR = .
BUILD_DIR = Build

C_OPTS =	-O3 \
			-Wall \
			-Werror \
			-fmessage-length=0 \
			-mcpu=cortex-a9 \
			-fpic \
			-g3 \
			-gdwarf-2

C_FILES =	

S_FILES =	startup.S

OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o) $(S_FILES:%.S=$(BUILD_DIR)/%.o)

ALL_CFLAGS = $(C_OPTS) $(DEFINES) $(CFLAGS)
ALL_LDFLAGS =	$(LD_FLAGS) \
				-nostartfiles \
				-Wl,-T,reloc.ld \
				-mcpu=cortex-a9 \
				-fpic \
				-g3 \
				-gdwarf-2

AUTODEPENDENCY_CFLAGS=-MMD -MF$(@:.o=.d) -MT$@




all: $(NAME).bin

clean:
	rm -rf $(BUILD_DIR) $(NAME).elf $(NAME).bin

$(NAME).bin: $(NAME).elf
	$(OBJCOPY) -O binary $(NAME).elf $(NAME).bin

$(NAME).elf: $(OBJS)
	$(LD) $(ALL_LDFLAGS) -o $@ $^ $(LIBS)

.SUFFIXES: .o .c .S

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(AUTODEPENDENCY_CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

