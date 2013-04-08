BOOTNAME=boot1201.img
OBJS=kernel.o start.o idt.o main.o gdt.o process.o proc.o osliba.o global.o cliba.o sysproc.o \
     syscall.o keyboard.o keymap.o tty.o stdlib.o string.o ktime.o syshandle.o signal.o hd.o panic.o
CFLAGS=-nostdlib -fno-builtin -I include
help:
	@echo -e "build:\n\t make all \n\t make start(for bochs only) \nclean:\n\t make clean\nhelp:\n\t all|start|clean|help"
all:boot.bin loader.bin kernel.bin
	rm -rf $(BOOTNAME).img;\
		dd if=/dev/null of=$(BOOTNAME) bs=512 count=1 conv=notrunc;\
		sudo losetup /dev/loop0 $(BOOTNAME);\
		sudo mkfs.msdos /dev/loop0;\
		sudo fsck.msdos /dev/loop0;\
		sudo losetup -d /dev/loop0;\
		dd if=boot.bin of=$(BOOTNAME) bs=512 count=1 conv=notrunc;\
		dd if=/dev/zero of=$(BOOTNAME) seek=1 skip=1 bs=512 count=2879 conv=notrunc;\
		sudo mount -o loop $(BOOTNAME) /mnt/sharefolder -o fat=12;\
		sudo rm -rf /mnt/sharefolder/*
		sudo cp loader.bin /mnt/sharefolder;\
		sudo cp kernel.bin /mnt/sharefolder;\
		sudo umount /mnt/sharefolder;
boot.bin:boot.elf
	objcopy -O binary -R .comment -R .note -R .pdr -S $< $@
boot.elf:boot.o
	ld $< -o $@ -T conf/ldfile.ld
boot.o:boot/boot.s
	gcc -c $<
loader.bin:loader.elf
	objcopy -O binary -R .comment -R .note -R .pdr -S $< $@
loader.elf:loader.o
	ld $< -o $@ -T conf/lddos.ld
loader.o:boot/loader.s
	gcc -c $<
kernel.bin:$(OBJS)
	ld $(OBJS) -o $@ -Ttext 30400
kernel.o:kernel/kernel.s
	as $< -o $@
start.o:kernel/start.c
	gcc -c $< $(CFLAGS)
idt.o:kernel/idt.c
	gcc -c $< $(CFLAGS)
main.o:kernel/main.c
	gcc -c $< $(CFLAGS)
gdt.o:kernel/gdt.c
	gcc -c $< $(CFLAGS)
process.o:kernel/process.c
	gcc -c $< $(CFLAGS)
proc.o:kernel/proc.s
	gcc -c $<
osliba.o:lib/osliba.s
	gcc -c $<
global.o:utils/global.c
	gcc -c $< $(CFLAGS)
cliba.o:lib/cliba.c
	gcc -c $< $(CFLAGS)
sysproc.o:sys/sysproc.c
	gcc -c $< $(CFLAGS)
syscall.o:sys/syscall.c
	gcc -c $< $(CFLAGS)
keyboard.o:tty/keyboard.c
	gcc -c $< $(CFLAGS)
keymap.o:tty/keymap.c
	gcc -c $< $(CFLAGS)
tty.o:tty/tty.c
	gcc -c $< $(CFLAGS)
stdlib.o:lib/stdlib.c
	gcc -c $< $(CFLAGS)
string.o:lib/string.c
	gcc -c $< $(CFLAGS)
ktime.o:kernel/ktime.c
	gcc -c $< $(CFLAGS)
syshandle.o:sys/syshandle.s
	gcc -c $< $(CFLAGS)
signal.o:kernel/signal.c
	gcc -c $< $(CFLAGS)
hd.o:block/hd.c
	gcc -c $< $(CFLAGS)
panic.o:kernel/panic.c
	gcc -c $< $(CFLAGS)
start:
	bochs -f conf/bochsrc
clean:
	@find . -name "*.o" -exec rm -rf {} \; 
	@find . -name "*.elf" -exec rm -rf {} \; 
	@find . -name "*.bin" -exec rm -rf {} \; 
	@find . -name "boot*.img" -exec rm -rf {} \; 

