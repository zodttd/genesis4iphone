
ifeq ($(CROSS),)
ifneq ($(X86),)
TARGET = helix_mp3_x86.a
else
CROSS = arm-linux-
endif
endif # no CROSS

ifneq ($(findstring arm,$(CROSS)),)
ifeq ($(findstring cegcc,$(CROSS)),) # cegcc has ARM built in
CFLAGS += -DARM
endif
else
CFLAGS += -D__i386__
endif

CC = $(CROSS)gcc
AS = $(CROSS)as
AR = $(CROSS)ar
CFLAGS += -Ipub -O3

TARGET ?= helix_mp3.a

OBJS += mp3dec.o mp3tabs.o
OBJS += real/bitstream.o real/buffers.o real/dct32.o real/dequant.o real/dqchan.o \
	real/huffman.o real/hufftabs.o real/imdct.o real/scalfact.o real/stproc.o \
	real/subband.o real/trigtabs.o
# asm
ifneq ($(X86),)
OBJS += real/polyphase.o
else
OBJS += real/arm/asmpoly_gcc.o
endif

all : $(TARGET)

$(TARGET) : $(OBJS)
#	$(CC) -shared -nostdlib $^ -o $@
	$(AR) rcs $@ $^

.s.o:
	$(CC) $(CFLAGS) -c $< -o $@


clean :
	$(RM) $(OBJS) $(TARGET)

