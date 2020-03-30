obj-m += kflags.o

all: build load

build: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

load:
	sudo rmmod kflags.ko || true
	sudo insmod kflags.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
