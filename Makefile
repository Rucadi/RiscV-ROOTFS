ARCH := riscv
CROSS_COMPILE := riscv64-linux-gnu-


rootfs.cpio.gz: rootfs configure_packages shared_mem_program
	cd rootfs &&  find . -print0 | cpio --null -ov --owner +0:+0 --format=newc > ../rootfs_only.cpio 
	cat blobs/blob.cpio rootfs_only.cpio > rootfs.cpio
	rm rootfs_only.cpio
	gzip -f rootfs.cpio
	mv rootfs.cpio.gz 




configure_packages: rootfs 
	cp `which qemu-riscv64-static` rootfs/bin/
	chroot $(realpath rootfs) qemu-riscv64-static /bin/bash gen.sh




shared_mem_program:
	${CROSS_COMPILE}gcc utils/riscv_copy_from_shared_mem.c -o rootfs/root/copy_from_shmem
	chmod a+x rootfs/root/copy_from_shmem
	cp utils/riscv_copy_from_shared_mem.c rootfs/root/riscv_copy_from_shared_mem.c 


rootfs:
	multistrap -d rootfs -f debian.config
	cp riscv_scripts/* rootfs
	sh -c "cd kernel_drivers && make"
	cp kernel_drivers/interrupts_from_host.ko rootfs/root/interrupts_from_host.ko


generate_fit:
	make -C img_gen 

clean:
	rm -rf rootfs