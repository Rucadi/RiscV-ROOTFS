
ARCH := riscv
CROSS_COMPILE := riscv64-linux-gnu-



obj-m += interrupts_from_host.o

all:
	make -C ../linux M=$(PWD) ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu-  modules

clean:
	make -C ../linux M=$(PWD) ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- clean
	