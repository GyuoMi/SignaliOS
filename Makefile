# install dependencies
# sudo apt-get install g++ binutils libc6-dev-i386
# sudo apt-get install VirtualBox grub-legacy xorriso

# compiler and linker options
# tell compiler don't look for glib c
GCCPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386

# object files list
objects = loader.o kernel.o


# compile c++ source files to object files
%.o: %.cpp
	gcc $(GCCPARAMS) -c -o $@ $<

# assemble asm source files to object files
%.o: %.s
	as $(ASPARAMS) -o $@ $<

# link object files to create the kernel binary
mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

# create and ISO image containing kernel binary
mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "SignaliOS" 		  {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin'    >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg

# run the os in a virtualbox vm
run: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm 'SignaliOS' &

# install the kernel binary to /boot dir
install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

# clean up generated files
.PHONY: clean
clean:
	rm -f $(objects) mykernel.bin mykernel.iso
