CC       = arm-none-eabi
CFLAGS   = -Wall -Wextra -Wstrict-aliasing -pedantic -Werror -O0 -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s -nostdlib -nostartfiles -std=c99 -ffreestanding 
LDFLAGS  = -Wall -Wextra -Wstrict-aliasing -pedantic -Werror -O0 -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -std=c99 -ffreestanding -L ~/CodeSourcery/Sourcery_G++_Lite/arm-none-eabi/lib -lm
ASFLAGS  =  

SRCDIRS  = .
SRCDIRS += ../sim

CFILES   = main.c
CFILES  += gpio.c
CFILES  += uart.c
CFILES  += ll_time.c
CFILES  += protocol.c
CFILES  += particle.c

ASFILES  = vectors.s
ASFILES += memcpy.S

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


INC_PARAMS = $(foreach d, $(SRCDIRS), -I$d)
OBJDIR     = .obj
BINDIR     = bin
OBJ        = $(CFILES:%.c=%.o)
OBJFILES   = $(addsuffix .o, $(ASFILES) $(CFILES))
OBJS       = $(addprefix $(OBJDIR)/,$(OBJFILES))

vpath %.c $(SRCDIRS)
vpath %.h $(SRCDIRS)
vpath %.s $(SRCDIRS)
vpath %.S $(SRCDIRS)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


gcc: bin


all: clean gcc


clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(OBJDIR)/%.s.o : %.s
	@echo
	@echo === Compiling $<
	@echo
	$(CC)-as $(ASFLAGS) $(INC_PARAMS) $< -o $@
	@echo

$(OBJDIR)/%.S.o : %.S
	@echo
	@echo === Compiling $<
	@echo
	$(CC)-as $(ASFLAGS) $(INC_PARAMS) $< -o $@
	@echo

$(OBJDIR)/%.c.o: %.c
	@echo
	@echo === Compiling $<
	@echo
	$(CC)-gcc $(CFLAGS) $(INC_PARAMS) -c $< -o $@
	@echo

elf: memmap $(OBJS)
	@echo
	@echo === Build kernel.elf
	@echo
	$(CC)-gcc $(OBJS) $(LDFLAGS)  -T memmap -o $(BINDIR)/kernel.elf
	@echo

bin: $(OBJDIR) elf
	@echo
	@echo === Extract kernel.img from kernel.elf
	@echo
	$(CC)-objcopy $(BINDIR)/kernel.elf -O binary $(BINDIR)/kernel.img
	rm $(BINDIR)/kernel.elf
	@echo
