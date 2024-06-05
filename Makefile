
UBOOT_IMAGE ?= zImage
KERNEL_IMAGE ?= Image
INITRD ?= initrd.img
DOCKER ?= docker

.PHONY: all clean eptdump

all: boot.img eptdump

eptdump:
	$(MAKE) -C eptdump


$(INITRD): Dockerfile aml_env setpart init inittab
	$(DOCKER) buildx build --platform linux/arm64 -t amlogic-partitioner -f Dockerfile . --load
	$(DOCKER) create --name amlogic-partitioner amlogic-partitioner sh
	$(DOCKER) container export amlogic-partitioner -o amlogic-partitioner.tar
	$(DOCKER) rm -f amlogic-partitioner
	$(DOCKER) rmi amlogic-partitioner
	mkdir -p amlogic-partitioner
	sudo tar -xf amlogic-partitioner.tar -C amlogic-partitioner/
	cd amlogic-partitioner/ ; sudo find . | sudo cpio -H newc -o  > ../initrd.img.cpio ; cd ..
	gzip initrd.img.cpio
	mv initrd.img.cpio.gz $(INITRD)
	sudo rm -rf amlogic-partitioner/
	rm amlogic-partitioner.tar

$(UBOOT_IMAGE): $(KERNEL_IMAGE)
	mkimage -A arm64 -O linux -T kernel -C none -a 0x108000 -e 0x108000 -d $(KERNEL_IMAGE) $(UBOOT_IMAGE)

boot.img: $(INITRD) $(UBOOT_IMAGE)
	mkbootimg --cmdline 'console=ttyAML0,115200n8 console=tty0 root=/dev/ram0 rw init=/init panic=10 loglevel=8'  --kernel $(UBOOT_IMAGE) --ramdisk $(INITRD) -o boot.img

uinitrd.img: boot.img
	mkimage -A arm64 -O linux -T ramdisk -d initrd.img uinitrd.img

clean:
	rm -rf amlogic-partitioner/ amlogic-partitioner.tar
	$(DOCKER) rm -f amlogic-partitioner || true
	$(DOCKER) rmi amlogic-partitioner || true
	make -C eptdump clean
	rm -f initrd.img
	rm -f boot.img