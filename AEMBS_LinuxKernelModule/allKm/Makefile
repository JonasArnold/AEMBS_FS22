PATH_TO_KERNEL_SOURCES = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

obj-m += allKm.o

all:
	make -C $(PATH_TO_KERNEL_SOURCES) M=$(PWD) modules

clean:
	make -C $(PATH_TO_KERNEL_SOURCES) M=$(PWD) clean

