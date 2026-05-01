# Compiler & Linker Flags

# GCCPARAMS:
# -m32: Compile for 32-bit x86 (standard for early OS dev)
# -ffreestanding: Tells GCC there's no standard library (no stdio.h, etc.)
# -fno-stack-protector: Disables stack smashing protection (which requires an OS)
# -fno-pie: Disables position independent executable (we need fixed memory address)
GCCPARAMS = -m32 -Iinclude -ffreestanding -fno-stack-protector -fno-pie -fno-builtin -nostdlib -Wno-write-strings

# ASPARAMS:
# --32: Generate 32-bit code (GNU Assembler syntax)
ASPARAMS = --32

# LDPARAMS:
# -m elf_i386: Link as a 32-bit ELF (executable and linkable format) file
LDPARAMS = -m elf_i386

# Build target
objects = obj/loader.o \
	obj/kernel.o \
	obj/gdt.o \
	obj/drivers/driver.o \
	obj/hardwarecommunication/port.o \
	obj/hardwarecommunication/interrupts.o \
	obj/drivers/keyboard.o \
	obj/drivers/mouse.o \
	obj/interruptstubs.o

# .PHONY: tells make that these are not actual files, but commands
.PHONY: clean run run-bin run-iso install

# Rule to compile C files into objects files
obj/%.o: src/%.c
	@mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

# Rule to assemble .s files into objects files
obj/%.o: src/%.s
	@mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

# The final linking step: Uses my linker.ld script to create the binary
mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T linker.ld -o $@ $(objects)

# Install: copies the kernel binary to the system's /boot directory
install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

# Iso Generation:
# 1. Creates a directory structure compatible with GRUB
# 2. Automatically generates a grub.cfg configuration file
# 3. Uses grub-mkrescue to create a bootable .iso image
mykernel.iso: mykernel.bin
	mkdir -p iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	@echo 'set timeout=0' > iso/boot/grub/grub.cfg
	@echo 'set default=0' >> iso/boot/grub/grub.cfg
	@echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	@echo '    multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	@echo '    boot' >> iso/boot/grub/grub.cfg
	@echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso

# Utility to remove artifacts
clean:
	rm -f $(objects) mykernel.bin

# Utility to launch the kernel in the QEMU emulator
run-bin: mykernel.bin
	qemu-system-i386 -kernel mykernel.bin

# Utility to launch the OS using VirtualBox:
# 1. kill any running instances of VirtualBox to avoid lock errors
# 2. Starts the specific VM named "TeeOS" using the generated ISO
run-iso: mykernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "TeeOS"
