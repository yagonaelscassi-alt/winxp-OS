# WinXP-OS - pensado para Debian 9 (stretch): gcc 6.3, binutils 2.28, grub 2.02
# Dependencias: sudo apt-get install build-essential gcc-multilib grub-pc-bin xorriso mtools qemu-system-x86

CC      := gcc
AS      := gcc
CFLAGS  := -m32 -std=gnu99 -ffreestanding -fno-pic -fno-stack-protector \
           -fno-builtin -O2 -Wall -Wextra
LDFLAGS := -m32 -no-pie -nostdlib -Wl,-z,noexecstack -T linker.ld -Wl,-m,elf_i386 -Wl,--build-id=none

BUILD := build
OBJS  := $(BUILD)/boot.o $(BUILD)/kernel.o
KERNEL := $(BUILD)/winxp.bin
ISO   := $(BUILD)/winxp.iso

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: src/boot.S | $(BUILD)
	$(AS) -m32 -c $< -o $@

$(BUILD)/kernel.o: src/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

check: $(KERNEL)
	grub-file --is-x86-multiboot $(KERNEL) && echo "multiboot OK"

$(ISO): $(KERNEL)
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(KERNEL) $(BUILD)/iso/boot/winxp.bin
	cp grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(BUILD)/iso

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -m 128

clean:
	rm -rf $(BUILD)

.PHONY: all check run clean
